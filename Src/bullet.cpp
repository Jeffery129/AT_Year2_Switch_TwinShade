// =========================================================
// bullet.cpp プレイヤー弾制御
//
// 制作者:        日付：
// =========================================================
#include "main.h"
#include "texture.h"
#include "sprite.h"
#include "collision.h"
#include "block.h"
#include "explosion.h"
#include "camera.h"
#include "player.h"
#include "bullet.h"
#include "enemy.h"
#include "boss.h"
#include "enemy_bullet.h"
#include "color_change_block.h"
#include "sound.h"

// =========================================================
// マクロ定義
// =========================================================
#define BULLET_SPEED                   (30.0f)
#define BULLET_LIFE_FRAME              (180)

// 弾頭Colliderの高さに対する割合
#define BULLET_HEAD_COLLISION_RATE     (0.5f)

// カメラシェーク
#define NORMAL_SHOOT_SHAKE_POWER       (10.0f)
#define NORMAL_SHAKE_FRAME             (5)
#define CHARGE_SHOOT_SHAKE_MIN_POWER   (20.0f)
#define CHARGE_SHOOT_SHAKE_MAX_POWER   (40.0f)
#define CHARGE_SHAKE_FRAME             (10)
#define CHARGE_MIN_SCALE               (5.0f)
#define CHARGE_MAX_SCALE               (6.5f)

// Bulletダメージ 
#define BULLET_NORMAL_DAMAGE           (10.0f)
#define BULLET_CHARGE_MIN_DAMAGE       (30.0f)
#define BULLET_CHARGE_MAX_DAMAGE       (60.0f)
#define BULLET_CHARGE_EXPLOSION_DAMAGE (15.0f)

// Charge Bullet
#define CHARGE_BULLET_SCALE_RATE	   (1.2f)

// =========================================================
// 弾アニメーションデータ
// =========================================================
const Pic_Data_Bullet bullet_pic[BULLET_PIC_NUM][BULLET_COLOR_NUM]
{
	// BULLET_PIC_NORMAL
	{
		{ "rom:/Normal_Bullet_Red.tga", MakeFloat2(250, 32), 4, 4, 1, 5 },
		{ "rom:/Normal_Bullet_Blue.tga", MakeFloat2(250, 32), 4, 4, 1, 5 },
	},

	// BULLET_PIC_CHARGE
	{
		{ "rom:/Charged_Bullet_Red.tga", MakeFloat2(192, 32), 4, 4, 1, 5 },
		{ "rom:/Charged_Bullet_Blue.tga", MakeFloat2(192, 32), 4, 4, 1, 5 },
	},
};

// =========================================================
// ヒットアニメーションデータ
// =========================================================
const Pic_Data_Bullet hit_pic[BULLET_PIC_NUM][BULLET_COLOR_NUM]
{
	// BULLET_PIC_NORMAL
	{
		{ "rom:/Normal_Hit_Red.tga", MakeFloat2(180, 36), 5, 5, 1, 5 },
		{ "rom:/Normal_Hit_Blue.tga", MakeFloat2(180, 36), 5, 5, 1, 5 },
	},

	// BULLET_PIC_CHARGE
	{
		{ "rom:/Charge_Hit_Red_v2.tga", MakeFloat2(520, 52), 10, 10, 1, 5 },
		{ "rom:/Charge_Hit_Blue_v2.tga", MakeFloat2(520, 52), 10, 10, 1, 5 },
	},
};

// =========================================================
// グローバル変数
// =========================================================
BULLET bullet[MAX_BULLET];

unsigned int
BulletTextureId[BULLET_PIC_NUM][BULLET_COLOR_NUM];

// =========================================================
// プロトタイプ宣言
// =========================================================
void UpdateBulletCollision(BULLET* targetBullet);
Float2 GetBulletHitEffectSize(const BULLET* targetBullet);

bool BulletBlockCollision(BULLET* targetBullet);

