// =========================================================
// titlebg.cpp タイトル背景
//
// 制作者:        日付：
// =========================================================
#include "main.h"
#include "texture.h"
#include "sprite.h"
#include "titlebg.h"
#include "particle_waterfall.h"
#include <cmath>

// =========================================================
// マクロ定義
// =========================================================
#define TITLE_BG_TEXT_POS_X				(0.0f)
#define TITLE_BG_TEXT_POS_Y				(-SCREEN_HEIGHT / 2 + 330.0f)
#define TITLE_BG_TEXT_SIZE_W			(768.0f)
#define TITLE_BG_TEXT_SIZE_H			(340.0f)
#define TITLE_BG_TEXT_SCALE				(1.6f)
#define TITLE_BG_PRESS_POS_X			(0.0f)
#define TITLE_BG_PRESS_POS_Y			(SCREEN_HEIGHT / 2 - 80.0f)
#define TITLE_BG_PRESS_SIZE_W			(610.0f)
#define TITLE_BG_PRESS_SIZE_H			(110.0f)
#define TITLE_BG_PRESS_SCALE			(1.1f)

// Text・Press上下浮動
#define TITLE_BG_TEXT_FLOAT_HEIGHT		(10.0f)
#define TITLE_BG_TEXT_FLOAT_SPEED		(0.02f)
#define TITLE_BG_PRESS_FLOAT_HEIGHT		(4.0f)
#define TITLE_BG_PRESS_FLOAT_SPEED		(0.04f)
#define TITLE_BG_PRESS_FLOAT_PHASE		(1.0f)

// タイトル画面Particle設定
#define TITLE_PARTICLE_RED_POS_X		(-SCREEN_WIDTH / 4.0f - 100.0f)
#define TITLE_PARTICLE_BLUE_POS_X		(SCREEN_WIDTH / 4.0f + 100.0f)
#define TITLE_PARTICLE_POS_Y			(SCREEN_HEIGHT / 2 - 150.0f)
#define TITLE_PARTICLE_POS_X_RANGE		(static_cast<int>((SCREEN_WIDTH / 2.0f) * 0.7f))
#define TITLE_PARTICLE_DIRECTION_ROT	(0.0f)
#define TITLE_PARTICLE_SPEED_RANGE		(20)
#define TITLE_PARTICLE_START_SPEED_Y	(2.0f)
#define TITLE_PARTICLE_START_SCALE		(1.0f)
#define TITLE_PARTICLE_NUM_PER_SHOOT	(1)
#define TITLE_PARTICLE_FRAME_PER_SHOOT	(5)

// =========================================================
// 列挙体定義
// =========================================================
enum TITLE_BG_STATIC_PIC
{
	TITLE_BG_BACK = 0,
	TITLE_BG_FRONT,
	TITLE_BG_PRESS,
	TITLE_BG_TEXT,
	TITLE_BG_STATIC_PIC_NUM
};

// =========================================================
// 画像データ
// =========================================================
const char* titleBgFileName_Static[TITLE_BG_STATIC_PIC_NUM]
{
	"rom:/Title_Menu_Background.tga",
	"rom:/Title_Menu_Front.tga",
	"rom:/Title_Menu_Press.tga",
	"rom:/Title_Menu_Text.tga"
};

// =========================================================
// グローバル変数
// =========================================================
unsigned int titleBgStaticTextureId[TITLE_BG_STATIC_PIC_NUM]{};
unsigned int TitlebgAnimeFrame{};

// =========================================================
// タイトル背景初期化
// =========================================================
void InitializeTitlebg(void)
{
	// タイトル固定画像を読み込む
	for (int i = 0; i < TITLE_BG_STATIC_PIC_NUM; i++)
	{
		titleBgStaticTextureId[i] = LoadTexture(titleBgFileName_Static[i]);
	}

	// Red・Blue Particleを読み込む
	InitializeParticleWaterfall();
	TitlebgAnimeFrame = 0;
}

