// =========================================================
// bullet.h プレイヤー弾制御
//
// 制作者:        日付：
// =========================================================
#ifndef _BULLET_H_
#define _BULLET_H_

// =========================================================
// マクロ定義
// =========================================================
#define MAX_BULLET       (100)
#define BULLET_PIC_NUM   (2)
#define BULLET_COLOR_NUM (2)

// =========================================================
// 列挙型定義
// =========================================================
enum BULLET_PIC_TYPE
{
	BULLET_PIC_NORMAL = 0,
	BULLET_PIC_CHARGE,
};

enum BULLET_COLOR_TYPE
{
	BULLET_COLOR_RED = 0,
	BULLET_COLOR_BLUE,
};

// =========================================================
// 構造体宣言
// =========================================================
struct Pic_Data_Bullet
{
	const char FILE_NAME[256]{};
	Float2 PIC_SIZE{};
	int PATTERN_MAX{};
	int PATTERN_NUM_U{};
	int PATTERN_NUM_V{};
	int ANIME_SPEED{};

	float PATTERN_WIDTH{ 1.0f / PATTERN_NUM_U };
	float PATTERN_HIGHT{ 1.0f / PATTERN_NUM_V };
};

struct BULLET
{
	Float2 pos;				// 座標
	Float2 vel;				// 移動値
	Float2 size;			// サイズ
	Float2 dir;				// 発射方向
	Float2 CollisionPosition; // 当たり判定中心座標
	Float2 CollisionSize;	  // 当たり判定サイズ
	float rotation;			// 回転角度
	float scale;			// スケール
	float damage;			// ダメージ

	int picType;			// 通常弾・チャージ弾
	int colorType;			// 発射時の色
	int animeFrame;			// アニメーションフレーム
	int lifeFrame;			// 生存フレーム

	bool use;				// 使用フラグ
};

// =========================================================
// アニメーションデータ
// =========================================================
extern const Pic_Data_Bullet bullet_pic[BULLET_PIC_NUM][BULLET_COLOR_NUM];
extern const Pic_Data_Bullet hit_pic[BULLET_PIC_NUM][BULLET_COLOR_NUM];

// =========================================================
// プロトタイプ宣言
// =========================================================
void InitializeBullet(void);
void UpdateBullet(void);
void DrawBullet(void);
void FinalizeBullet(void);
void ResetBullet();

void SetBullet(Float2 pos, Float2 dir, float scale, int picType, int colorType);
BULLET* GetBullet(void);

#endif