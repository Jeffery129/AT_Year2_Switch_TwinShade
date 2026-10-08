// =========================================================
// fade.cpp フェード
//   
// 制作者:		日付：
// =========================================================
#include "main.h"
#include "texture.h"
#include "sprite.h"
#include "fade.h"

// =========================================================
// マクロ定義
// =========================================================
#define FADE_SPEED			(0.03f)
#define FADE_IN_GAME_SPEED	(0.04f)

// =========================================================
// グローバル変数
// =========================================================
FADE fade;					// シーン切り替え用フェード
FADE fadeInGame;			// ゲーム内フェード

unsigned int FadeTextureId;	// テクスチャID
SCENE NextScene;			// 次のシーン

// =========================================================
// フェード初期化
// =========================================================
void InitializeFade(void)
{
	fade.pos = MakeFloat2(0.0f, 0.0f);
	fade.size = MakeFloat2(SCREEN_WIDTH, SCREEN_HEIGHT);
	fade.color = MakeFloat4(0.0f, 0.0f, 0.0f, 0.0f);
	fade.state = FADE_NONE;

	fadeInGame.pos = MakeFloat2(0.0f, 0.0f);
	fadeInGame.size = MakeFloat2(SCREEN_WIDTH, SCREEN_HEIGHT);
	fadeInGame.color = MakeFloat4(0.0f, 0.0f, 0.0f, 0.0f);
	fadeInGame.state = FADE_NONE;

	// テクスチャ読み込み（しない）
	FadeTextureId = 0;
}

// =========================================================
// フェード更新
// =========================================================
void UpdateFade(void)
{
	// ==========================================
	// シーン切り替え用フェード
	// ==========================================
	switch (fade.state)
	{
	case FADE_NONE:
		break;

	case FADE_OUT:
		fade.color.w += FADE_SPEED;

		if (fade.color.w >= 1.0f)
		{// フェードアウト終了
			fade.color.w = 1.0f;

			// フェードインが始まる
			fade.state = FADE_IN;

			// 次のシーンへ切り替え
			SetScene(NextScene);
		}
		break;

	case FADE_IN:
		fade.color.w -= FADE_SPEED;

		if (fade.color.w <= 0.0f)
		{// フェードイン終了
			fade.color.w = 0.0f;

			// フェードが止まる
			fade.state = FADE_NONE;
		}
		break;

	default:
		break;
	}

	// ==========================================
	// ゲーム内フェード
	// ==========================================
	switch (fadeInGame.state)
	{
	case FADE_NONE:
		break;

	case FADE_OUT:
		fadeInGame.color.w += FADE_IN_GAME_SPEED;

		if (fadeInGame.color.w >= 1.0f)
		{// ゲーム内フェードアウト終了
			fadeInGame.color.w = 1.0f;

			fadeInGame.state = FADE_IN;
		}
		break;

	case FADE_IN:
		fadeInGame.color.w -= FADE_IN_GAME_SPEED;

		if (fadeInGame.color.w <= 0.0f)
		{// ゲーム内フェードイン終了
			fadeInGame.color.w = 0.0f;

			// ゲーム内フェードが止まる
			fadeInGame.state = FADE_NONE;
		}
		break;

	default:
		break;
	}
}

// =========================================================
// フェード描画
// =========================================================
void DrawFade(void)
{
	// シーン切り替え用フェード
	DrawSpriteQuad(
		fade.pos.x, fade.pos.y,
		fade.size.x, fade.size.y,
		fade.color,
		FadeTextureId
	);

	// ゲーム内フェード
	DrawSpriteQuad(
		fadeInGame.pos.x, fadeInGame.pos.y,
		fadeInGame.size.x, fadeInGame.size.y,
		fadeInGame.color,
		FadeTextureId
	);
}

// =========================================================
// フェード終了処理
// =========================================================
void FinalizeFade(void)
{
	UnloadTexture(FadeTextureId);

	fade.state = FADE_NONE;
	fadeInGame.state = FADE_NONE;
}

// =========================================================
// シーン切り替え用フェードアウト開始
// =========================================================
void StartFade(SCENE next)
{
	if (fade.state != FADE_NONE) return;

	fade.color.w = 0.0f;
	fade.state = FADE_OUT;
	NextScene = next;
}

// =========================================================
// ゲーム内フェードアウト開始
// =========================================================
void StartFadeInGame(void)
{
	if (fadeInGame.state != FADE_NONE) return;

	fadeInGame.color.w = 0.0f;
	fadeInGame.state = FADE_OUT;
}

// =========================================================
// 現在のフェードを取得
// =========================================================
FADE* GetFade(void)
{
	return &fade;
}

// =========================================================
// 現在のゲーム内フェードを取得
// =========================================================
FADE* GetFadeInGame(void)
{
	return &fadeInGame;
}