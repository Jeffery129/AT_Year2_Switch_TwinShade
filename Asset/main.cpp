// ===================================================
// main.cpp GM21制作用プロジェクト
//  
// 制作者：		日付：
// ===================================================
#include "main.h"
#include "title.h"
#include "texture.h"
#include "sprite.h"
#include "controller.h"
#include "game.h"
#include "fade.h"
#include "sound.h"
#include "select.h"
#include "result.h"
#include "save_data.h"

// =========================================================
// プロトタイプ宣言
// =========================================================
void Initialize(void);
void Update(void);
void Draw(void);
void Finalize(void);

//グローバル変数
SCENE	scene; // 実行中のシーン番号
GAME_STAGE stage; // ゲームステージ番号

// =========================================================
// main関数
// =========================================================
extern "C" void nnMain()
{
	// 初期化
	Initialize();

	while (true)
	{
		// 更新
		Update();

		// 描画
		Draw();
	}

	// 終了処理
	Finalize();
}

// =========================================================
// 初期化
// =========================================================
void Initialize(void)
{
	// システム系初期化
	InitSound();
	InitSystem();
	InitSprite();
	InitController();
	InitializeFade();
	InitializeSaveData();

	// 開始シーンの設定
	scene = SCENE_TITLE;

	// Stageの初期化
	stage = GAME_STAGE_T_01;

	// オブジェクト初期化
	switch (scene)
	{
	case SCENE_TITLE:
		InitializeTitle();
		break;

	case SCENE_SELECT:
		InitializeSelect();
		break;

	case SCENE_GAME:
		InitializeGame();
		break;

	case SCENE_RESULT:
		InitializeResult();
		break;

	default:
		break;
	}
}

// =========================================================
// 更新
// =========================================================
void Update(void)
{
	// Sound更新
	UpdateSound();

	// システム系更新
	UpdateController();

	// オブジェクト更新
	switch (scene)
	{
	case SCENE_TITLE:
		UpdateTitle();
		break;

	case SCENE_SELECT:
		UpdateSelect();
		break;

	case SCENE_GAME:
		UpdateGame();
		break;

	case SCENE_RESULT:
		UpdateResult();
		break;

	default:
		break;
	}

	UpdateFade();
}

// =========================================================
// 描画
// =========================================================
void Draw(void)
{
	// クリア色設定
	glClearColor(0.0f, 0.0f, 0.5f, 1.0f);

	// 画面クリア
	glClear(
		GL_COLOR_BUFFER_BIT |
		GL_DEPTH_BUFFER_BIT
	);

	// オブジェクト描画
	switch (scene)
	{
	case SCENE_TITLE:
		DrawTitle();
		break;

	case SCENE_SELECT:
		DrawSelect();
		break;

	case SCENE_GAME:
		DrawGame();
		break;

	case SCENE_RESULT:
		DrawResult();
		break;

	default:
		break;
	}

	DrawFade();

	glFinish();
	SwapBuffers();
}

// =========================================================
// 終了処理
// =========================================================
void Finalize(void)
{
	// オブジェクト終了処理
	switch (scene)
	{
	case SCENE_TITLE:
		FinalizeTitle();
		break;

	case SCENE_SELECT:
		FinalizeSelect();
		break;

	case SCENE_GAME:
		FinalizeGame();
		break;

	case SCENE_RESULT:
		FinalizeResult();
		break;

	default:
		break;
	}

	FinalizeFade();
	FinalizeSaveData();

	// システム系終了処理
	UninitController();
	UninitSprite();
	UninitSystem();
	UninitSound();
}

// =========================================================
// Scene変更
// =========================================================
void SetScene(SCENE next)
{
	//現在のシーンを終了させる
	switch (scene)
	{
	case SCENE_TITLE:
		FinalizeTitle();
		break;

	case SCENE_SELECT:
		FinalizeSelect();
		break;

	case SCENE_GAME:
		FinalizeGame();
		break;

	case SCENE_RESULT:
		FinalizeResult();
		break;

	default:
		break;
	}

	//実行中のシーンを切り替え
	scene = next;

	//次のシーンを初期化する
	switch (scene)
	{
	case SCENE_TITLE:
		InitializeTitle();
		break;

	case SCENE_SELECT:
		InitializeSelect();
		break;

	case SCENE_GAME:
		InitializeGame();
		break;

	case SCENE_RESULT:
		InitializeResult();
		break;

	default:
		break;
	}
}

// =========================================================
// Game Stage変更
// =========================================================
void SetGameStage(GAME_STAGE targetStage)
{
	stage = targetStage;
}

// =========================================================
// Game Stage取得
// =========================================================
GAME_STAGE GetCurrentGameStage()
{
	return stage;
}

// =========================================================
// 共用関数
// =========================================================
// Lerp関数
float LerpFloat(float start, float end, float rate)
{
	return start + (end - start) * rate;
}

Float2 LerpFloat2(Float2 start, Float2 end, float rate)
{
	Float2 result{};

	result.x = LerpFloat(
		start.x,
		end.x,
		rate
	);

	result.y = LerpFloat(
		start.y,
		end.y,
		rate
	);

	return result;
}

// MoveTowards
float MoveTowardsFloat(
	float current,
	float target,
	float speed
)
{
	float distance = target - current;

	if (fabsf(distance) <= speed)
	{
		return target;
	}

	return current +
		(distance > 0.0f ? speed : -speed);
}