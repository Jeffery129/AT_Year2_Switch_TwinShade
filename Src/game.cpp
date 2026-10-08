// =========================================================
// game.cpp ゲームシーン制御
// 
// 制作者:		日付：
// =========================================================
#include "main.h"
#include "game.h"
#include "background.h"
#include "controller.h"
#include "fade.h"
#include "player.h"
#include "block.h"
#include "spike.h"
#include "color_change_block.h"
#include "score.h"
#include "sound.h"
#include "explosion.h"
#include "bullet.h"
#include "item.h"
#include "camera.h"
#include "enemy.h"
#include "boss.h"
#include "enemy_bullet.h"
#include "portal.h"
#include "texture.h"
#include "sprite.h"

// =========================================================
// マクロ定義
// =========================================================
#define STAGE_CHANGE_PATTERN_MAX        (19)
#define STAGE_CHANGE_PATTERN_NUM_U      (19)
#define STAGE_CHANGE_PATTERN_NUM_V      (1)
#define STAGE_CHANGE_ANIME_SPEED        (2)
#define STAGE_CHANGE_EXECUTE_PATTERN    (4) // 5枚目の画像でステージを切り替える

// Menu Scroll Background
#define BG_NOISE_PIC_NUM				(4)
#define BG_NOISE_SIZE					(240.0f)
#define BG_NOISE_SCROLL_SPEED			(0.001f)

// Pause Menu
#define PAUSE_BUTTON_PIC_NUM			(2)

#define PAUSE_TEXT_POS_X				(0.0f)
#define PAUSE_TEXT_POS_Y				(0.0f)
#define PAUSE_TEXT_SIZE_X				(1920.0f)
#define PAUSE_TEXT_SIZE_Y				(1080.0f)

#define PAUSE_BUTTON_SIZE_X				(396.0f)
#define PAUSE_BUTTON_SIZE_Y				(96.0f)
#define PAUSE_BUTTON_HOVER_SCALE        (1.2f)

#define PAUSE_OPTION_POS_X				(0.0f - 75.0f / 2)
#define PAUSE_OPTION_POS_Y				(SCREEN_HEIGHT / 4 - PAUSE_BUTTON_SIZE_Y / 2 - 150.0f)
#define PAUSE_TITLE_POS_X				(0.0f - 75.0f / 2)
#define PAUSE_TITLE_POS_Y				(SCREEN_HEIGHT / 4 - PAUSE_BUTTON_SIZE_Y / 2 + 30.0f)

// Setting Menu
#define SETTING_MENU_POS_X				(0.0f)
#define SETTING_MENU_POS_Y				(0.0f)
#define SETTING_SIZE_SCALE			    (1.5f)
#define SETTING_MENU_SIZE_X				(640.0f * SETTING_SIZE_SCALE)
#define SETTING_MENU_SIZE_Y				(540.0f * SETTING_SIZE_SCALE)

#define SETTING_INTRO_POS_X				(0.0f)
#define SETTING_INTRO_POS_Y 			(0.0f)
#define SETTING_INTRO_SIZE_X			(1920.0f)
#define SETTING_INTRO_SIZE_Y			(1080.0f)

// Sound
#define SETTING_VOLUME_MIN				(0.0f)
#define SETTING_VOLUME_MAX				(1.0f)
#define SETTING_VOLUME_STEP				(0.1f)

// Sensitivity
#define SETTING_SENS_MIN				(0.05f)
#define SETTING_SENS_MAX				(1.0f)
#define SETTING_SENS_STEP				(0.05f)

// Bar
#define SETTING_BAR_POS_X				(140.0f)
#define SETTING_SOUND_BAR_POS_Y			(30.0f)
#define SETTING_SENS_BAR_POS_Y			(165.0f)
#define SETTING_BAR_SIZE_X				(410.0f)
#define SETTING_BAR_SIZE_Y				(45.0f)

// Game Loading
#define GAME_LOADING_TEXT_PATTERN_MAX       (12)
#define GAME_LOADING_TEXT_PATTERN_NUM_U     (6)
#define GAME_LOADING_TEXT_PATTERN_NUM_V     (2)
#define GAME_LOADING_TEXT_ANIME_SPEED       (1)
#define GAME_LOADING_TEXT_SIZE_SCALE		(4.0f)
#define GAME_LOADING_TEXT_SIZE_X            (151.0f * GAME_LOADING_TEXT_SIZE_SCALE)
#define GAME_LOADING_TEXT_SIZE_Y            (40.0f * GAME_LOADING_TEXT_SIZE_SCALE)
#define GAME_LOADING_TEXT_POS_X             (140.0f)
#define GAME_LOADING_TEXT_POS_Y             (20.0f)

#define GAME_LOADING_PLAYER_PATTERN_MAX     (10)
#define GAME_LOADING_PLAYER_PATTERN_NUM_U   (10)
#define GAME_LOADING_PLAYER_PATTERN_NUM_V   (1)
#define GAME_LOADING_PLAYER_ANIME_SPEED     (1)
#define GAME_LOADING_PLAYER_SIZE_SCALE		(1.4f)
#define GAME_LOADING_PLAYER_SIZE_X          (200.0f * GAME_LOADING_PLAYER_SIZE_SCALE)
#define GAME_LOADING_PLAYER_SIZE_Y          (200.0f * GAME_LOADING_PLAYER_SIZE_SCALE)
#define GAME_LOADING_PLAYER_POS_X           (-300.0f)
#define GAME_LOADING_PLAYER_POS_Y           (0.0f)

#define GAME_LOADING_WAIT_FRAME             (10)

// =========================================================
// 列挙体 (UI)
// =========================================================
const char g_bgNoiseFileName[BG_NOISE_PIC_NUM][256]
{
	"rom:/BG_Noise_Low_Resolusion_1.tga",
	"rom:/BG_Noise_Low_Resolusion_2.tga",
	"rom:/BG_Noise_Low_Resolusion_3.tga",
	"rom:/BG_Noise_Low_Resolusion_4.tga"
};

enum BG_NOISE_DIR
{
	BG_NOISE_LEFT_UP = 0,
	BG_NOISE_LEFT_DOWN,
	BG_NOISE_RIGHT_UP,
	BG_NOISE_RIGHT_DOWN,
	BG_NOISE_DIR_MAX
};

enum BUTTON_STATE
{
	BUTTON_STATE_NONE = 0,
	BUTTON_STATE_HOVER,
	BUTTON_STATE_MAX
};