// =========================================================
// 弾初期化
// =========================================================
void InitializeBullet(void)
{
	for (int i = 0; i < MAX_BULLET; i++)
	{
		bullet[i].pos = MakeFloat2(0.0f, 0.0f);
		bullet[i].vel = MakeFloat2(0.0f, 0.0f);
		bullet[i].size = MakeFloat2(0.0f, 0.0f);
		bullet[i].dir = MakeFloat2(1.0f, 0.0f);
		bullet[i].CollisionPosition = MakeFloat2(0.0f, 0.0f);
		bullet[i].CollisionSize = MakeFloat2(0.0f, 0.0f);
		bullet[i].rotation = 0.0f;
		bullet[i].scale = 0.0f;
		bullet[i].damage = 0.0f;

		bullet[i].picType = BULLET_PIC_NORMAL;
		bullet[i].colorType = BULLET_COLOR_RED;

		bullet[i].animeFrame = 0;
		bullet[i].lifeFrame = 0;
		bullet[i].use = false;
	}

	// 弾テクスチャの読み込み
	for (int i = 0; i < BULLET_PIC_NUM; i++)
	{
		for (int j = 0; j < BULLET_COLOR_NUM; j++)
		{
			BulletTextureId[i][j] = LoadTexture(bullet_pic[i][j].FILE_NAME);
		}
	}
}

