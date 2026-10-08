// =========================================================
// enemy_bullet.cpp 敵弾制御
// =========================================================
#include "enemy_bullet.h"
#include "bullet.h"
#include "sprite.h"
#include "texture.h"
#include "collision.h"
#include "block.h"
#include "color_change_block.h"
#include "player.h"
#include <cmath>

// =========================================================
// マクロ定義
// =========================================================
#define BULLET_FADE_START_FRAME             (10)
#define BULLET_DESTROY_FADE_FRAME           (10) //壊されるのフェード
#define BULLET_COLLISION_SIZE_OFFSET_RATE   (0.7f)

// =========================================================
// 敵弾各データ
// =========================================================
const BULLET_DATA enemy_bullet_data[BELONGS_MAX]
{
// Speed, Life, 回転?, 追尾?, p貫通? b貫通? 壊せる? 何発いる？
    { 6.0f, 180, false, true, true, true, true, 3}, // 遠隔キノコ
};

const Pic_Data_Enemy_Bullet enemy_bullet_pic[BELONGS_MAX]
{
    { "rom:/Mush_Range_Bullet.tga", MakeFloat2(2000.0f, 250.0f), 8, 8, 1, 10 },
};

const Pic_Data_Enemy_Bullet bullet_aimed_effect{ "rom:/Aimed_Effect.tga", MakeFloat2(2400.0f, 200.0f), 12, 12, 1, 4 };

// =========================================================
// グローバル変数
// =========================================================
ENEMY_BULLET enemyBullet[MAX_ENEMY_BULLET]{};
unsigned int EnemyBulletTextureId[BELONGS_MAX]{};

// AimedEffect
unsigned int EnemyBulletAimedEffectTextureId{};
int enemyBulletAimedEffectFrame{};

// =========================================================
// プロトタイプ宣言
// =========================================================
bool EnemyBulletBlockCollision(const ENEMY_BULLET* targetBullet);
bool EnemyBulletPlayerCollision(const ENEMY_BULLET* targetBullet);
void DrawEnemyBulletAimedEffect(const ENEMY_BULLET* targetBullet);

// =========================================================
// 敵弾初期化
// =========================================================
void InitializeEnemyBullet()
{
    for (int i = 0; i < MAX_ENEMY_BULLET; i++)
    {
        enemyBullet[i] = ENEMY_BULLET{};
    }

    for (int i = 0; i < BELONGS_MAX; i++)
    {
        EnemyBulletTextureId[i] = LoadTexture(enemy_bullet_pic[i].FILE_NAME);
    }

    EnemyBulletAimedEffectTextureId = LoadTexture(bullet_aimed_effect.FILE_NAME);
    enemyBulletAimedEffectFrame = 0;
}