struct UI_PIC_DATA
{
	const char FILE_NAME[256]{};
	Float2 pos{};
	Float2 size{};
};

enum PAUSE_BUTTON
{
	PAUSE_BUTTON_OPTION = 0,
	PAUSE_BUTTON_TITLE,
	PAUSE_BUTTON_MAX
};

UI_PIC_DATA g_pauseTitle
{
	"rom:/Pause_Title_Full.tga", 
	{ PAUSE_TEXT_POS_X, PAUSE_TEXT_POS_Y },
	{ PAUSE_TEXT_SIZE_X, PAUSE_TEXT_SIZE_Y },
};
UI_PIC_DATA g_pauseButton[PAUSE_BUTTON_MAX][BUTTON_STATE_MAX]
{
	// OPTION
	{
		{
			"rom:/Pause_Option_Unselected.tga",
			{ PAUSE_OPTION_POS_X, PAUSE_OPTION_POS_Y },
			{ PAUSE_BUTTON_SIZE_X, PAUSE_BUTTON_SIZE_Y }
		},
		{
			"rom:/Pause_Option_Selected.tga",
			{ PAUSE_OPTION_POS_X, PAUSE_OPTION_POS_Y },
			{ PAUSE_BUTTON_SIZE_X * PAUSE_BUTTON_HOVER_SCALE, PAUSE_BUTTON_SIZE_Y * PAUSE_BUTTON_HOVER_SCALE }
		}
	},

	// TITLE
	{
		{
			"rom:/Pause_Title_Unselected.tga",
			{ PAUSE_TITLE_POS_X, PAUSE_TITLE_POS_Y },
			{ PAUSE_BUTTON_SIZE_X, PAUSE_BUTTON_SIZE_Y }
		},
		{
			"rom:/Pause_Title_Selected.tga",
			{ PAUSE_TITLE_POS_X, PAUSE_TITLE_POS_Y },
			{ PAUSE_BUTTON_SIZE_X * PAUSE_BUTTON_HOVER_SCALE, PAUSE_BUTTON_SIZE_Y * PAUSE_BUTTON_HOVER_SCALE }
		}
	}
};
unsigned int g_pauseTitleTextureId{};
unsigned int g_pauseButtonTextureId[PAUSE_BUTTON_MAX][BUTTON_STATE_MAX]{};
unsigned int g_bgNoiseTextureId[BG_NOISE_PIC_NUM]{};

int g_pauseHover{ PAUSE_BUTTON_OPTION };
int g_pauseNoiseId{};
int g_pauseNoiseDir{};
float g_pauseNoiseU{};
float g_pauseNoiseV{};
Float4 g_pauseNoiseColor{};

enum SETTING_PIC
{
	SETTING_PIC_INTRO = 0,
	SETTING_PIC_FRAME,
	SETTING_PIC_SOUND,
	SETTING_PIC_SENSITIVITY,
	SETTING_PIC_MAX
};
enum SETTING_BUTTON
{
	SETTING_BUTTON_SOUND = 0,
	SETTING_BUTTON_SENSITIVITY,
	SETTING_BUTTON_MAX
};
const char g_settingFileName[SETTING_PIC_MAX][256]
{
	"rom:/Setting_Control_Intro.tga",				// 一番下で描画 pos(0, 0), size(1920, 1080)
	"rom:/Setting_Menu_Frame.tga",					// 二番目で描画
	"rom:/Setting_Hover_Sound.tga",					// hover時に描画
	"rom:/Setting_Hover_Sensitivity.tga"			// hover時に描画
};
unsigned int g_settingTextureId[SETTING_PIC_MAX]{};

bool g_settingOpen{ false };
int g_settingHover{ SETTING_BUTTON_SOUND };

float g_masterVolume{ 1.0f };
float g_sensitivity{ 0.3f };


// Game初期化Step
enum GAME_INITIALIZE_STEP
{
	GAME_INITIALIZE_BG = 0,
	GAME_INITIALIZE_EXPLOSION,
	GAME_INITIALIZE_BLOCK,
	GAME_INITIALIZE_SPIKE,
	GAME_INITIALIZE_COLOR_CHANGE_BLOCK,
	GAME_INITIALIZE_PORTAL,
	GAME_INITIALIZE_PLAYER,
	GAME_INITIALIZE_ENEMY,
	GAME_INITIALIZE_BOSS,
	GAME_INITIALIZE_SCORE,
	GAME_INITIALIZE_ITEM,
	GAME_INITIALIZE_BULLET,
	GAME_INITIALIZE_ENEMY_BULLET,
	GAME_INITIALIZE_STAGE_CHANGE,
	GAME_INITIALIZE_PAUSE_MENU,
	GAME_INITIALIZE_CAMERA,
	GAME_INITIALIZE_FINISH
};

// Game Loading
bool isGameInitializing{ false };
GAME_INITIALIZE_STEP gameInitializeStep{ GAME_INITIALIZE_BG };
unsigned int gameLoadingTextTextureId{};
unsigned int gameLoadingPlayerTextureId{};
int gameLoadingAnimeFrame{};
int gameLoadingWaitFrame{};

// =========================================================
// グローバル変数
// =========================================================
bool isTimeScaleZero{ false };
bool isGamePause{ false };

// 死亡後連打ATM(Boss)
float maxNum{ 100.0f };
float currentNum{ 0.0f };
float oneClick{ 10.0f };
float decreaseNum{ 0.2f };

bool isDeathATM{ false };
bool isDeathATMSuccess{ false };
bool isDeathATMSuccessFadeStarted{ false };

// 通常死亡復活済み
bool isNormalDeathRevived{ false };

// Stage切り替え
unsigned int stageChangeTextureId{ 0 };
int stageChangeAnimeFrame{ 0 };

bool isStageChanging{ false };
bool hasStageChanged{ false };

GAME_STAGE stageChangeTarget{ GAME_STAGE_MAX };

// =========================================================
// プロトタイプ宣言
// =========================================================
void UpdateDeathProcess(void);
void UpdateDeathATM(void);
void DrawDeathATM(void);
void CompleteDeathATM(void);

void ResetGameStageState(void);

void UpdateStageChange(void);
void DrawStageChange(void);
void EndStageChange(void);

// Pause Menu
void InitializePauseMenu(void);
void OpenPauseMenu(void);
void UpdatePauseMenu(void);
void DrawPauseMenu(void);
void FinalizePauseMenu(void);

