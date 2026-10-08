// =========================================================
// enemy_bullet.h 敵弾制御
//
// 制作者:        日付：
// =========================================================
#ifndef _ENEMY_BULLET_H_
#define _ENEMY_BULLET_H_

#include "main.h"

// =========================================================
// マクロ定義
// =========================================================
#define MAX_ENEMY_BULLET (100)

// =========================================================
// 構造体 & 列挙体
// =========================================================
enum BULLET_BELONGS_TO
{
	MUSH_RANGE,

	// ほかの敵ここ追加
	BELONGS_MAX
};

struct BULLET_DATA
{
	float bulletSpeed{};
	int lifeFrame{};
	bool isNeedRotation{};
	bool isFollowPlayer{};
	bool isGoThroughPlayer{};			// プレイヤーに貫通するか？
	bool isGoThroughBlock{};			// ブロックに貫通するか？
	bool canBeDestroyed{};				// プレイヤーの弾に消せるか？
	int needHowManyHitToBeDestroyed{};	// 壊すのにNormal Bulletが何発必要か
};

struct Pic_Data_Enemy_Bullet
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

struct ENEMY_BULLET
{
	Float2 pos{};
	Float2 vel{};
	Float2 size{};
	Float2 dir{};
	Float2 CollisionPosition{};
	Float2 CollisionSize{};
	Float4 color{};
	float rotation{};
	float damage{};

	int animeFrame{};
	int lifeFrame{};
	int life{};
	int destroyFrame{};

	BULLET_BELONGS_TO myMaster{};

	bool isFollowPlayer{};
	bool isGoThroughPlayer{};
	bool isGoThroughBlock{};
	bool canBeDestroyed{};
	bool isBeingAimed{};
	bool isDestroying{};
	bool use{};
};

// =========================================================
// プロトタイプ宣言
// =========================================================
void InitializeEnemyBullet();
void UpdateEnemyBullet();
void DrawEnemyBullet();
void FinalizeEnemyBullet();

void SetEnemyBullet(Float2 pos, Float2 targetPos, float damage, BULLET_BELONGS_TO whoseBullet);

bool PlayerBulletEnemyBulletCollision(Float2 bulletCollisionPosition, Float2 bulletCollisionSize, int bulletType);
void StartDestroyEnemyBullet(ENEMY_BULLET* targetBullet);

ENEMY_BULLET* GetEnemyBullet();

void ResetEnemyBullet();

#endif