// =========================================================
// 敵弾更新
// =========================================================
void UpdateEnemyBullet()
{
    for (int i = 0; i < MAX_ENEMY_BULLET; i++)
    {
        ENEMY_BULLET* targetBullet = &enemyBullet[i];
        if (!targetBullet->use) continue;
        const BULLET_DATA& bulletData = enemy_bullet_data[targetBullet->myMaster];

        // 破壊フェード中は移動・Collisionを停止して透明度だけ変化させる
        if (targetBullet->isDestroying)
        {
            targetBullet->destroyFrame++;
            targetBullet->color.w = MoveTowardsFloat(targetBullet->color.w, 0.0f, 1.0f / static_cast<float>(BULLET_DESTROY_FADE_FRAME));

            if (targetBullet->destroyFrame >= BULLET_DESTROY_FADE_FRAME)
            {
                *targetBullet = ENEMY_BULLET{};
            }

            continue;
        }

        // プレイヤーを追尾する
        if (targetBullet->isFollowPlayer)
        {
            PLAYER* player = GetPlayer();

            Float2 toPlayer = MakeFloat2(
                player->pos.x - targetBullet->pos.x,
                player->pos.y - targetBullet->pos.y
            );

            float length = sqrtf(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);
            if (length > 0.0001f)
            {
                targetBullet->dir = MakeFloat2(
                    toPlayer.x / length,
                    toPlayer.y / length
                );

                if (length <= bulletData.bulletSpeed)
                {   // 残り距離が速度以下の場合はPlayer位置まで移動する
                    targetBullet->vel = toPlayer;
                }
                else
                {   // 通常時は一定速度で追尾する
                    targetBullet->vel = MakeFloat2(
                        targetBullet->dir.x * bulletData.bulletSpeed,
                        targetBullet->dir.y * bulletData.bulletSpeed
                    );
                }

                if (bulletData.isNeedRotation)
                {
                    targetBullet->rotation = atan2f(targetBullet->dir.y, targetBullet->dir.x);
                }
            }
        }

        // 現在の速度で移動する
        targetBullet->pos.x += targetBullet->vel.x;
        targetBullet->pos.y += targetBullet->vel.y;

        // Collision更新
        targetBullet->CollisionPosition = targetBullet->pos;

        // アニメーション・生存時間更新
        targetBullet->animeFrame++;
        targetBullet->lifeFrame++;

        // 生存時間終了前のFade
        if (targetBullet->lifeFrame >= bulletData.lifeFrame - BULLET_FADE_START_FRAME)
        {
            targetBullet->color.w = MoveTowardsFloat(targetBullet->color.w, 0.0f, 1.0f / static_cast<float>(BULLET_FADE_START_FRAME));
        }

        // 生存時間終了
        if (targetBullet->lifeFrame >= bulletData.lifeFrame)
        {
            *targetBullet = ENEMY_BULLET{};
            continue;
        }

        // 貫通しない弾だけ削除する
        if (EnemyBulletBlockCollision(targetBullet))
        {
            if (!targetBullet->isGoThroughBlock)
            {
                *targetBullet = ENEMY_BULLET{};
                continue;
            }
        }

        // Playerとの当たり判定
        if (EnemyBulletPlayerCollision(targetBullet))
        {
            SetPlayerHit(targetBullet->CollisionPosition, targetBullet->damage);

            // 貫通しない弾だけ削除する
            if (!targetBullet->isGoThroughPlayer) *targetBullet = ENEMY_BULLET{};
            continue;
        }
    }
}

// =========================================================
// 敵弾描画
// =========================================================
void DrawEnemyBullet()
{
    ENEMY_BULLET* aimedBullet{};

    for (int i = 0; i < MAX_ENEMY_BULLET; i++)
    {
        if (!enemyBullet[i].use) continue;

        BULLET_BELONGS_TO bulletMaster = enemyBullet[i].myMaster;
        const BULLET_DATA& bulletData = enemy_bullet_data[bulletMaster];

        int animeSpeed = enemy_bullet_pic[bulletMaster].ANIME_SPEED;
        if (animeSpeed <= 0) animeSpeed = 1;

        int patternMax = enemy_bullet_pic[bulletMaster].PATTERN_MAX;
        if (patternMax <= 0) patternMax = 1;

        int patternNumU = enemy_bullet_pic[bulletMaster].PATTERN_NUM_U;
        if (patternNumU <= 0) patternNumU = 1;

        int frame = (enemyBullet[i].animeFrame / animeSpeed) % patternMax;

        float tx = enemy_bullet_pic[bulletMaster].PATTERN_WIDTH * (frame % patternNumU);
        float ty = enemy_bullet_pic[bulletMaster].PATTERN_HIGHT * (frame / patternNumU);
        float tw = enemy_bullet_pic[bulletMaster].PATTERN_WIDTH;
        float th = enemy_bullet_pic[bulletMaster].PATTERN_HIGHT;

        DrawSpriteAnimation_Scroll(
            enemyBullet[i].pos.x, enemyBullet[i].pos.y,
            enemyBullet[i].size.x, enemyBullet[i].size.y,
            enemyBullet[i].color, enemyBullet[i].rotation,
            tx, ty, tw, th,
            EnemyBulletTextureId[bulletMaster],
            false
        );

        // Aimed Effect Draw
        if (enemyBullet[i].isBeingAimed && !enemyBullet[i].isDestroying)
        {
            aimedBullet = &enemyBullet[i];
        }

        if (aimedBullet != nullptr)
        {
            enemyBulletAimedEffectFrame++;
            DrawEnemyBulletAimedEffect(aimedBullet);
        }
        else
        {
            enemyBulletAimedEffectFrame = 0;
        }

    }
}