// Setting Menu
void OpenSettingMenu(void);
void CloseSettingMenu(void);
void UpdateSettingMenu(void);
void DrawSettingMenu(void);

void DrawSettingBar(float posY, float value, float minValue, float maxValue);

// Game Loading
void UpdateGameInitialize(void);
void FinishGameInitialize(void);
void DrawGameLoading(void);
void DrawGameLoadingText(void);
void DrawGameLoadingPlayer(void);
void FinalizeGameLoading(void);

// =========================================================
// ゲームシーン初期化
// =========================================================
void InitializeGame(void)
{
	isGameInitializing = true;
	gameInitializeStep = GAME_INITIALIZE_BG;
	gameLoadingAnimeFrame = 0;
	gameLoadingWaitFrame = 0;

	// Loading用画像だけ先に読み込む
	gameLoadingTextTextureId = LoadTexture("rom:/Loading_Text.tga");
	gameLoadingPlayerTextureId = LoadTexture("rom:/Run_White.tga");
}

#pragma region InitializazeProcess

// =========================================================
// Game分割初期化更新
// =========================================================
void UpdateGameInitialize(void)
{
	if (!isGameInitializing) return;

	gameLoadingAnimeFrame++;

	// Loading画面を先に表示してから初期化を開始する
	if (gameLoadingWaitFrame < GAME_LOADING_WAIT_FRAME)
	{
		gameLoadingWaitFrame++;
		return;
	}

	switch (gameInitializeStep)
	{
	case GAME_INITIALIZE_BG:
		InitializeBG();
		gameInitializeStep = GAME_INITIALIZE_EXPLOSION;
		break;

	case GAME_INITIALIZE_EXPLOSION:
		InitializeExplosion();
		gameInitializeStep = GAME_INITIALIZE_BLOCK;
		break;

	case GAME_INITIALIZE_BLOCK:
		InitializeBlock();
		gameInitializeStep = GAME_INITIALIZE_SPIKE;
		break;

	case GAME_INITIALIZE_SPIKE:
		InitializeSpike();
		gameInitializeStep = GAME_INITIALIZE_COLOR_CHANGE_BLOCK;
		break;

	case GAME_INITIALIZE_COLOR_CHANGE_BLOCK:
		InitializeColorChangeBlock();
		gameInitializeStep = GAME_INITIALIZE_PORTAL;
		break;

	case GAME_INITIALIZE_PORTAL:
		InitializePortal();
		gameInitializeStep = GAME_INITIALIZE_PLAYER;
		break;

	case GAME_INITIALIZE_PLAYER:
		InitializePlayer();
		gameInitializeStep = GAME_INITIALIZE_ENEMY;
		break;

	case GAME_INITIALIZE_ENEMY:
		InitializeEnemy();
		gameInitializeStep = GAME_INITIALIZE_BOSS;
		break;

	case GAME_INITIALIZE_BOSS:
		InitializeBoss();
		gameInitializeStep = GAME_INITIALIZE_SCORE;
		break;

	case GAME_INITIALIZE_SCORE:
		InitializeScore();
		gameInitializeStep = GAME_INITIALIZE_ITEM;
		break;

	case GAME_INITIALIZE_ITEM:
		InitializeItem();
		gameInitializeStep = GAME_INITIALIZE_BULLET;
		break;

	case GAME_INITIALIZE_BULLET:
		InitializeBullet();
		gameInitializeStep = GAME_INITIALIZE_ENEMY_BULLET;
		break;

	case GAME_INITIALIZE_ENEMY_BULLET:
		InitializeEnemyBullet();
		gameInitializeStep = GAME_INITIALIZE_STAGE_CHANGE;
		break;

	case GAME_INITIALIZE_STAGE_CHANGE:
		stageChangeTextureId = LoadTexture("rom:/Scene_Stage_Change.tga");
		gameInitializeStep = GAME_INITIALIZE_PAUSE_MENU;
		break;

	case GAME_INITIALIZE_PAUSE_MENU:
		InitializePauseMenu();
		gameInitializeStep = GAME_INITIALIZE_CAMERA;
		break;

	case GAME_INITIALIZE_CAMERA:
		InitializeCamera(GetPlayer()->pos);
		gameInitializeStep = GAME_INITIALIZE_FINISH;
		break;

	case GAME_INITIALIZE_FINISH:
		FinishGameInitialize();
		break;

	default:
		break;
	}
}

// =========================================================
// Game分割初期化完了
// =========================================================
void FinishGameInitialize(void)
{
	stageChangeAnimeFrame = 0;
	isStageChanging = false;
	hasStageChanged = false;
	stageChangeTarget = GAME_STAGE_MAX;

	ResetGameStageState();

	// SelectからBoss Stageへ直接入った場合
	if (GetCurrentGameStage() == GAME_STAGE_BOSS)
	{
		StartBossIntro();
	}

	// Stage BGM
	StopBGM();

	GAME_STAGE currentStage = GetCurrentGameStage();

	switch (currentStage)
	{
	case GAME_STAGE_T_01:
		PlayBGM(BGM_Castle);
		break;

	case GAME_STAGE_T_02:
		PlayBGM(BGM_Corridor);
		break;

	case GAME_STAGE_T_03:
		PlayBGM(BGM_Light_Cave);
		break;

	case GAME_STAGE_S_01:
		PlayBGM(BGM_Castle);
		break;

	case GAME_STAGE_S_02:
		PlayBGM(BGM_Corridor);
		break;

	case GAME_STAGE_BOSS:
		PlayBGM(BGM_Boss);
		break;

	case GAME_STAGE_MAX:
	default:
		break;
	}

	isGameInitializing = false;
	FinalizeGameLoading();
}

// =========================================================
// Loading Text描画
// =========================================================
void DrawGameLoadingText(void)
{
	if (gameLoadingTextTextureId == 0) return;

	int frame = (gameLoadingAnimeFrame / GAME_LOADING_TEXT_ANIME_SPEED) % GAME_LOADING_TEXT_PATTERN_MAX;
	float tx = static_cast<float>(frame % GAME_LOADING_TEXT_PATTERN_NUM_U) / static_cast<float>(GAME_LOADING_TEXT_PATTERN_NUM_U);
	float ty = static_cast<float>(frame / GAME_LOADING_TEXT_PATTERN_NUM_U) / static_cast<float>(GAME_LOADING_TEXT_PATTERN_NUM_V);
	float tw = 1.0f / static_cast<float>(GAME_LOADING_TEXT_PATTERN_NUM_U);
	float th = 1.0f / static_cast<float>(GAME_LOADING_TEXT_PATTERN_NUM_V);

	DrawSpriteAnimation(
		GAME_LOADING_TEXT_POS_X, GAME_LOADING_TEXT_POS_Y,
		GAME_LOADING_TEXT_SIZE_X, GAME_LOADING_TEXT_SIZE_Y,
		MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f,
		tx, ty, tw, th,
		gameLoadingTextTextureId
	);
}