// =========================================================
// 弾更新
// =========================================================
void UpdateBullet(void)
{
	for (int i = 0; i < MAX_BULLET; i++)
	{
		if (!bullet[i].use) continue;

		// 移動前の座標を保存する
		Float2 oldPos = bullet[i].pos;

		// 弾を移動
		bullet[i].pos.x += bullet[i].vel.x;
		bullet[i].pos.y += bullet[i].vel.y;

		// 弾頭Colliderを更新する
		UpdateBulletCollision(&bullet[i]);

		// アニメーションフレームを更新
		bullet[i].animeFrame++;

		// 生存フレームを更新
		bullet[i].lifeFrame++;

		// =========================================================
		// ブロックとの当たり判定
		// =========================================================
		if (BulletBlockCollision(&bullet[i]))
		{
			// 壁の中へ入り過ぎないように移動前へ戻す
			bullet[i].pos = oldPos;

			// 移動前の位置に合わせてColliderも戻す
			UpdateBulletCollision(&bullet[i]);

			Float2 effectSize = GetBulletHitEffectSize(&bullet[i]);

			// Hitは子弾の頭部位置に表示する
			SetExplosion(
				bullet[i].CollisionPosition,
				effectSize,
				bullet[i].picType,
				bullet[i].colorType,
				bullet[i].rotation
			);

			switch (bullet[i].picType)
			{
			case BULLET_PIC_NORMAL:
				StartCameraShake(NORMAL_SHOOT_SHAKE_POWER, NORMAL_SHAKE_FRAME);
				PlaySE(SE_Normal_Hit);
				break;

			case BULLET_PIC_CHARGE:
				float chargeRate = (bullet[i].scale - CHARGE_MIN_SCALE) / (CHARGE_MAX_SCALE - CHARGE_MIN_SCALE);
				if (chargeRate < 0.0f) chargeRate = 0.0f;
				else if (chargeRate > 1.0f) chargeRate = 1.0f;

				PlaySE(SE_Charge_Hit);

				ChargeExplosionCollisionEnemy(bullet[i].CollisionPosition, effectSize.x / 2, BULLET_CHARGE_EXPLOSION_DAMAGE);

				float shakePower = LerpFloat(
					CHARGE_SHOOT_SHAKE_MIN_POWER,
					CHARGE_SHOOT_SHAKE_MAX_POWER,
					chargeRate
				);

				StartCameraShake(shakePower, CHARGE_SHAKE_FRAME);
				break;
			}

			bullet[i].use = false;
			continue;
		}

		// =========================================================
		// Enemy Bulletとの当たり判定
		// =========================================================
		if (PlayerBulletEnemyBulletCollision(bullet[i].CollisionPosition, bullet[i].CollisionSize, bullet[i].picType))
		{
			Float2 effectSize = GetBulletHitEffectSize(&bullet[i]);

			SetExplosion(
				bullet[i].CollisionPosition,
				effectSize,
				bullet[i].picType,
				bullet[i].colorType,
				bullet[i].rotation
			);

			switch (bullet[i].picType)
			{
			case BULLET_PIC_NORMAL:
				AddNormalHitCnt();
				PlaySE(SE_Normal_Hit);
				StartCameraShake(NORMAL_SHOOT_SHAKE_POWER, NORMAL_SHAKE_FRAME);
				break;

			case BULLET_PIC_CHARGE:
				float chargeRate = (bullet[i].scale - CHARGE_MIN_SCALE) / (CHARGE_MAX_SCALE - CHARGE_MIN_SCALE);
				if (chargeRate < 0.0f) chargeRate = 0.0f;
				else if (chargeRate > 1.0f) chargeRate = 1.0f;

				PlaySE(SE_Charge_Hit);

				ChargeExplosionCollisionEnemy(bullet[i].CollisionPosition, effectSize.x / 2, BULLET_CHARGE_EXPLOSION_DAMAGE);

				float shakePower = LerpFloat(
					CHARGE_SHOOT_SHAKE_MIN_POWER,
					CHARGE_SHOOT_SHAKE_MAX_POWER,
					chargeRate
				);

				StartCameraShake(shakePower, CHARGE_SHAKE_FRAME);
				break;
			}

			bullet[i].use = false;
			continue;
		}

		// =========================================================
		// Enemyとの当たり判定
		// =========================================================
		if (BulletEnemyCollision(bullet[i].CollisionPosition, bullet[i].CollisionSize, bullet[i].dir, bullet[i].damage, bullet[i].picType))
		{
			Float2 effectSize = GetBulletHitEffectSize(&bullet[i]);
			SetExplosion(
				bullet[i].CollisionPosition,
				effectSize,
				bullet[i].picType,
				bullet[i].colorType,
				bullet[i].rotation
			);

			switch (bullet[i].picType)
			{
			case BULLET_PIC_NORMAL:
				AddNormalHitCnt();
				PlaySE(SE_Normal_Hit);
				StartCameraShake(NORMAL_SHOOT_SHAKE_POWER, NORMAL_SHAKE_FRAME);
				break;

			case BULLET_PIC_CHARGE:
				float chargeRate = (bullet[i].scale - CHARGE_MIN_SCALE) / (CHARGE_MAX_SCALE - CHARGE_MIN_SCALE);
				if (chargeRate < 0.0f) chargeRate = 0.0f;
				else if (chargeRate > 1.0f) chargeRate = 1.0f;

				PlaySE(SE_Charge_Hit);

				ChargeExplosionCollisionEnemy(bullet[i].CollisionPosition, effectSize.x / 2, BULLET_CHARGE_EXPLOSION_DAMAGE);

				float shakePower = LerpFloat(
					CHARGE_SHOOT_SHAKE_MIN_POWER,
					CHARGE_SHOOT_SHAKE_MAX_POWER,
					chargeRate
				);

				StartCameraShake(shakePower, CHARGE_SHAKE_FRAME);
				break;
			}

			bullet[i].use = false;
			continue;
		}

		// =========================================================
		// BOSSとの当たり判定
		// =========================================================
		if (BulletBossCollision(bullet[i].CollisionPosition, bullet[i].CollisionSize, bullet[i].damage, bullet[i].colorType))
		{
			Float2 effectSize = GetBulletHitEffectSize(&bullet[i]);
			SetExplosion(
				bullet[i].CollisionPosition,
				effectSize,
				bullet[i].picType,
				bullet[i].colorType,
				bullet[i].rotation
			);

			switch (bullet[i].picType)
			{
			case BULLET_PIC_NORMAL:
				AddNormalHitCnt();
				PlaySE(SE_Normal_Hit);
				StartCameraShake(NORMAL_SHOOT_SHAKE_POWER, NORMAL_SHAKE_FRAME);
				break;

			case BULLET_PIC_CHARGE:
				float chargeRate = (bullet[i].scale - CHARGE_MIN_SCALE) / (CHARGE_MAX_SCALE - CHARGE_MIN_SCALE);
				if (chargeRate < 0.0f) chargeRate = 0.0f;
				else if (chargeRate > 1.0f) chargeRate = 1.0f;

				PlaySE(SE_Charge_Hit);

				float shakePower = LerpFloat(
					CHARGE_SHOOT_SHAKE_MIN_POWER,
					CHARGE_SHOOT_SHAKE_MAX_POWER,
					chargeRate
				);

				StartCameraShake(shakePower, CHARGE_SHAKE_FRAME);
				break;
			}

			bullet[i].use = false;
			continue;
		}

		// 長時間残っている弾を削除
		if (bullet[i].lifeFrame >= BULLET_LIFE_FRAME)
		{
			bullet[i].use = false;
		}
	}
}

