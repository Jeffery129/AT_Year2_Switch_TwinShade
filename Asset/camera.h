// =========================================================
// camera.h カメラ制御
//
// 制作者:        日付：
// =========================================================
#ifndef _CAMERA_H_
#define _CAMERA_H_

// =========================================================
// 構造体宣言
// =========================================================
struct CAMERA
{
	Float2 pos;			// 現在のカメラ座標
	Float2 targetPos;	// 追従する目標座標
};

// =========================================================
// プロトタイプ宣言
// =========================================================
void InitializeCamera(Float2 startPos);
void UpdateCamera(Float2 targetPos);
void SetCameraAimOffset(Float2 aimDir, bool isAiming);
void StartCameraShake(float power, int frame);
void FinalizeCamera(void);

CAMERA* GetCamera(void);

void ResetCamera(Float2 startPos);

#endif