// =========================================================
// Loading Player描画
// =========================================================
void DrawGameLoadingPlayer(void)
{
	if (gameLoadingPlayerTextureId == 0) return;

	int frame = (gameLoadingAnimeFrame / GAME_LOADING_PLAYER_ANIME_SPEED) % GAME_LOADING_PLAYER_PATTERN_MAX;
	float tx = static_cast<float>(frame % GAME_LOADING_PLAYER_PATTERN_NUM_U) / static_cast<float>(GAME_LOADING_PLAYER_PATTERN_NUM_U);
	float ty = static_cast<float>(frame / GAME_LOADING_PLAYER_PATTERN_NUM_U) / static_cast<float>(GAME_LOADING_PLAYER_PATTERN_NUM_V);
	float tw = 1.0f / static_cast<float>(GAME_LOADING_PLAYER_PATTERN_NUM_U);
	float th = 1.0f / static_cast<float>(GAME_LOADING_PLAYER_PATTERN_NUM_V);

	DrawSpriteAnimation(
		GAME_LOADING_PLAYER_POS_X, GAME_LOADING_PLAYER_POS_Y,
		GAME_LOADING_PLAYER_SIZE_X, GAME_LOADING_PLAYER_SIZE_Y,
		MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f,
		tx, ty, tw, th,
		gameLoadingPlayerTextureId
	);
}

// =========================================================
// Game Loading描画
// =========================================================
void DrawGameLoading(void)
{
	// 黒背景
	DrawSpriteQuad(
		0.0f, 0.0f,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		MakeFloat4(0.0f, 0.0f, 0.0f, 1.0f),
		0
	);

	// Loading Text左側のPlayer
	DrawGameLoadingPlayer();

	// Loading Text
	DrawGameLoadingText();
}

// =========================================================
// ゲームシーン更新
// =========================================================
void UpdateGame(void)
{
	if (isGameInitializing)
	{
		UpdateGameInitialize();
		return;
	}

	if (!isTimeScaleZero)
	{
		UpdateBG();
		UpdateBlock();
		UpdateColorChangeBlock();

		// Boss登場中
		if (GetBossIntro())
		{
			UpdateBossIntro();

			PLAYER* player = GetPlayer();

			// Playerを出生位置で待機
			player->state = PLAYER_STATE_IDLE;
			player->vel = MakeFloat2(0.0f, 0.0f);
			player->exVel = MakeFloat2(0.0f, 0.0f);
			player->isAiming = false;

			SetCameraAimOffset(MakeFloat2(0.0f, 0.0f), false);

			// Camera Shakeを更新
			UpdateCamera(player->pos);
		}
		else
		{
			UpdateSpike();

			// Portal更新中にStage切り替えが開始する
			UpdatePortal();

			// Stage切り替え開始後はWorldを更新しない
			if (!isStageChanging)
			{
				UpdatePlayer();
				UpdateBullet();
				UpdateEnemy();
				UpdateBoss();
				UpdateEnemyBullet();
				UpdateExplosion();
				UpdateScore();
				UpdateItem();

				// プレイヤーのAim情報をカメラに設定
				SetCameraAimOffset(
					GetPlayer()->aimDir,
					GetPlayer()->isAiming
				);

				// プレイヤーを追従
				UpdateCamera(GetPlayer()->pos);
			}
		}
	}

	UpdateStageChange(); // Stage切り替え中更新
	UpdatePauseMenu(); // Pause中もMenu更新する

	// 死亡後連打ATM
	if (!isStageChanging && !GetBossIntro() && !isGamePause)
	{
		UpdateDeathProcess();
	}

	// Game Pause
	if (!isStageChanging && !GetBossIntro() && !g_settingOpen &&
		GetControllerTrigger(NpadButton::Plus::Index)&&
		GetPlayer()->state != PLAYER_STATE_DEAD && !isDeathATM)
	{
		// Fade中はGame Pauseを切り替えない
		if (GetFade()->state == FADE_NONE && GetFadeInGame()->state == FADE_NONE)
		{
			isGamePause = !isGamePause;
			isTimeScaleZero = isGamePause;

			if (isGamePause) // Pause
			{
				SetVolumeBGM(0.4f, 10);
				PlaySE(UI_Open_Menu);
			}
			else
			{
				SetVolumeBGM(1.0f, 10);
				PlaySE(UI_Close_Menu);
			}

			if (isGamePause) OpenPauseMenu();
		}
	}
#ifdef DEBUG
	// Test用
	if (!isStageChanging && !GetBossIntro() && GetControllerTrigger(NpadButton::X::Index))
	{
		GAME_STAGE currentStage = GetCurrentGameStage();
		GAME_STAGE nextStage = static_cast<GAME_STAGE>(currentStage + 1);
		if (nextStage >= GAME_STAGE::GAME_STAGE_MAX) return;
		StartStageChange(nextStage);
	}
#endif // DEBUG
}

// =========================================================
// Game Loading終了処理
// =========================================================
void FinalizeGameLoading(void)
{
	if (gameLoadingTextTextureId != 0)
	{
		UnloadTexture(gameLoadingTextTextureId);
		gameLoadingTextTextureId = 0;
	}

	if (gameLoadingPlayerTextureId != 0)
	{
		UnloadTexture(gameLoadingPlayerTextureId);
		gameLoadingPlayerTextureId = 0;
	}

	gameLoadingAnimeFrame = 0;
	gameLoadingWaitFrame = 0;
}

#pragma endregion