// =========================================================
// 弾描画
// =========================================================
void DrawBullet(void)
{
	for (int i = 0; i < MAX_BULLET; i++)
	{
		if (!bullet[i].use) continue;

		int picType = bullet[i].picType;
		int colorType = bullet[i].colorType;

		if (picType < 0 || picType >= BULLET_PIC_NUM)
		{
			picType = BULLET_PIC_NORMAL;
		}

		if (colorType < 0 || colorType >= BULLET_COLOR_NUM)
		{
			colorType = BULLET_COLOR_RED;
		}

		int animeSpeed = bullet_pic[picType][colorType].ANIME_SPEED;

		if (animeSpeed <= 0)
		{
			animeSpeed = 1;
		}

		int patternMax = bullet_pic[picType][colorType].PATTERN_MAX;

		if (patternMax <= 0)
		{
			patternMax = 1;
		}

		int frame = (bullet[i].animeFrame / animeSpeed) % patternMax;

		float tx = bullet_pic[picType][colorType].PATTERN_WIDTH * (frame % bullet_pic[picType][colorType].PATTERN_NUM_U);
		float ty = bullet_pic[picType][colorType].PATTERN_HIGHT * (frame / bullet_pic[picType][colorType].PATTERN_NUM_U);
		float tw = bullet_pic[picType][colorType].PATTERN_WIDTH;
		float th = bullet_pic[picType][colorType].PATTERN_HIGHT;

		// 発射時に保存した色のテクスチャを描画する
		DrawSpriteAnimation_Scroll(
			bullet[i].pos.x, bullet[i].pos.y,
			bullet[i].size.x, bullet[i].size.y,
			MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f),
			bullet[i].rotation,
			tx,
			ty,
			tw,
			th,
			BulletTextureId[picType][colorType],
			true
		);
	}
}

// =========================================================
// 弾終了処理
// =========================================================
void FinalizeBullet(void)
{
	for (int i = 0; i < BULLET_PIC_NUM; i++)
	{
		for (int j = 0; j < BULLET_COLOR_NUM; j++)
		{
			UnloadTexture(BulletTextureId[i][j]);
		}
	}
}

// =========================================================
// 弾生成
// =========================================================
void SetBullet(Float2 pos, Float2 dir, float scale, int picType, int colorType)
{
	for (int i = 0; i < MAX_BULLET; i++)
	{
		if (bullet[i].use) continue;

		if (picType < 0 || picType >= BULLET_PIC_NUM)
		{
			picType = BULLET_PIC_NORMAL;
		}

		if (colorType < 0 || colorType >= BULLET_COLOR_NUM)
		{
			colorType = BULLET_COLOR_RED;
		}

		if (scale < 0.0f)
		{
			scale = 0.0f;
		}

		float dirLength = sqrtf(dir.x * dir.x + dir.y * dir.y);

		if (dirLength <= 0.0001f)
		{
			dir = MakeFloat2(1.0f, 0.0f);
		}
		else
		{
			dir.x /= dirLength;
			dir.y /= dirLength;
		}

		bullet[i].pos = pos;
		bullet[i].dir = dir;

		bullet[i].vel = MakeFloat2(
			dir.x * BULLET_SPEED,
			dir.y * BULLET_SPEED
		);

		float frameWidth = bullet_pic[picType][colorType]. PIC_SIZE.x / bullet_pic[picType][colorType].PATTERN_NUM_U;
		float frameHeight = bullet_pic[picType][colorType].PIC_SIZE.y / bullet_pic[picType][colorType].PATTERN_NUM_V;

		bullet[i].size = MakeFloat2(frameWidth * scale, frameHeight * scale);
		bullet[i].rotation = atan2f(dir.y, dir.x);

		bullet[i].scale = scale;

		// Bulletの種類とScaleからダメージを保存する
		if (picType == BULLET_PIC_CHARGE)
		{
			float chargeRate = (scale - CHARGE_MIN_SCALE) / (CHARGE_MAX_SCALE - CHARGE_MIN_SCALE);

			if (chargeRate < 0.0f)
			{
				chargeRate = 0.0f;
			}
			else if (chargeRate > 1.0f)
			{
				chargeRate = 1.0f;
			}

			bullet[i].damage = LerpFloat(
				BULLET_CHARGE_MIN_DAMAGE,
				BULLET_CHARGE_MAX_DAMAGE,
				chargeRate
			);
		}
		else
		{
			bullet[i].damage = BULLET_NORMAL_DAMAGE;
		}

		bullet[i].picType = picType;

		// プレイヤーが発射した瞬間の色を保存する
		bullet[i].colorType = colorType;

		bullet[i].animeFrame = 0;
		bullet[i].lifeFrame = 0;
		bullet[i].use = true;

		// 生成直後の弾頭Colliderを計算する
		UpdateBulletCollision(&bullet[i]);

		return;
	}
}

