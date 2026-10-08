// =========================================================
// portal.cpp ゲームステージ遷移
// 
// 制作者:		日付：
// =========================================================
#include "portal.h"
#include "block.h"
#include "texture.h"
#include "sprite.h"
#include "collision.h"
#include "player.h"
#include "controller.h"
#include "game.h"
#include "select.h"
#include "sound.h"

//--------------------------------------------------------------
// マクロ定義 (*´▽｀*)
//--------------------------------------------------------------
#define PORTAL_SIZE_X        (200.0f * 0.9f)
#define PORTAL_SIZE_Y        (440.0f * 0.9f)
#define PORTAL_COLLISION_X   (150.0f * 0.9f)
#define PORTAL_COLLISION_Y	 (320.0f * 0.9f)
#define PORTAL_MAX_NUM       (5)

#define TUTORIAL_PIC_NUM     (8)
//--------------------------------------------------------------
// 構造体 & 列挙体定義 (*´▽｀*)
//--------------------------------------------------------------
struct Pic_Data_Portal
{
	const char FILE_NAME[256]{};
	int PATTERN_MAX{};
	int PATTERN_NUM_U{};
	int PATTERN_NUM_V{};
	int ANIME_SPEED{};
	float PATTERN_WIDTH{ 1.0f / PATTERN_NUM_U };
	float PATTERN_HIGHT{ 1.0f / PATTERN_NUM_V };
};

struct Portal_Data
{
	GAME_STAGE myStage;
	Float2 pos;
	GAME_STAGE nextStage;
};

struct Tutorial_Pic_Data
{
	GAME_STAGE stage{};
	const char FILE_NAME[256]{};
	Float2 pos{};
	Float2 size{};
};

// =========================================================
// グローバル変数
// =========================================================
Pic_Data_Portal portal_pic[PORTAL_STATE::PORTAL_STATE_MAX] =
{
	{ "rom:/Portal.tga",        6, 6, 1, 5 },
	{ "rom:/Portal_Button.tga", 6, 6, 1, 5 },
};
unsigned int portalTextureID[PORTAL_STATE::PORTAL_STATE_MAX]{};

Float2 CENTER_OFFSET_PORTAL = MakeFloat2(MAP_BLOCK_WIDTH * 8.5f, MAP_BLOCK_HEIGHT * 5.0f);
Portal_Data portal_data[PORTAL_MAX_NUM]
{
	{
		GAME_STAGE_T_01, 
		MakeFloat2(
			MAP_BLOCK_WIDTH * 2.5f - CENTER_OFFSET_PORTAL.x,
			MAP_BLOCK_HEIGHT * 8.5f - PORTAL_SIZE_Y / 2.0f - CENTER_OFFSET_PORTAL.y
		),
		GAME_STAGE_T_02
	},
	{
		GAME_STAGE_T_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 2.5f - CENTER_OFFSET_PORTAL.x,
			MAP_BLOCK_HEIGHT * 7.5f - PORTAL_SIZE_Y / 2.0f - CENTER_OFFSET_PORTAL.y
		),
		GAME_STAGE_T_03
	},
	{
		GAME_STAGE_T_03,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 51.5f - CENTER_OFFSET_PORTAL.x,
			MAP_BLOCK_HEIGHT * 7.5f - PORTAL_SIZE_Y / 2.0f - CENTER_OFFSET_PORTAL.y
		),
		GAME_STAGE_S_01
	},
	{
		GAME_STAGE_S_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 55.5f - CENTER_OFFSET_PORTAL.x,
			MAP_BLOCK_HEIGHT * 28.5f - PORTAL_SIZE_Y / 2.0f - CENTER_OFFSET_PORTAL.y
		),
		GAME_STAGE_S_02
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 10.0f - CENTER_OFFSET_PORTAL.x,
			MAP_BLOCK_HEIGHT * 16.5f - PORTAL_SIZE_Y / 2.0f - CENTER_OFFSET_PORTAL.y
		),
		GAME_STAGE_BOSS
	}
};
PORTAL portal[PORTAL_MAX_NUM]{};
unsigned int portal_anime_frame{ 0 };

