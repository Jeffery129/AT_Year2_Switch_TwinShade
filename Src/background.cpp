// =========================================================
// background.cpp
//
// §ìÒ:		“ú•tF
// =========================================================
#include "background.h"

// =========================================================
// ƒOƒ[ƒoƒ‹•Ï”
// =========================================================
unsigned int g_bgTextureId[GAME_STAGE_MAX][BG_PIC_NUMBER]{};

// =========================================================
// ”wŒi‰Šú‰»
// =========================================================
void InitializeBG()
{
	for (int stage = 0; stage < GAME_STAGE_MAX; stage++)
	{
		for (int layer = 0; layer < BG_PIC_NUMBER; layer++)
		{
			g_bgTextureId[stage][layer] = LoadTexture(g_bg[stage][layer].FILE_NAME);
		}
	}
}

// =========================================================
// ”wŒiXV
// =========================================================
void UpdateBG()
{
}

// =========================================================
// ”wŒi•`‰æ
// =========================================================
void DrawBG()
{
	GAME_STAGE currentStage = GetCurrentGameStage();

	if (currentStage < GAME_STAGE_T_01 || currentStage >= GAME_STAGE_MAX) return;

	Float2 scroll = GetOffset_Scroll();

	for (int i = 0; i < BG_PIC_NUMBER; i++)
	{
		if (g_bgTextureId[currentStage][i] == 0) continue;

		float parallaxRate = g_bg[currentStage][i].speed_layer * BG_PARALLAX_RATE;

		if (currentStage == GAME_STAGE_T_02)
		{
			float tx = (-scroll.x / 2700.0f) * parallaxRate;

			DrawSpriteQuad_UV(
				0.0f, 0.0f,
				2700.0f, SCREEN_HEIGHT,
				tx, 0.0f,
				1.0f, 1.0f,
				g_bgTextureId[currentStage][i]
			);

			continue;
		}

		float tx = (-scroll.x / SCREEN_WIDTH) * parallaxRate;

		DrawSpriteQuad_UV(
			0.0f, 0.0f,
			SCREEN_WIDTH, SCREEN_HEIGHT,
			tx, 0.0f,
			1.0f, 1.0f,
			g_bgTextureId[currentStage][i]
		);
	}
}

// =========================================================
// ”wŒiI—¹ˆ—
// =========================================================
void FinalizeBG()
{
	for (int stage = 0; stage < GAME_STAGE_MAX; stage++)
	{
		for (int layer = 0; layer < BG_PIC_NUMBER; layer++)
		{
			if (g_bgTextureId[stage][layer] == 0) continue;

			UnloadTexture(g_bgTextureId[stage][layer]);
			g_bgTextureId[stage][layer] = 0;
		}
	}
}