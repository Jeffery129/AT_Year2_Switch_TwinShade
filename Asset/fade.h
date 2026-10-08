// =========================================================
// fade.h フェード
// 
// 制作者:		日付：
// =========================================================
#ifndef _FADE_H_
#define _FADE_H_
#include "main.h"
#include "texture.h"
#include "sprite.h"
// =========================================================
// 列挙体宣言
// =========================================================
enum FADE_STATE {
	FADE_NONE = 0,	// 何も動いていないとき
	FADE_OUT,		// 
	FADE_IN,		// 
	FADE_MAX
};

// =========================================================
// 構造体宣言
// =========================================================
struct FADE {
	Float2 pos;		// 座標
	Float2 size;	// サイズ
	Float4 color;
	FADE_STATE state;	// フェードの状態
};

// =========================================================
// プロトタイプ宣言
// =========================================================
void InitializeFade(void);
void UpdateFade(void);
void DrawFade(void);
void FinalizeFade(void);

void StartFade(SCENE next);
void StartFadeInGame(void);

FADE* GetFade(void);
FADE* GetFadeInGame(void);

#endif