// 各Tutorialステージの説明図、レーヤーはポータルと一緒なのでここに書きます
Tutorial_Pic_Data tutorial_pic[TUTORIAL_PIC_NUM]
{
	{
		GAME_STAGE_T_01,
		"rom:/Tutorial_01_Pic_01.tga",
		MakeFloat2(
			MAP_BLOCK_WIDTH * 6.0f - CENTER_OFFSET_PORTAL.x,
			MAP_BLOCK_HEIGHT * 12.5f - CENTER_OFFSET_PORTAL.y
		),
		MakeFloat2(840.0f * 1.1f, 720.0f * 1.1f)
	},
	{
		GAME_STAGE_T_01,
		"rom:/Tutorial_01_Pic_02.tga",
		MakeFloat2(
			MAP_BLOCK_WIDTH * 32.0f - CENTER_OFFSET_PORTAL.x,
			MAP_BLOCK_HEIGHT * 11.5f - CENTER_OFFSET_PORTAL.y
		),
		MakeFloat2(840.0f * 1.4f, 360.0f * 1.4f)
	},
	{
		GAME_STAGE_T_02,
		"rom:/Tutorial_02_Pic_01.tga",
		MakeFloat2(
			MAP_BLOCK_WIDTH * 5.5f - CENTER_OFFSET_PORTAL.x,
			MAP_BLOCK_HEIGHT * 20.5f - CENTER_OFFSET_PORTAL.y
		),
		MakeFloat2(960.0f * 1.2f, 720.0f * 1.2f)
	},
	{
		GAME_STAGE_T_02,
		"rom:/Tutorial_02_Pic_02.tga",
		MakeFloat2(
			MAP_BLOCK_WIDTH * 20.0f - CENTER_OFFSET_PORTAL.x,
			MAP_BLOCK_HEIGHT * 18.0f - CENTER_OFFSET_PORTAL.y
		),
		MakeFloat2(840.0f * 1.1f, 360.0f * 1.1f)
	},
	{
		GAME_STAGE_T_02,
		"rom:/Tutorial_02_Pic_03.tga",
		MakeFloat2(
			MAP_BLOCK_WIDTH * 46.0f - CENTER_OFFSET_PORTAL.x,
			MAP_BLOCK_HEIGHT * 19.0f - CENTER_OFFSET_PORTAL.y
		),
		MakeFloat2(360.0f * 1.0f, 840.0f * 1.0f)
	},
	{
		GAME_STAGE_T_02,
		"rom:/Tutorial_02_Pic_04.tga",
		MakeFloat2(
			MAP_BLOCK_WIDTH * 47.5f - CENTER_OFFSET_PORTAL.x,
			MAP_BLOCK_HEIGHT * 12.5f - CENTER_OFFSET_PORTAL.y
		),
		MakeFloat2(720.0f * 1.3f, 480.0f * 1.3f)
	},
	{
		GAME_STAGE_T_03,
		"rom:/Tutorial_03_Pic_01.tga",
		MakeFloat2(
			MAP_BLOCK_WIDTH * 10.0f - CENTER_OFFSET_PORTAL.x,
			MAP_BLOCK_HEIGHT * 4.0f - CENTER_OFFSET_PORTAL.y - 60.0f
		),
		MakeFloat2(600.0f * 1.3f, 360.0f * 1.3f)
	},
	{
		GAME_STAGE_T_03,
		"rom:/Tutorial_03_Pic_02.tga",
		MakeFloat2(
			MAP_BLOCK_WIDTH * 20.5f - CENTER_OFFSET_PORTAL.x,
			MAP_BLOCK_HEIGHT * 5.0f - CENTER_OFFSET_PORTAL.y
		),
		MakeFloat2(720.0f * 1.3f, 380.0f * 1.3f)
	}
};
unsigned int tutorialTextureID[TUTORIAL_PIC_NUM]{};

// =========================================================
// プロトタイプ宣言
// =========================================================
GAME_STAGE GetCurrentPortalTargetStage();

void InitializeTutorialPic();
void DrawTutorialPic();
void FinalizeTutorialPic();

// =========================================================
// 関数定義
// =========================================================
void InitializePortal()
{
	for (int i = 0; i < PORTAL_STATE::PORTAL_STATE_MAX; i++)
	{
		portalTextureID[i] = LoadTexture(portal_pic[i].FILE_NAME);
	}

	for (int i = 0; i < PORTAL_MAX_NUM; i++)
	{
		portal[i].myStage = portal_data[i].myStage;
		portal[i].targetStage = portal_data[i].nextStage;
		portal[i].pos = portal_data[i].pos;
		portal[i].state = PORTAL_STATE::INACTIVE;
		portal[i].use = false;
	}

	ReloadPortalStage();

	//チュートリアル
	InitializeTutorialPic();
}

void UpdatePortal()
{
	// Stage切り替え中は更新しない
	if (GetStageChanging()) return;

	portal_anime_frame++;

	bool isPlayerInside = PortalAndPlayerCollision();
	for (int i = 0; i < PORTAL_MAX_NUM; i++)
	{
		if (!portal[i].use) continue;
		portal[i].state = isPlayerInside ? PORTAL_STATE::ACTIVE : PORTAL_STATE::INACTIVE;
	}

	if (isPlayerInside && GetControllerTrigger(NpadButton::A::Index))
	{
		GAME_STAGE nextStage = GetCurrentPortalTargetStage();
		if (nextStage != GAME_STAGE_MAX)
		{
			PlaySE(SE_Portal_Next_Stage);
			SetLatestUnlockedStage(nextStage);
			StartStageChange(nextStage);
		}
	}
}

