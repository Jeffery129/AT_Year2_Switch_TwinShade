// =========================================================
// explosion.h 爆発アニメーション制御
// 
// 制作者:		日付：
// =========================================================
#ifndef _EXPLOSION_H_
#define _EXPLOSION_H_

// =========================================================
// マクロ定義
// =========================================================
#define MAX_EXPLOSION (100)		// 爆発アニメーションの総数

// =========================================================
// 構造体宣言
// =========================================================
struct EXPLOSION {
	Float2 pos;		// 座標
	Float2 size;	// サイズ
	int frame;		// フレーム
	bool use;		// 使用フラグ
};

// =========================================================
// プロトタイプ宣言
// =========================================================
void InitializeExplosion(void);
void UpdateExplosion(void);
void DrawExplosion(void);
void FinalizeExplosion(void);
EXPLOSION* GetExplosion(void);
void SetExplosion(Float2 p, Float2 s);
void SetExplosion(Float2 p, Float2 s, int picType, int colorType, float rotation);

void ResetExplosion();

#endif