// =========================================================
// 弾頭Collider更新
// =========================================================
void UpdateBulletCollision(BULLET * targetBullet)
{
	float collisionSize =  targetBullet->size.y * BULLET_HEAD_COLLISION_RATE * 0.7f;
	if (collisionSize < 1.0f)
	{
		collisionSize = 1.0f;
	}
	
	// 弾の中心から弾頭Collider中心までの距離
	float headOffset = targetBullet->size.x * 0.5f - collisionSize * 0.5f;

	targetBullet->CollisionPosition = MakeFloat2(
		targetBullet->pos.x + targetBullet->dir.x * headOffset,
		targetBullet->pos.y + targetBullet->dir.y * headOffset
	);

	targetBullet->CollisionSize = MakeFloat2(
		collisionSize,
		collisionSize
	);
}

// =========================================================
// 弾とブロックの当たり判定
// =========================================================
bool BulletBlockCollision(BULLET* targetBullet)
{
	BLOCK* block = GetBlock();
	int blockCount = GetBlockCount();
	for (int i = 0; i < blockCount; i++)
	{
		if (!block[i].use) continue;

		if (CheckBoxCollider(
			targetBullet->CollisionPosition, block[i].CollisionPosition,
			targetBullet->CollisionSize, block[i].CollisionSize
		))
		{
			return true;
		}
	}

	COLOR_CHANGE_BLOCK* ccBlock = GetColorChangeBlock();
	for (int i = 0; i < CCBLOCK_MAX; i++)
	{
		if (!ccBlock[i].use) continue;

		if (CheckBoxCollider(
			targetBullet->CollisionPosition, ccBlock[i].CollisionPos,
			targetBullet->CollisionSize, ccBlock[i].CollisionSize
		))
		{
			return true;
		}
	}

	return false;
}

// =========================================================
// 弾のScaleからヒットエフェクトサイズを取得
// =========================================================
Float2 GetBulletHitEffectSize(const BULLET* targetBullet)
{
	int picType = targetBullet->picType;
	int colorType =targetBullet->colorType;

	if (picType < 0 || picType >= BULLET_PIC_NUM)
	{
		picType = BULLET_PIC_NORMAL;
	}

	if (colorType < 0 || colorType >= BULLET_COLOR_NUM)
	{
		colorType = BULLET_COLOR_RED;
	}

	float frameWidth  = hit_pic[picType][colorType].PIC_SIZE.x / hit_pic[picType][colorType].PATTERN_NUM_U;
	float frameHeight = hit_pic[picType][colorType].PIC_SIZE.y / hit_pic[picType][colorType].PATTERN_NUM_V;
	float effectScale = targetBullet->scale;
	if (picType == BULLET_PIC_TYPE::BULLET_PIC_CHARGE) effectScale *= CHARGE_BULLET_SCALE_RATE;

	return MakeFloat2(frameWidth * effectScale, frameHeight * effectScale);
}

// =========================================================
// 弾配列取得
// =========================================================
BULLET* GetBullet(void)
{
	return &bullet[0];
}

// =========================================================
// 弾リセット
// =========================================================
void ResetBullet()
{
	for (int i = 0; i < MAX_BULLET; i++)
	{
		bullet[i] = BULLET{};
	}
}