// =========================================================
// 敵弾終了処理
// =========================================================
void FinalizeEnemyBullet()
{
    for (int i = 0; i < BELONGS_MAX; i++)
    {
        if (EnemyBulletTextureId[i] != 0)
        {
            UnloadTexture(EnemyBulletTextureId[i]);
            EnemyBulletTextureId[i] = 0;
        }
    }

    for (int i = 0; i < MAX_ENEMY_BULLET; i++)
    {
        enemyBullet[i] = ENEMY_BULLET{};
    }

    if (EnemyBulletAimedEffectTextureId != 0)
    {
        UnloadTexture(EnemyBulletAimedEffectTextureId);
        EnemyBulletAimedEffectTextureId = 0;
    }

    enemyBulletAimedEffectFrame = 0;
}

// =========================================================
// 敵弾生成
// =========================================================
void SetEnemyBullet(Float2 pos, Float2 targetPos, float damage, BULLET_BELONGS_TO whoseBullet)
{
    if (whoseBullet < 0 || whoseBullet >= BELONGS_MAX) return;

    Float2 dir = MakeFloat2(
        targetPos.x - pos.x,
        targetPos.y - pos.y
    );

    float length = sqrtf(dir.x * dir.x + dir.y * dir.y);
    if (length <= 0.0001f) return;

    dir.x /= length;
    dir.y /= length;

    const Pic_Data_Enemy_Bullet& picData = enemy_bullet_pic[whoseBullet];
    const BULLET_DATA& bulletData = enemy_bullet_data[whoseBullet];

    int patternNumU = picData.PATTERN_NUM_U;
    if (patternNumU <= 0) patternNumU = 1;

    int patternNumV = picData.PATTERN_NUM_V;
    if (patternNumV <= 0) patternNumV = 1;

    float width = picData.PIC_SIZE.x / patternNumU;
    float height = picData.PIC_SIZE.y / patternNumV;

    for (int i = 0; i < MAX_ENEMY_BULLET; i++)
    {
        if (enemyBullet[i].use) continue;
        enemyBullet[i] = ENEMY_BULLET{};
        enemyBullet[i].pos = pos;
        enemyBullet[i].dir = dir;
        enemyBullet[i].vel = MakeFloat2(
            dir.x * bulletData.bulletSpeed,
            dir.y * bulletData.bulletSpeed
        );

        enemyBullet[i].size = MakeFloat2(width, height);
        enemyBullet[i].CollisionPosition = pos;
        enemyBullet[i].CollisionSize = MakeFloat2(
            enemyBullet[i].size.x * BULLET_COLLISION_SIZE_OFFSET_RATE,
            enemyBullet[i].size.y * BULLET_COLLISION_SIZE_OFFSET_RATE
        );
        enemyBullet[i].color = MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f);
        enemyBullet[i].rotation = bulletData.isNeedRotation ? atan2f(dir.y, dir.x) : 0.0f;
        enemyBullet[i].damage = damage;
        enemyBullet[i].animeFrame = 0;
        enemyBullet[i].lifeFrame = 0;
        enemyBullet[i].myMaster = whoseBullet;
        enemyBullet[i].isFollowPlayer = bulletData.isFollowPlayer;
        enemyBullet[i].isGoThroughPlayer = bulletData.isGoThroughPlayer;
        enemyBullet[i].isGoThroughBlock = bulletData.isGoThroughBlock;
        enemyBullet[i].canBeDestroyed = bulletData.canBeDestroyed;
        enemyBullet[i].life = bulletData.needHowManyHitToBeDestroyed;
        enemyBullet[i].destroyFrame = 0;
        enemyBullet[i].isBeingAimed = false;
        enemyBullet[i].isDestroying = false;
        enemyBullet[i].use = true;
        return;
    }
}

// =========================================================
// 敵弾破壊開始
// =========================================================
void StartDestroyEnemyBullet(ENEMY_BULLET* targetBullet)
{
    if (targetBullet == nullptr) return;
    if (!targetBullet->use) return;
    if (targetBullet->isDestroying) return;

    targetBullet->isDestroying = true;
    targetBullet->isBeingAimed = false;
    targetBullet->isFollowPlayer = false;
    targetBullet->destroyFrame = 0;
    targetBullet->vel = MakeFloat2(0.0f, 0.0f);

    // Fade中は当たり判定を無効にする
    targetBullet->CollisionPosition = MakeFloat2(0.0f, 0.0f);
    targetBullet->CollisionSize = MakeFloat2(0.0f, 0.0f);
}

