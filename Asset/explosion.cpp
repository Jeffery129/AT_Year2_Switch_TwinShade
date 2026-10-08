// =========================================================
// explosion.cpp 爆発アニメーション制御
// 
// 制作者:		日付：
// =========================================================
#include "main.h"
#include "texture.h"
#include "sprite.h"
#include "explosion.h"
#include "bullet.h"

// =========================================================
// グローバル変数
// =========================================================
EXPLOSION explosion[MAX_EXPLOSION];	// 爆発アニメーションの実体
unsigned int ExplosionTextureId[BULLET_PIC_NUM][BULLET_COLOR_NUM];

// 各ヒットエフェクトの種類と色
int ExplosionPicType[MAX_EXPLOSION];
int ExplosionColorType[MAX_EXPLOSION];
float ExplosionRotation[MAX_EXPLOSION];

// =========================================================
// 爆発アニメーション初期化
// =========================================================
void InitializeExplosion(void)
{
	for (int i = 0; i < MAX_EXPLOSION; i++)
	{
		explosion[i].pos = MakeFloat2(0.0f, 0.0f);		// 座標
		explosion[i].size = MakeFloat2(0.0f, 0.0f);		// サイズ
		explosion[i].frame = 0;						// フレーム
		explosion[i].use = false;					// 使用フラグ

		ExplosionPicType[i] = BULLET_PIC_NORMAL;
		ExplosionColorType[i] = BULLET_COLOR_RED;
		ExplosionRotation[i] = 0.0f;
	}

	for (int i = 0; i < BULLET_PIC_NUM; i++)
	{
		for (int j = 0; j < BULLET_COLOR_NUM; j++)
		{
			ExplosionTextureId[i][j] = LoadTexture(hit_pic[i][j].FILE_NAME);
		}
	}
}

// =========================================================
// 爆発アニメーション更新
// =========================================================
void UpdateExplosion(void)
{
	for (int i = 0; i < MAX_EXPLOSION; i++)
	{
		if (!explosion[i].use) continue;

		int picType = ExplosionPicType[i];
		int colorType = ExplosionColorType[i];

		if (picType < 0 || picType >= BULLET_PIC_NUM)
		{
			picType = BULLET_PIC_NORMAL;
		}

		if (colorType < 0 || colorType >= BULLET_COLOR_NUM)
		{
			colorType = BULLET_COLOR_RED;
		}

		int animeSpeed = hit_pic[picType][colorType].ANIME_SPEED;
		if (animeSpeed <= 0) animeSpeed = 1; // ゼロ除算の防止

		explosion[i].frame++;

		int currentPattern = explosion[i].frame / animeSpeed;

		if (currentPattern >= hit_pic[picType][colorType].PATTERN_MAX)
		{
			explosion[i].use = false; // アニメーション終了
		}
	}
}

// =========================================================
// 爆発アニメーション描画
// =========================================================
void DrawExplosion(void)
{
	for (int i = 0; i < MAX_EXPLOSION; i++)
	{
		if (!explosion[i].use) continue;

		int picType = ExplosionPicType[i];
		int colorType = ExplosionColorType[i];

		if (picType < 0 || picType >= BULLET_PIC_NUM)
		{
			picType = BULLET_PIC_NORMAL;
		}

		if (colorType < 0 || colorType >= BULLET_COLOR_NUM)
		{
			colorType = BULLET_COLOR_RED;
		}

		int animeSpeed = hit_pic[picType][colorType].ANIME_SPEED;
		if (animeSpeed <= 0) animeSpeed = 1; // ゼロ除算の防止

		int patternMax = hit_pic[picType][colorType].PATTERN_MAX;
		if (patternMax <= 0) patternMax = 1; // ゼロ除算の防止

		int frame = (explosion[i].frame / animeSpeed) % patternMax;
		float tx = hit_pic[picType][colorType].PATTERN_WIDTH * (frame % hit_pic[picType][colorType].PATTERN_NUM_U);
		float ty = hit_pic[picType][colorType].PATTERN_HIGHT * (frame / hit_pic[picType][colorType].PATTERN_NUM_U);
		float tw = hit_pic[picType][colorType].PATTERN_WIDTH;
		float th = hit_pic[picType][colorType].PATTERN_HIGHT;

		DrawSpriteAnimation_Scroll(
			explosion[i].pos.x,
			explosion[i].pos.y,
			explosion[i].size.x,
			explosion[i].size.y,
			MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f),
			ExplosionRotation[i],
			tx,
			ty,
			tw,
			th,
			ExplosionTextureId[picType][colorType],
			false
		);
	}
}

// =========================================================
// 爆発アニメーション終了処理
// =========================================================
void FinalizeExplosion(void)
{
	for (int i = 0; i < BULLET_PIC_NUM; i++)
	{
		for (int j = 0; j < BULLET_COLOR_NUM; j++)
		{
			UnloadTexture(ExplosionTextureId[i][j]);
		}
	}
}

// =========================================================
// 爆発アニメーションのアドレス取得
// =========================================================
EXPLOSION* GetExplosion(void)
{
	return &explosion[0];
}

// =========================================================
// 爆発アニメーションのセット処理
// =========================================================
void SetExplosion(Float2 p, Float2 s)
{
	for (int i = 0; i < MAX_EXPLOSION; i++)
	{
		if (!explosion[i].use)
		{
			explosion[i].pos = p;		// 座標
			explosion[i].size = s;		// サイズ
			explosion[i].frame = 0;

			explosion[i].use = true;		// 使用フラグ
			
			break;
		}
	}
}

void SetExplosion(Float2 p, Float2 s, int picType, int colorType, float rotation)
{
	for (int i = 0; i < MAX_EXPLOSION; i++)
	{
		if (!explosion[i].use)
		{
			if (picType < 0 || picType >= BULLET_PIC_NUM)
			{
				picType = BULLET_PIC_NORMAL;
			}

			if (colorType < 0 || colorType >= BULLET_COLOR_NUM)
			{
				colorType = BULLET_COLOR_RED;
			}

			explosion[i].pos = p;
			explosion[i].size = s;
			explosion[i].frame = 0;
			explosion[i].use = true;
			ExplosionPicType[i] = picType;
			ExplosionColorType[i] = colorType;
			ExplosionRotation[i] = rotation;
			break;
		}
	}
}

// =========================================================
// 爆発のリセット
// =========================================================
void ResetExplosion()
{
	for (int i = 0; i < MAX_EXPLOSION; i++)
	{
		explosion[i] = EXPLOSION{};
	}
}