// =========================================================
// ゲームシーン描画
// =========================================================
void DrawGame(void)
{
	if (isGameInitializing)
	{
		DrawGameLoading();
		return;
	}

	DrawBG();
	DrawPortal();
	DrawBoss();
	DrawBlock();
	DrawColorChangeBlock();
	DrawSpike();
	DrawBullet();
	DrawAimLine();				//Player.cppからだした描画、レーヤー調整
	DrawPlayer();
	DrawEnemyBullet();
	DrawEnemy();
	DrawExplosion();
	DrawItem();
	//DrawScore();
	DrawChargeEffect();			//Player.cppからだした描画、レーヤー調整
	DrawSprintCoolDownBar();	//Player.cppからだした描画、レーヤー調整

	//Player UI描画
	DrawStatusUI();
	// Boss HP Bar
	DrawBossHpBar();
	// Boss Warning
	DrawBossWarning();

	if (isDeathATM && (GetFadeInGame()->state != FADE_OUT || isDeathATMSuccessFadeStarted))
	{
		DrawDeathATM();
	}

	if (isGamePause)
	{
		DrawPauseMenu();
	}

	// 最前面にStage切り替えを描画
	DrawStageChange();
}

// =========================================================
// ゲームシーン終了処理
// =========================================================
void FinalizeGame(void)
{
	FinalizeGameLoading();

	FinalizeCamera();
	FinalizeBG();
	FinalizeBullet();
	FinalizeEnemyBullet();
	FinalizeExplosion();
	FinalizeBlock();
	FinalizeSpike();
	FinalizeColorChangeBlock();
	FinalizePortal();
	FinalizePlayer();
	FinalizeEnemy();
	FinalizeBoss();
	FinalizeScore();
	FinalizeItem();

	FinalizePauseMenu();

	if (stageChangeTextureId != 0)
	{
		UnloadTexture(stageChangeTextureId);
		stageChangeTextureId = 0;
	}

	stageChangeAnimeFrame = 0;
	isStageChanging = false;
	hasStageChanged = false;
	stageChangeTarget = GAME_STAGE_MAX;

	isNormalDeathRevived = false;
}

// =========================================================
// ゲーム内状態リセット
// =========================================================
void ResetGameStageState(void)
{
	currentNum = 0.0f;

	isDeathATM = false;
	isDeathATMSuccess = false;
	isDeathATMSuccessFadeStarted = false;

	isNormalDeathRevived = false;

	isGamePause = false;
}

// =========================================================
// Stage切り替え開始
// =========================================================
void StartStageChange(GAME_STAGE nextStage)
{
	if (isStageChanging) return;
	if (nextStage < GAME_STAGE_T_01 || nextStage >= GAME_STAGE_MAX) return;
	if (nextStage == GetCurrentGameStage()) return;

	stageChangeTarget = nextStage;
	stageChangeAnimeFrame = 0;

	isStageChanging = true;
	hasStageChanged = false;

	isGamePause = false;

	// World更新停止
	SetGameTimeScaleZero(true);
}

// =========================================================
// Stage切り替え更新
// =========================================================
void UpdateStageChange(void)
{
	if (!isStageChanging) return;

	const int animationEndFrame = STAGE_CHANGE_PATTERN_MAX * STAGE_CHANGE_ANIME_SPEED;

	// 最終Pattern描画後に終了
	if (stageChangeAnimeFrame >= animationEndFrame)
	{
		EndStageChange();
		return;
	}

	int pattern = stageChangeAnimeFrame / STAGE_CHANGE_ANIME_SPEED;

	// 5枚目でStageを変更
	if (!hasStageChanged && pattern == STAGE_CHANGE_EXECUTE_PATTERN)
	{
		ChangeGameStage(stageChangeTarget);
		hasStageChanged = true;
	}

	stageChangeAnimeFrame++;
}

// =========================================================
// Stage切り替え描画
// =========================================================
void DrawStageChange(void)
{
	if (!isStageChanging) return;

	// Update後の値から描画用Frameを取得
	int drawAnimeFrame = stageChangeAnimeFrame - 1;
	if (drawAnimeFrame < 0) drawAnimeFrame = 0;

	int pattern = drawAnimeFrame / STAGE_CHANGE_ANIME_SPEED;
	if (pattern >= STAGE_CHANGE_PATTERN_MAX) pattern = STAGE_CHANGE_PATTERN_MAX - 1;

	const float patternWidth = 1.0f / static_cast<float>(STAGE_CHANGE_PATTERN_NUM_U);
	const float patternHeight = 1.0f / static_cast<float>(STAGE_CHANGE_PATTERN_NUM_V);

	const int patternX = pattern % STAGE_CHANGE_PATTERN_NUM_U;
	const int patternY = pattern / STAGE_CHANGE_PATTERN_NUM_U;

	const float tx = patternWidth * static_cast<float>(patternX);
	const float ty = patternHeight * static_cast<float>(patternY);

	DrawSpriteAnimation(
		0.0f, 0.0f,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f,
		tx, ty, patternWidth, patternHeight,
		stageChangeTextureId
	);
}

// =========================================================
// Stage切り替え終了
// =========================================================
void EndStageChange(void)
{
	isStageChanging = false;
	hasStageChanged = false;

	stageChangeAnimeFrame = 0;
	stageChangeTarget = GAME_STAGE_MAX;

	// World更新再開
	SetGameTimeScaleZero(false);

	// Boss登場開始
	if (GetCurrentGameStage() == GAME_STAGE_BOSS)
	{
		StartBossIntro();
	}
}

// =========================================================
// Stage切り替え状態取得
// =========================================================
bool GetStageChanging(void)
{
	return isStageChanging;
}

// =========================================================
// ゲームStage切り替え
// =========================================================
void ChangeGameStage(GAME_STAGE nextStage)
{
	if (nextStage < GAME_STAGE_T_01 || nextStage >= GAME_STAGE_MAX) return;
	if (nextStage == GetCurrentGameStage()) return;

	// 前Stageの一時Objectをクリア
	ResetBullet();
	ResetEnemyBullet();
	ResetExplosion();

	// Stage番号を先に変更
	SetGameStage(nextStage);

	// 新しいStageデータを設定
	ReloadBlockStage();
	ReloadSpikeStage();
	ReloadColorChangeBlockStage();
	ReloadPortalStage();
	ReloadPlayerStage();
	ReloadEnemyStage();
	ReloadBoss();

	// BGM切り替え
	StopBGM();
	GAME_STAGE currentStage = GetCurrentGameStage();
	switch (currentStage)
	{
	case GAME_STAGE_T_01:
		PlayBGM(BGM_Castle);
		break;
	case GAME_STAGE_T_02:
		PlayBGM(BGM_Corridor);
		break;
	case GAME_STAGE_T_03:
		PlayBGM(BGM_Light_Cave);
		break;
	case GAME_STAGE_S_01:
		PlayBGM(BGM_Castle);
		break;
	case GAME_STAGE_S_02:
		PlayBGM(BGM_Corridor);
		break;
	case GAME_STAGE_BOSS:
		PlayBGM(BGM_Boss);
		break;
	case GAME_STAGE_MAX:
	default:
		break;
	}

	// MapとPlayer設定後にCameraをリセット
	ResetCamera(GetPlayer()->pos);
	ResetGameStageState();

	// Animation終了までWorld更新停止
	SetGameTimeScaleZero(true);
}