// =========================================================
// Player BulletとEnemy Bulletの当たり判定
// =========================================================
bool PlayerBulletEnemyBulletCollision(Float2 bulletCollisionPosition, Float2 bulletCollisionSize, int bulletType)
{
    for (int i = 0; i < MAX_ENEMY_BULLET; i++)
    {
        ENEMY_BULLET* targetBullet = &enemyBullet[i];

        if (!targetBullet->use) continue;
        if (targetBullet->isDestroying) continue;
        if (!targetBullet->canBeDestroyed) continue;

        if (!CheckBoxCollider(
            bulletCollisionPosition, targetBullet->CollisionPosition,
            bulletCollisionSize, targetBullet->CollisionSize))
        {
            continue;
        }

        if (bulletType == BULLET_PIC_CHARGE)
        {
            // Charge Bulletは残りLifeに関係なく破壊
            targetBullet->life = 0;
            StartDestroyEnemyBullet(targetBullet);
        }
        else
        {
            // Normal Bulletは1発ごとにLifeを減らす
            targetBullet->life--;

            if (targetBullet->life <= 0)
            {
                targetBullet->life = 0;
                StartDestroyEnemyBullet(targetBullet);
            }
        }

        return true;
    }

    return false;
}

// =========================================================
// 敵弾とBlockの当たり判定
// =========================================================
bool EnemyBulletBlockCollision(const ENEMY_BULLET* targetBullet)
{
    BLOCK* block = GetBlock();
    int blockCount = GetBlockCount();
    for (int i = 0; i < blockCount; i++)
    {
        if (!block[i].use) continue;

        if (CheckBoxCollider(
            targetBullet->CollisionPosition, block[i].CollisionPosition,
            targetBullet->CollisionSize, block[i].CollisionSize))
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
            targetBullet->CollisionSize, ccBlock[i].CollisionSize))
        {
            return true;
        }
    }

    return false;
}

// =========================================================
// 敵弾とPlayerの当たり判定
// =========================================================
bool EnemyBulletPlayerCollision(const ENEMY_BULLET* targetBullet)
{
    PLAYER* player = GetPlayer();

    if (!player->use) return false;
    if (player->state == PLAYER_STATE_DEAD) return false;

    return CheckBoxCollider(
        targetBullet->CollisionPosition, player->CollisionPosition,
        targetBullet->CollisionSize, player->CollisionSize
    );
}

// =========================================================
// Enemy Bullet Aimed Effect描画
// =========================================================
void DrawEnemyBulletAimedEffect(const ENEMY_BULLET* targetBullet)
{
    if (targetBullet == nullptr) return;

    int animeSpeed =bullet_aimed_effect.ANIME_SPEED;
    if (animeSpeed <= 0) animeSpeed = 1;
    int patternMax = bullet_aimed_effect.PATTERN_MAX;
    if (patternMax <= 0) patternMax = 1;
    int patternNumU = bullet_aimed_effect.PATTERN_NUM_U;
    if (patternNumU <= 0) patternNumU = 1;
    int patternNumV = bullet_aimed_effect.PATTERN_NUM_V;
    if (patternNumV <= 0) patternNumV = 1;
    int frame = (enemyBulletAimedEffectFrame / animeSpeed) % patternMax;

    float tx = bullet_aimed_effect.PATTERN_WIDTH * (frame % patternNumU);
    float ty = bullet_aimed_effect.PATTERN_HIGHT * (frame / patternNumU);
    float effectWidth = bullet_aimed_effect.PIC_SIZE.x / patternNumU;
    float effectHeight =  bullet_aimed_effect.PIC_SIZE.y / patternNumV;
    float scaleX = targetBullet->size.x / effectWidth;
    float scaleY = targetBullet->size.y / effectHeight;
    float scale = scaleX > scaleY ? scaleX : scaleY;

    DrawSpriteAnimation_Scroll(
        targetBullet->pos.x, targetBullet->pos.y,
        effectWidth * scale, effectHeight * scale,
        MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f,
        tx,ty,
        bullet_aimed_effect.PATTERN_WIDTH,
        bullet_aimed_effect.PATTERN_HIGHT,
        EnemyBulletAimedEffectTextureId,
        true
    );
}

// =========================================================
// Enemy Bullet取得
// =========================================================
ENEMY_BULLET* GetEnemyBullet()
{
    return &enemyBullet[0];
}

// =========================================================
// Enemy Bullet リセット
// =========================================================
void ResetEnemyBullet()
{
    for (int i = 0; i < MAX_ENEMY_BULLET; i++)
    {
        enemyBullet[i] = ENEMY_BULLET{};
    }
}