void DrawPortal()
{
	//チュートリアル
	DrawTutorialPic();

	// 両stateのspeedなどは確定で完全一致なので、統一してinactiveのデータで計算
	int animeSpeed = portal_pic[PORTAL_STATE::INACTIVE].ANIME_SPEED;
	int patternMax = portal_pic[PORTAL_STATE::INACTIVE].PATTERN_MAX;
	int patternNumU = portal_pic[PORTAL_STATE::INACTIVE].PATTERN_NUM_U;
	int frame = (portal_anime_frame / animeSpeed) % patternMax;
	float tx = portal_pic[PORTAL_STATE::INACTIVE].PATTERN_WIDTH * (frame % patternNumU);
	float ty = portal_pic[PORTAL_STATE::INACTIVE].PATTERN_HIGHT * (frame / patternNumU);
	float tw = portal_pic[PORTAL_STATE::INACTIVE].PATTERN_WIDTH;
	float th = portal_pic[PORTAL_STATE::INACTIVE].PATTERN_HIGHT;

	for (int i = 0; i < PORTAL_MAX_NUM; i++)
	{
		if (!portal[i].use) continue;

		DrawSpriteAnimation_Scroll(
			portal[i].pos.x, portal[i].pos.y,
			PORTAL_SIZE_X, PORTAL_SIZE_Y,
			MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f,
			tx, ty, tw, th,
			portalTextureID[portal[i].state],
			true
		);
	}
}

void FinalizePortal()
{
	//チュートリアル
	FinalizeTutorialPic();

	for (int i = 0; i < PORTAL_STATE::PORTAL_STATE_MAX; i++)
	{
		UnloadTexture(portalTextureID[i]);
	}

	for (int i = 0; i < PORTAL_MAX_NUM; i++)
	{
		if (!portal[i].use) continue;
		portal[i].use = false;
	}

	portal_anime_frame = 0;
}

// =========================================================
// 今のステージのポータルをロードする
// =========================================================
void ReloadPortalStage()
{
	GAME_STAGE currentStage = GetCurrentGameStage();

	portal_anime_frame = 0;
	for (int i = 0; i < PORTAL_MAX_NUM; i++)
	{
		portal[i].use = false;
		portal[i].state = PORTAL_STATE::INACTIVE;

		if (portal[i].myStage != currentStage) continue;

		portal[i].use = true;
	}
}

// =========================================================
// ターゲットステージのゲッター
// =========================================================
GAME_STAGE GetCurrentPortalTargetStage()
{
	for (int i = 0; i < PORTAL_MAX_NUM; i++)
	{
		if (!portal[i].use) continue;

		return portal[i].targetStage;
	}

	return GAME_STAGE_MAX;
}

// =========================================================
// コリジョン判定
// =========================================================
bool PortalAndPlayerCollision()
{
	PLAYER* player = GetPlayer();
	bool ret { false };
	for (int i = 0; i < PORTAL_MAX_NUM; i++)
	{
		if (!portal[i].use) continue;
		if (CheckBoxCollider(
			portal[i].pos, player->pos,
			MakeFloat2(PORTAL_COLLISION_X, PORTAL_COLLISION_Y), player->CollisionSize
		))
		{
			ret = true;
		}
	}

	return ret;
}

// =========================================================
// ポータルゲッター
// =========================================================
PORTAL* GetPortal()
{
	return &portal[0];
}

void InitializeTutorialPic()
{
	for (int i = 0; i < TUTORIAL_PIC_NUM; i++)
	{
		tutorialTextureID[i] = LoadTexture(tutorial_pic[i].FILE_NAME);
	}
}

// =========================================================
// チュートリアルの説明図描画
// =========================================================
void DrawTutorialPic()
{
	for (int i = 0; i < TUTORIAL_PIC_NUM; i++)
	{
		if (tutorial_pic[i].stage != GetCurrentGameStage()) continue;
		DrawSpriteQuad_Scroll(
			tutorial_pic[i].pos.x, tutorial_pic[i].pos.y,
			tutorial_pic[i].size.x, tutorial_pic[i].size.y,
			tutorialTextureID[i], true
		);
	}
}

void FinalizeTutorialPic()
{
	for (int i = 0; i < TUTORIAL_PIC_NUM; i++)
	{
		UnloadTexture(tutorialTextureID[i]);
	}
}