// =========================================================
// ゲーム内時間停止設定
// =========================================================
void SetGameTimeScaleZero(bool isZero)
{
	isTimeScaleZero = isZero;

	// 死亡演出などによる時間停止ではPause表示を出さない
	if (isZero)
	{
		isGamePause = false;
	}
}

// =========================================================
// ゲーム内時間停止状態取得
// =========================================================
bool GetGameTimeScaleZero(void)
{
	return isTimeScaleZero;
}

// =========================================================
// 死亡後連打ATM開始
// =========================================================
void StartDeathATM(void)
{
	// Boss Stage以外は開始しない
	if (GetCurrentGameStage() != GAME_STAGE_MAX) return;
	if (isDeathATM) return;

	currentNum = 0.0f;
	isDeathATM = true;
	isDeathATMSuccess = false;
	isDeathATMSuccessFadeStarted = false;

	StartFadeInGame();
}

// =========================================================
// 死亡後連打ATM更新
// =========================================================
void UpdateDeathATM(void)
{
	if (!isDeathATM) return;

	// 最初のFade Out中は入力を受け付けない
	if (!isDeathATMSuccess && GetFadeInGame()->state != FADE_OUT)
	{
		currentNum -= decreaseNum;

		if (GetControllerTrigger(NpadButton::A::Index)) currentNum += oneClick;
		if (GetControllerTrigger(NpadButton::B::Index)) currentNum += oneClick;
		if (GetControllerTrigger(NpadButton::X::Index)) currentNum += oneClick;
		if (GetControllerTrigger(NpadButton::Y::Index)) currentNum += oneClick;

		if (currentNum < 0.0f) currentNum = 0.0f;
		if (currentNum > maxNum) currentNum = maxNum;

		if (currentNum >= maxNum)
		{
			currentNum = maxNum;
			isDeathATMSuccess = true;
		}
	}

	if (isDeathATMSuccess && !isDeathATMSuccessFadeStarted && GetFadeInGame()->state == FADE_NONE)
	{
		StartFadeInGame();
		isDeathATMSuccessFadeStarted = true;
	}

	if (isDeathATMSuccessFadeStarted && GetFadeInGame()->state == FADE_IN)
	{
		CompleteDeathATM();
	}
}

// =========================================================
// 死亡後連打ATM描画
// =========================================================
void DrawDeathATM(void)
{
	// 背景
	DrawSpriteQuad(
		0.0f, 0.0f,
		SCREEN_WIDTH, SCREEN_WIDTH,
		MakeFloat4(0.0f, 0.0f, 0.0f, 1.0f),
		0
	);

	// 進捗バー
	const float startX = -400.0f;	// 進捗バーの左端
	const float posY = -100.0f;
	const float maxWidth = 800.0f;
	const float height = 30.0f;

	float rate = currentNum / maxNum;
	if (rate < 0.0f) rate = 0.0f;
	if (rate > 1.0f) rate = 1.0f;

	float width = maxWidth * rate;
	float centerX = startX + width / 2.0f;

	if (width > 0.0f)
	{
		DrawSpriteQuad(
			centerX, posY,
			width, height,
			MakeFloat4(0.95f, 0.40f, 0.40f, 1.0f),
			0
		);
	}
}

// =========================================================
// 死亡後連打ATM完了
// =========================================================
void CompleteDeathATM(void)
{
	RevivePlayer();

	// Respawn位置にCameraを移動
	ResetCamera(GetPlayer()->pos);

	isDeathATM = false;
	isDeathATMSuccess = false;
	isDeathATMSuccessFadeStarted = false;

	currentNum = 0.0f;

	SetGameTimeScaleZero(false);
}

// =========================================================
// 死亡後連打ATM状態取得
// =========================================================
bool GetDeathATM(void)
{
	return isDeathATM;
}

// =========================================================
// 死亡処理更新
// =========================================================
void UpdateDeathProcess(void)
{
	// Boss StageはATM処理
	if (GetCurrentGameStage() == GAME_STAGE_MAX)
	{
		UpdateDeathATM();
		return;
	}

	// 通常StageはFade中に復活
	if (GetPlayer()->state == PLAYER_STATE_DEAD)
	{
		// Fade Out完了時に復活
		if (!isNormalDeathRevived && GetFadeInGame()->state == FADE_IN)
		{
			RevivePlayer();

			// Respawn位置にCameraを移動
			ResetCamera(GetPlayer()->pos);
			isNormalDeathRevived = true;

			// World更新再開
			SetGameTimeScaleZero(false);
		}

		return;
	}

	// Fade In完了後に状態を戻す
	if (isNormalDeathRevived && GetFadeInGame()->state == FADE_NONE)
	{
		isNormalDeathRevived = false;
	}
}

#pragma region Pause Menu
// =========================================================
// Pause Menu初期化
// =========================================================
void InitializePauseMenu(void)
{
	g_pauseTitleTextureId = LoadTexture(g_pauseTitle.FILE_NAME);

	for (int i = 0; i < PAUSE_BUTTON_MAX; i++)
	{
		for (int j = 0; j < BUTTON_STATE_MAX; j++)
		{
			g_pauseButtonTextureId[i][j] = LoadTexture(g_pauseButton[i][j].FILE_NAME);
		}
	}

	for (int i = 0; i < BG_NOISE_PIC_NUM; i++)
	{
		g_bgNoiseTextureId[i] = LoadTexture(g_bgNoiseFileName[i]);
	}

	for (int i = 0; i < SETTING_PIC_MAX; i++)
	{
		g_settingTextureId[i] =
			LoadTexture(g_settingFileName[i]);
	}

	g_pauseHover = PAUSE_BUTTON_OPTION;
	g_pauseNoiseId = 0;
	g_pauseNoiseDir = BG_NOISE_LEFT_UP;
	g_pauseNoiseU = 0.0f;
	g_pauseNoiseV = 0.0f;
	g_pauseNoiseColor = MakeFloat4(1.0f, 0.75f, 0.80f, 0.75f);

	g_settingOpen = false;
	g_settingHover = SETTING_BUTTON_SOUND;

	g_masterVolume = GetMasterVolume();
	g_sensitivity =GetRightStickSensitivity();
}

