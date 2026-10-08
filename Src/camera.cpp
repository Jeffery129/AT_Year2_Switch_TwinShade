// =========================================================
// camera.cpp カメラ制御
//
// 制作者:        日付：
// =========================================================
#include "main.h"
#include "camera.h"
#include "sprite.h"
#include "block.h"

// =========================================================
// マクロ定義
// =========================================================
#define CAMERA_LERP_RATE		(0.15f)   //cameraTargetPosに追いかけるスピード
#define CAMERA_AIM_OFFSET		(170.0f)  //Aim状態でcameraのオフセット

#define CAMERA_SHAKE_RANDOM_RANGE (1000)

// =========================================================
// グローバル変数
// =========================================================
CAMERA camera;

Float2 g_CameraAimDir;
bool g_IsCameraAiming;

float g_CameraShakePower;
int g_CameraShakeFrame;

// =========================================================
// プロトタイプ宣言
// =========================================================
float CameraLerp(float start, float end, float rate);
float ClampCameraValue(float value, float minValue, float maxValue);
Float2 ClampCameraPosition(Float2 pos);
void CameraShakeUpdate();

// =========================================================
// カメラ初期化
// =========================================================
void InitializeCamera(Float2 startPos)
{
	ResetCamera(startPos);
}

// =========================================================
// カメラ更新
// =========================================================
void UpdateCamera(Float2 targetPos)
{
	camera.targetPos = targetPos;

	if (g_IsCameraAiming)
	{
		camera.targetPos.x += g_CameraAimDir.x * CAMERA_AIM_OFFSET;
		camera.targetPos.y += g_CameraAimDir.y * CAMERA_AIM_OFFSET;
	}

	// 目標座標をマップ範囲内に固定
	camera.targetPos = ClampCameraPosition(camera.targetPos);

	// プレイヤーを滑らかに追従
	camera.pos.x = CameraLerp(camera.pos.x, camera.targetPos.x, CAMERA_LERP_RATE);
	camera.pos.y = CameraLerp(camera.pos.y, camera.targetPos.y, CAMERA_LERP_RATE);

	// 補間後もマップ範囲内に固定
	camera.pos = ClampCameraPosition(camera.pos);

	// Camera Shake
	if (g_CameraShakeFrame > 0)
	{
		CameraShakeUpdate();
	}
	else
	{
		SetCameraPosition(camera.pos.x, camera.pos.y);
	}
}

// =========================================================
// Aim中のカメラオフセット設定
// =========================================================
void SetCameraAimOffset(Float2 aimDir, bool isAiming)
{
	g_CameraAimDir = aimDir;
	g_IsCameraAiming = isAiming;
}

// =========================================================
// Camera Shake開始
// =========================================================
void StartCameraShake(float power, int frame)
{
	if (power <= 0.0f || frame <= 0)
	{
		return;
	}

	g_CameraShakePower = power;
	g_CameraShakeFrame = frame;
}
// =========================================================
// カメラ終了処理
// =========================================================
void FinalizeCamera(void)
{
	camera.pos = MakeFloat2(0.0f, 0.0f);
	camera.targetPos = MakeFloat2(0.0f, 0.0f);

	g_CameraAimDir = MakeFloat2(0.0f, 0.0f);
	g_IsCameraAiming = false;

	SetCameraPosition(0.0f, 0.0f);
}

// =========================================================
// カメラ取得
// =========================================================
CAMERA* GetCamera(void)
{
	return &camera;
}

// =========================================================
// 線形補間
// =========================================================
float CameraLerp(float start, float end, float rate)
{
	return start + (end - start) * rate;
}

// =========================================================
// 値の範囲制限
// =========================================================
float ClampCameraValue(float value, float minValue, float maxValue)
{
	if (value < minValue)
	{
		return minValue;
	}

	if (value > maxValue)
	{
		return maxValue;
	}

	return value;
}

// =========================================================
// マップ範囲内にカメラ座標を制限
// =========================================================
Float2 ClampCameraPosition(Float2 pos)
{
	STAGE_MAP_SIZE mapSize = GetStageMapSize();
	float mapWidth = mapSize.columnCnt * MAP_BLOCK_WIDTH;
	float mapHeight = mapSize.rowCnt * MAP_BLOCK_HEIGHT;

	// マップ左端が -SCREEN_WIDTH / 2
	// マップ上端が -SCREEN_HEIGHT / 2
	// カメラ中心の最小座標は 0 になる
	float cameraMinX = 0.0f;
	float cameraMinY = 0.0f;

	float cameraMaxX = mapWidth - SCREEN_WIDTH;
	float cameraMaxY = mapHeight - SCREEN_HEIGHT;

	// マップが画面より小さい場合
	if (cameraMaxX < cameraMinX)
	{
		pos.x = (mapWidth - SCREEN_WIDTH) * 0.5f;
	}
	else
	{
		pos.x = ClampCameraValue(pos.x, cameraMinX, cameraMaxX);
	}

	if (cameraMaxY < cameraMinY)
	{
		pos.y = (mapHeight - SCREEN_HEIGHT) * 0.5f;
	}
	else
	{
		pos.y = ClampCameraValue(pos.y, cameraMinY, cameraMaxY);
	}

	return pos;
}

// =========================================================
// Camera Shake更新
// =========================================================
void CameraShakeUpdate()
{
	float shakeX =
		static_cast<float>(rand() % (CAMERA_SHAKE_RANDOM_RANGE * 2 + 1) - CAMERA_SHAKE_RANDOM_RANGE ) /
		static_cast<float>(CAMERA_SHAKE_RANDOM_RANGE);

	float shakeY =
		static_cast<float>(rand() % (CAMERA_SHAKE_RANDOM_RANGE * 2 + 1) - CAMERA_SHAKE_RANDOM_RANGE) /
		static_cast<float>(CAMERA_SHAKE_RANDOM_RANGE);

	Float2 shakePos = MakeFloat2(
		camera.pos.x + shakeX * g_CameraShakePower,
		camera.pos.y + shakeY * g_CameraShakePower
	);

	// 最終描画位置をマップ範囲内に制限
	shakePos = ClampCameraPosition(shakePos);

	SetCameraPosition(shakePos.x, shakePos.y);

	g_CameraShakeFrame--;
	if (g_CameraShakeFrame <= 0)
	{
		g_CameraShakeFrame = 0;
		g_CameraShakePower = 0.0f;

		SetCameraPosition(camera.pos.x, camera.pos.y);
	}
}
// =========================================================
// カメラリセット
// =========================================================
void ResetCamera(Float2 startPos)
{
	camera.pos = ClampCameraPosition(startPos);
	camera.targetPos = camera.pos;

	g_CameraAimDir = MakeFloat2(0.0f, 0.0f);
	g_IsCameraAiming = false;

	g_CameraShakePower = 0.0f;
	g_CameraShakeFrame = 0;

	SetCameraPosition(camera.pos.x, camera.pos.y);
}