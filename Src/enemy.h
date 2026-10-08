// =========================================================
// enemy.h 敵管理
//
// 制作者:        日付：
// =========================================================
#ifndef _Enemy_H_
#define _Enemy_H_

//---必ず入れる----
#include "main.h"
//-----------------

#define MAX_ENEMY (50)
#define MUSHROOM_MELEE_PIC_NUM (6)
#define MUSHROOM_RANGE_PIC_NUM (6)

//--------------------------------------------------------------
// 構造体定義 (*´▽｀*)
//--------------------------------------------------------------
struct Pic_Data_Enemy
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

enum ENEMY_STATE
{
	ENEMY_STATE_IDLE = 0,
	ENEMY_STATE_RUN,
	ENEMY_STATE_ATTACK,
	ENEMY_STATE_HIT,
	ENEMY_STATE_DEAD,
	ENEMY_STATE_ALERT,

	ENEMY_STATE_MAX
};

struct Enemy
{
	Float2 pos{};
	Float2 size{};
	Float4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
	float rotation{};
	float scale{};
	bool isFacingRight{ true };
	bool use{ false };

	Float2 vel{};
	Float2 exVel{};

	ENEMY_STATE state{ ENEMY_STATE_IDLE };

	Float2 CollisionSize{};        // Bulletとの当たり判定サイズ
	Float2 CollisionPosition{};    // Bulletとの当たり判定の中心座標
	Float2 CollisionOffset{};      // 当たり判定中心座標のずれ、画像によって調整

	float hp{};                    // 体力
	float atk{};                   // 攻撃力
	float attackRange{};           // 攻撃開始範囲
	float searchRange{};           // 察敵範囲
	float patrolSpeed{};           // 巡回速度
	float chaseSpeed{};            // 追跡速度

	float patrolLeft{};            // 巡回範囲の左端
	float patrolRight{};           // 巡回範囲の右端
	float groundY{};               // 巡回する地面のY座標
	float gravityAcc{};            // 重力加速度

	int moveDir{ 1 };              // -1：左、1：右
	int stateTimer{};
	int animeFrame{};

	bool isChasing{ false };
	bool hasShownAlert{ false };   // Alert Effectを表示済み

	int lostPlayerTimer{};
	Float2 lastPlayerPos{};

	bool isBeingAimed{ false };	  // 狙われている
};

struct Mushroom_Melee : public Enemy
{
	int attackCollisionStartFrame{ 5 }; // Attackの5フレーム目から最後までだけ、プレイヤーと当たり判定
	int attackCoolTimer{};
	bool hasHitPlayer{ false };
};

struct Mushroom_Range : public Enemy
{
	int shootPattern{ 5 }; // Attackの10フレーム目に毒霧の弾をうつ
	int attackCoolTimer{};
	bool hasShot{ false };
};

// プロトタイプ宣言
void InitializeEnemy();
void UpdateEnemy();
void DrawEnemy();
void FinalizeEnemy();
void ReloadEnemyStage();

// Bulletとの当たり判定
bool BulletEnemyCollision(Float2 bulletCollisionPosition, Float2 bulletCollisionSize, Float2 bulletDir, float bulletDamage, int bulletType);

// Charge Bullet ExplosionとのAOE当たり判定
bool ChargeExplosionCollisionEnemy(Float2 explosionPos, float explosionRadius, float explosionDamage);

// 近接キノコ
Mushroom_Melee* GetMushroomMelee();
Mushroom_Range* GetMushroomRange();

#endif //_Enemy_H_