// =========================================================
// Pause Menu開始
// =========================================================
void OpenPauseMenu(void)
{
	g_pauseHover = PAUSE_BUTTON_OPTION;
	g_pauseNoiseId = rand() % BG_NOISE_PIC_NUM;
	g_pauseNoiseDir = rand() % BG_NOISE_DIR_MAX;

	g_pauseNoiseU = static_cast<float>(rand() % 100) / 100.0f;
	g_pauseNoiseV = static_cast<float>(rand() % 100) / 100.0f;

	// 可愛い淡い赤・淡い青
	const Float4 paleRed =  MakeFloat4(1.00f, 0.68f, 0.76f, 0.3f);
	const Float4 paleBlue = MakeFloat4(0.62f, 0.82f, 1.00f, 0.3f);

	g_pauseNoiseColor = rand() % 2 == 0 ? paleRed : paleBlue;

	g_settingOpen = false;
	g_settingHover = SETTING_BUTTON_SOUND;
}

// =========================================================
// Pause Menu更新
// =========================================================
void UpdatePauseMenu(void)
{
	if (!isGamePause) return;

	// Noise Scroll
	switch (g_pauseNoiseDir)
	{
	case BG_NOISE_LEFT_UP:
		g_pauseNoiseU -= BG_NOISE_SCROLL_SPEED;
		g_pauseNoiseV -= BG_NOISE_SCROLL_SPEED;
		break;

	case BG_NOISE_LEFT_DOWN:
		g_pauseNoiseU -= BG_NOISE_SCROLL_SPEED;
		g_pauseNoiseV += BG_NOISE_SCROLL_SPEED;
		break;

	case BG_NOISE_RIGHT_UP:
		g_pauseNoiseU += BG_NOISE_SCROLL_SPEED;
		g_pauseNoiseV -= BG_NOISE_SCROLL_SPEED;
		break;

	case BG_NOISE_RIGHT_DOWN:
		g_pauseNoiseU += BG_NOISE_SCROLL_SPEED;
		g_pauseNoiseV += BG_NOISE_SCROLL_SPEED;
		break;
	}

	if (g_pauseNoiseU >= 1.0f) g_pauseNoiseU -= 1.0f;
	if (g_pauseNoiseU < 0.0f) g_pauseNoiseU += 1.0f;
	if (g_pauseNoiseV >= 1.0f) g_pauseNoiseV -= 1.0f;
	if (g_pauseNoiseV < 0.0f) g_pauseNoiseV += 1.0f;

	// Setting Menu中
	if (g_settingOpen)
	{
		UpdateSettingMenu();
		return;
	}

	// Button選択
	if (GetControllerTrigger(NpadButton::Up::Index))
	{
		g_pauseHover--;
		if (g_pauseHover < 0) g_pauseHover = PAUSE_BUTTON_MAX - 1;
		PlaySE(UI_Tab_Switch);
	}
	else if (GetControllerTrigger(NpadButton::Down::Index))
	{
		g_pauseHover++;
		if (g_pauseHover >= PAUSE_BUTTON_MAX) g_pauseHover = 0;
		PlaySE(UI_Tab_Switch);
	}

	// A決定
	if (GetControllerTrigger(NpadButton::A::Index))
	{
		switch (g_pauseHover)
		{
		case PAUSE_BUTTON_OPTION:
			PlaySE(UI_Open_Menu);
			OpenSettingMenu();
			break;

		case PAUSE_BUTTON_TITLE:
			PlaySE(UI_Close_Menu);
			isGamePause = false;
			isTimeScaleZero = false;
			StartFade(SCENE_SELECT);
			break;
		}
	}
}

// =========================================================
// Pause Menu描画
// =========================================================
void DrawPauseMenu(void)
{
	// 一番下の黒背景
	DrawSpriteQuad(
		0.0f, 0.0f,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		MakeFloat4(0.0f, 0.0f, 0.0f, 0.5f),
		0
	);

	// 240x240 Noiseを画面全体へ繰り返す
	float repeatU = SCREEN_WIDTH / BG_NOISE_SIZE;
	float repeatV = SCREEN_HEIGHT / BG_NOISE_SIZE;

	DrawSpriteAnimation(
		0.0f, 0.0f,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		g_pauseNoiseColor, 0.0f,
		g_pauseNoiseU, g_pauseNoiseV,
		repeatU, repeatV,
		g_bgNoiseTextureId[g_pauseNoiseId]
	);

	// Setting Menu
	if (g_settingOpen)
	{
		DrawSettingMenu();
		return;
	}

	// Pause Title
	DrawSpriteQuad(
		g_pauseTitle.pos.x, g_pauseTitle.pos.y,
		g_pauseTitle.size.x, g_pauseTitle.size.y,
		MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f,
		g_pauseTitleTextureId
	);

	// Buttons
	for (int i = 0; i < PAUSE_BUTTON_MAX; i++)
	{
		int state = i == g_pauseHover ? BUTTON_STATE_HOVER : BUTTON_STATE_NONE;

		UI_PIC_DATA* pic = &g_pauseButton[i][state];

		DrawSpriteQuad(
			pic->pos.x, pic->pos.y,
			pic->size.x, pic->size.y,
			MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f,
			g_pauseButtonTextureId[i][state]
		);
	}
}

// =========================================================
// Pause Menu終了処理
// =========================================================
void FinalizePauseMenu(void)
{
	if (g_pauseTitleTextureId != 0)
	{
		UnloadTexture(g_pauseTitleTextureId);
		g_pauseTitleTextureId = 0;
	}

	for (int i = 0; i < PAUSE_BUTTON_MAX; i++)
	{
		for (int j = 0; j < BUTTON_STATE_MAX; j++)
		{
			if (g_pauseButtonTextureId[i][j] == 0) continue;
			UnloadTexture(g_pauseButtonTextureId[i][j]);
			g_pauseButtonTextureId[i][j] = 0;
		}
	}

	for (int i = 0; i < BG_NOISE_PIC_NUM; i++)
	{
		if (g_bgNoiseTextureId[i] == 0) continue;
		UnloadTexture(g_bgNoiseTextureId[i]);
		g_bgNoiseTextureId[i] = 0;
	}

	g_pauseHover = PAUSE_BUTTON_OPTION;
	g_pauseNoiseId = 0;
	g_pauseNoiseDir = BG_NOISE_LEFT_UP;
	g_pauseNoiseU = 0.0f;
	g_pauseNoiseV = 0.0f;

	for (int i = 0; i < SETTING_PIC_MAX; i++)
	{
		if (g_settingTextureId[i] == 0) continue;
		UnloadTexture(g_settingTextureId[i]);
		g_settingTextureId[i] = 0;
	}

	g_settingOpen = false;
	g_settingHover = SETTING_BUTTON_SOUND;
}