// =========================================================
// タイトル背景更新
// =========================================================
void UpdateTitlebg(void)
{
	TitlebgAnimeFrame++;
	UpdateParticleWaterfall();
}

// =========================================================
// タイトル背景描画
// =========================================================
void DrawTitlebg(void)
{
	// 一番下の背景
	DrawSpriteQuad(
		0.0f, 0.0f,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f,
		titleBgStaticTextureId[TITLE_BG_BACK]
	);

	// 左側のRed Particle
	DrawParticleWaterfall(
		TITLE_PARTICLE_RED_POS_X, TITLE_PARTICLE_POS_Y,
		TITLE_PARTICLE_POS_X_RANGE, TITLE_PARTICLE_DIRECTION_ROT,
		TITLE_PARTICLE_SPEED_RANGE, TITLE_PARTICLE_START_SPEED_Y,
		TITLE_PARTICLE_START_SCALE, TITLE_PARTICLE_NUM_PER_SHOOT,
		TITLE_PARTICLE_FRAME_PER_SHOOT, PARTICLE_WATERFALL_RED
	);

	// 右側のBlue Particle
	DrawParticleWaterfall(
		TITLE_PARTICLE_BLUE_POS_X, TITLE_PARTICLE_POS_Y,
		TITLE_PARTICLE_POS_X_RANGE, TITLE_PARTICLE_DIRECTION_ROT,
		TITLE_PARTICLE_SPEED_RANGE, TITLE_PARTICLE_START_SPEED_Y,
		TITLE_PARTICLE_START_SCALE, TITLE_PARTICLE_NUM_PER_SHOOT,
		TITLE_PARTICLE_FRAME_PER_SHOOT, PARTICLE_WATERFALL_BLUE
	);

	// Particleより前に表示する背景
	DrawSpriteQuad(
		0.0f, 0.0f,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f,
		titleBgStaticTextureId[TITLE_BG_FRONT]
	);

	// TextとPressの上下浮動量を計算
	float animeFrame = static_cast<float>(TitlebgAnimeFrame);
	float textFloatY = sinf(animeFrame * TITLE_BG_TEXT_FLOAT_SPEED)
		* TITLE_BG_TEXT_FLOAT_HEIGHT;
	float pressFloatY = sinf(
		animeFrame * TITLE_BG_PRESS_FLOAT_SPEED
		+ TITLE_BG_PRESS_FLOAT_PHASE
	) * TITLE_BG_PRESS_FLOAT_HEIGHT;

	// Title Text
	DrawSpriteQuad(
		TITLE_BG_TEXT_POS_X,
		TITLE_BG_TEXT_POS_Y + textFloatY,
		TITLE_BG_TEXT_SIZE_W * TITLE_BG_TEXT_SCALE,
		TITLE_BG_TEXT_SIZE_H * TITLE_BG_TEXT_SCALE,
		MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f,
		titleBgStaticTextureId[TITLE_BG_TEXT]
	);

	// Press Any Button
	DrawSpriteQuad(
		TITLE_BG_PRESS_POS_X,
		TITLE_BG_PRESS_POS_Y + pressFloatY,
		TITLE_BG_PRESS_SIZE_W * TITLE_BG_PRESS_SCALE,
		TITLE_BG_PRESS_SIZE_H * TITLE_BG_PRESS_SCALE,
		MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f,
		titleBgStaticTextureId[TITLE_BG_PRESS]
	);
}

// =========================================================
// タイトル背景終了処理
// =========================================================
void FinalizeTitlebg(void)
{
	// Particle Textureを先に解放
	FinalizeParticleWaterfall();

	for (int i = 0; i < TITLE_BG_STATIC_PIC_NUM; i++)
	{
		if (titleBgStaticTextureId[i] == 0) continue;

		UnloadTexture(titleBgStaticTextureId[i]);
		titleBgStaticTextureId[i] = 0;
	}

	TitlebgAnimeFrame = 0;
}