// =========================================================
// Setting Menu開始
// =========================================================
void OpenSettingMenu(void)
{
	g_settingOpen = true;
	g_settingHover = SETTING_BUTTON_SOUND;

	g_masterVolume = GetMasterVolume();
	g_sensitivity = GetRightStickSensitivity();
}

// =========================================================
// Setting Menu終了
// =========================================================
void CloseSettingMenu(void)
{
	g_settingOpen = false;
	g_settingHover = SETTING_BUTTON_SOUND;
}

// =========================================================
// Setting Menu更新
// =========================================================
void UpdateSettingMenu(void)
{
	if (!g_settingOpen) return;

	// Button選択
	if (GetControllerTrigger(NpadButton::Up::Index))
	{
		g_settingHover--;

		if (g_settingHover < 0)
		{
			g_settingHover = SETTING_BUTTON_MAX - 1;
		}

		PlaySE(UI_Tab_Switch);
	}
	else if (GetControllerTrigger(NpadButton::Down::Index))
	{
		g_settingHover++;

		if (g_settingHover >= SETTING_BUTTON_MAX)
		{
			g_settingHover = 0;
		}

		PlaySE(UI_Tab_Switch);
	}

	// 左入力
	if (GetControllerTrigger(NpadButton::Left::Index))
	{
		switch (g_settingHover)
		{
		case SETTING_BUTTON_SOUND:
			g_masterVolume -= SETTING_VOLUME_STEP;
			if (g_masterVolume < SETTING_VOLUME_MIN)
			{
				g_masterVolume = SETTING_VOLUME_MIN;
			}

			SetMasterVolume( g_masterVolume);

			PlaySE(UI_Stage_Option_Change);
			break;

		case SETTING_BUTTON_SENSITIVITY:
			g_sensitivity -= SETTING_SENS_STEP;
			if (g_sensitivity < SETTING_SENS_MIN)
			{
				g_sensitivity = SETTING_SENS_MIN;
			}

			SetRightStickSensitivity(g_sensitivity);
			g_sensitivity = GetRightStickSensitivity();
			ResetRightStickInput();

			PlaySE(UI_Stage_Option_Change);
			break;
		}
	}

	// 右入力
	if (GetControllerTrigger(NpadButton::Right::Index))
	{
		switch (g_settingHover)
		{
		case SETTING_BUTTON_SOUND:
			g_masterVolume += SETTING_VOLUME_STEP;

			if (g_masterVolume > SETTING_VOLUME_MAX)
			{
				g_masterVolume = SETTING_VOLUME_MAX;
			}

			SetMasterVolume(g_masterVolume);

			PlaySE(UI_Stage_Option_Change);
			break;

		case SETTING_BUTTON_SENSITIVITY:
			g_sensitivity += SETTING_SENS_STEP;

			if (g_sensitivity > SETTING_SENS_MAX)
			{
				g_sensitivity = SETTING_SENS_MAX;
			}

			SetRightStickSensitivity(g_sensitivity);
			g_sensitivity = GetRightStickSensitivity();
			ResetRightStickInput();

			PlaySE(UI_Stage_Option_Change);
			break;
		}
	}

	// BでPause Menuへ戻る
	if (GetControllerTrigger(NpadButton::B::Index))
	{
		PlaySE(UI_Close_Menu);
		CloseSettingMenu();
	}
}

// =========================================================
// Setting Bar描画
// =========================================================
void DrawSettingBar(float posY, float value, float minValue, float maxValue)
{
	float range = maxValue - minValue;
	if (range <= 0.0f) return;

	float rate = (value - minValue) / range;
	if (rate < 0.0f) rate = 0.0f;
	if (rate > 1.0f) rate = 1.0f;

	// Bar背景
	DrawSpriteQuad(
		SETTING_BAR_POS_X, posY,
		SETTING_BAR_SIZE_X, SETTING_BAR_SIZE_Y,
		MakeFloat4(0.10f, 0.08f, 0.12f, 0.85f), 
		0
	);

	float width = SETTING_BAR_SIZE_X * rate;
	if (width <= 0.0f) return;

	float startX = SETTING_BAR_POS_X - SETTING_BAR_SIZE_X / 2.0f;
	float centerX = startX + width / 2.0f;

	// Bar本体
	DrawSpriteQuad(
		centerX, posY,
		width, SETTING_BAR_SIZE_Y,
		MakeFloat4(0.95f, 0.78f, 0.45f, 1.0f),
		0
	);
}

// =========================================================
// Setting Menu描画
// =========================================================
void DrawSettingMenu(void)
{
	if (!g_settingOpen) return;

	// 操作説明
	DrawSpriteQuad(
		SETTING_INTRO_POS_X, SETTING_INTRO_POS_Y,
		SETTING_INTRO_SIZE_X, SETTING_INTRO_SIZE_Y,
		MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f,
		g_settingTextureId[SETTING_PIC_INTRO]
	);

	// Setting Frame
	DrawSpriteQuad(
		SETTING_MENU_POS_X, SETTING_MENU_POS_Y,
		SETTING_MENU_SIZE_X, SETTING_MENU_SIZE_Y,
		MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f,
		g_settingTextureId[SETTING_PIC_FRAME]
	);

	// Sound Bar
	DrawSettingBar(
		SETTING_SOUND_BAR_POS_Y, g_masterVolume,
		SETTING_VOLUME_MIN, SETTING_VOLUME_MAX
	);

	// Sensitivity Bar
	DrawSettingBar(
		SETTING_SENS_BAR_POS_Y, g_sensitivity,
		SETTING_SENS_MIN, SETTING_SENS_MAX
	);

	// Hover
	int hoverPic = g_settingHover == SETTING_BUTTON_SOUND ? SETTING_PIC_SOUND : SETTING_PIC_SENSITIVITY;
	DrawSpriteQuad(
		SETTING_MENU_POS_X, SETTING_MENU_POS_Y,
		SETTING_MENU_SIZE_X, SETTING_MENU_SIZE_Y,
		MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f,
		g_settingTextureId[hoverPic]
	);
}


#pragma endregion