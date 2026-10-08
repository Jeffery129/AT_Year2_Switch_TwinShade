// =========================================================
// enemy.cpp 敵管理
//
// 制作者:        日付：
// =========================================================
#include "enemy.h"
#include "enemy_bullet.h"
#include "texture.h"
#include "sprite.h"
#include "collision.h"
#include "block.h"
#include "color_change_block.h"
#include "player.h"

// =========================================================
// マクロ定義
// =========================================================
#define ENEMY_IDLE_MIN_SECOND              (1)
#define ENEMY_IDLE_MAX_SECOND              (2)
#define ENEMY_RUN_MIN_SECOND               (2)
#define ENEMY_RUN_MAX_SECOND               (4)
#define ENEMY_LOST_PLAYER_SECOND           (0.5f)
#define ENEMY_LOST_PLAYER_FRAME            (ENEMY_LOST_PLAYER_SECOND * 60)
#define ENEMY_ALERT_FRAME                  (15)
#define ALERT_EFFECT_MAX                   (50)
#define ALERT_EFFECT_SCALE_RATIO           (1.0f)
#define AIMED_EFFECT_SCALE_RATIO           (1.1f)

#define TUTORIAL_MELEE_SCALE               (1.2f)
#define TUTORIAL_RANGE_SCALE               (1.0f)

#pragma region MUSHROOM_MELEE_STATUS

#define MUSHROOM_MELEE_SCALE               (1.0f)
#define MUSHROOM_MELEE_COLLISION_SIZE_RATE (0.7f)
#define MUSHROOM_MELEE_HP                  (100.0f)
#define MUSHROOM_MELEE_ATK                 (20.0f)
#define MUSHROOM_MELEE_SEARCH_RANGE        (MAP_BLOCK_WIDTH * 5)
#define MUSHROOM_MELEE_PATROL_SPEED        (4.0f)
#define MUSHROOM_MELEE_CHASE_SPEED         (8.0f)
#define MUSHROOM_MELEE_GRAVITY             (9.8f / 60.0f * 4.0f)
#define MUSHROOM_MELEE_ATTACK_RANGE        (180.0f * MUSHROOM_MELEE_SCALE)
#define MUSHROOM_MELEE_ATTACK_COOL_FRAME   (30)   
#define MUSHROOM_MELEE_KNOCKBACK_POWER     (3.0f)

#pragma endregion

#pragma region MUSHROOM_RANGE_STATUS

#define MUSHROOM_RANGE_SCALE               (1.0f)
#define MUSHROOM_RANGE_COLLISION_SIZE_RATE (0.7f)
#define MUSHROOM_RANGE_HP                  (80.0f)
#define MUSHROOM_RANGE_ATK                 (15.0f)
#define MUSHROOM_RANGE_SEARCH_RANGE        (MAP_BLOCK_WIDTH * 8)
#define MUSHROOM_RANGE_PATROL_SPEED        (4.0f)
#define MUSHROOM_RANGE_CHASE_SPEED         (6.0f)
#define MUSHROOM_RANGE_GRAVITY             (9.8f / 60.0f * 4.0f)
#define MUSHROOM_RANGE_ATTACK_RANGE        (MAP_BLOCK_WIDTH * 8)
#define MUSHROOM_RANGE_ATTACK_COOL_FRAME   (120)   
#define MUSHROOM_RANGE_KNOCKBACK_POWER     (6.0f)

#pragma endregion

// =========================================================
// 敵画像データ
// =========================================================
const Pic_Data_Enemy mushroom_melee_pic[MUSHROOM_MELEE_PIC_NUM]
{
    { "rom:/Mush_Melee_Idle.tga",   MakeFloat2(1280.0f, 320.0f), 4 , 4 , 1, 10 },
    { "rom:/Mush_Melee_Run.tga",    MakeFloat2(2880.0f, 320.0f), 9 , 9 , 1, 6 },
    { "rom:/Mush_Melee_Attack.tga", MakeFloat2(3200.0f, 320.0f), 10, 10, 1, 7 },
    { "rom:/Mush_Melee_Hit.tga",    MakeFloat2(1280.0f, 320.0f), 4 , 4 , 1, 6 },
    { "rom:/Mush_Melee_Dead.tga",   MakeFloat2(2560.0f, 320.0f), 8 , 8 , 1, 10 },
    { "rom:/Mush_Melee_Idle.tga",   MakeFloat2(1280.0f, 320.0f), 4 , 4 , 1, 10 },   // ALERT状態はIDLEのpicを使用する
};

const Pic_Data_Enemy mushroom_range_pic[MUSHROOM_RANGE_PIC_NUM]
{
    { "rom:/Mush_Range_Idle.tga",   MakeFloat2(1280.0f, 320.0f), 4 , 4 , 1, 10 },
    { "rom:/Mush_Range_Run.tga",    MakeFloat2(2880.0f, 320.0f), 9 , 9 , 1, 6 },
    { "rom:/Mush_Range_Attack.tga", MakeFloat2(5760.0f, 320.0f), 18, 18, 1, 7 },
    { "rom:/Mush_Range_Hit.tga",    MakeFloat2(1280.0f, 320.0f), 4 , 4 , 1, 6 },
    { "rom:/Mush_Range_Dead.tga",   MakeFloat2(2560.0f, 320.0f), 8 , 8 , 1, 10 },
    { "rom:/Mush_Range_Idle.tga",   MakeFloat2(1280.0f, 320.0f), 4 , 4 , 1, 10 },   // ALERT状態はIDLEのpicを使用する
};

const Pic_Data_Enemy aimed_effect { "rom:/Aimed_Effect.tga", MakeFloat2(2400.0f, 200.0f), 12, 12, 1, 4 };
const Pic_Data_Enemy alert_effect { "rom:/Alert_Effect.tga", MakeFloat2(1500.0f, 150.0f), 10, 10, 1, 3 };

// =========================================================
// 敵配置データ
// 各個体で違う項目だけ入力する
// =========================================================
struct ENEMY_SPAWN_DATA
{
    GAME_STAGE myStage;
    Float2 pos;
    float patrolLeft;
    float patrolRight;
    float groundY;
};

// =========================================================
// グローバル変数
// =========================================================
// 横5行目、縦8と9の真ん中は中心
Float2 CENTER_OFFSET = MakeFloat2(
    MAP_BLOCK_WIDTH * 8.5f,
    MAP_BLOCK_HEIGHT * 5.0f
);
#pragma region MUSHROOM_MELEE

Mushroom_Melee mushroomMelee[MAX_ENEMY]{};
unsigned int MushroomMeleeTextureId[MUSHROOM_MELEE_PIC_NUM]{};
const ENEMY_SPAWN_DATA mushroomMeleeSpawnData[]
{
    {   //チュートリアルのデカいキノコくん
        GAME_STAGE_T_03,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 20.5f - CENTER_OFFSET.x,
            MAP_BLOCK_HEIGHT * 11.5f - CENTER_OFFSET.y - (320.0f * MUSHROOM_MELEE_SCALE * TUTORIAL_MELEE_SCALE) / 2
        ),
        MAP_BLOCK_WIDTH * 11.0f - CENTER_OFFSET.x,
        MAP_BLOCK_WIDTH * 30.0f - CENTER_OFFSET.x,
        MAP_BLOCK_HEIGHT * 11.5f - CENTER_OFFSET.y - (320.0f * MUSHROOM_MELEE_SCALE * TUTORIAL_MELEE_SCALE) / 2
    },
    // 追加する場合はここへ入力
    {
        GAME_STAGE_S_01,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 27.0f - CENTER_OFFSET.x,
            MAP_BLOCK_HEIGHT * 88.5f - CENTER_OFFSET.y - (320.0f * MUSHROOM_MELEE_SCALE) / 2
        ),
        MAP_BLOCK_WIDTH * 25.5f - CENTER_OFFSET.x,
        MAP_BLOCK_WIDTH * 29.5f - CENTER_OFFSET.x,
        MAP_BLOCK_HEIGHT * 88.5f - CENTER_OFFSET.y - (320.0f * MUSHROOM_MELEE_SCALE) / 2
    },
    {
        GAME_STAGE_S_01,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 80.0f - CENTER_OFFSET.x,
            MAP_BLOCK_HEIGHT * 32.5f - CENTER_OFFSET.y - (320.0f * MUSHROOM_MELEE_SCALE) / 2
        ),
        MAP_BLOCK_WIDTH * 78.5f - CENTER_OFFSET.x,
        MAP_BLOCK_WIDTH * 81.5f - CENTER_OFFSET.x,
        MAP_BLOCK_HEIGHT * 32.5f - CENTER_OFFSET.y - (320.0f * MUSHROOM_MELEE_SCALE) / 2
    },
    {
        GAME_STAGE_S_02,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 37.0f - CENTER_OFFSET.x,
            MAP_BLOCK_HEIGHT * 86.5f - CENTER_OFFSET.y - (320.0f * MUSHROOM_MELEE_SCALE) / 2
        ),
        MAP_BLOCK_WIDTH * 33.5f - CENTER_OFFSET.x,
        MAP_BLOCK_WIDTH * 37.5f - CENTER_OFFSET.x,
        MAP_BLOCK_HEIGHT * 86.5f - CENTER_OFFSET.y - (320.0f * MUSHROOM_MELEE_SCALE) / 2
    },
    {
        GAME_STAGE_S_02,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 44.0f - CENTER_OFFSET.x,
            MAP_BLOCK_HEIGHT * 46.5f - CENTER_OFFSET.y - (320.0f * MUSHROOM_MELEE_SCALE) / 2
        ),
        MAP_BLOCK_WIDTH * 41.5f - CENTER_OFFSET.x,
        MAP_BLOCK_WIDTH * 46.5f - CENTER_OFFSET.x,
        MAP_BLOCK_HEIGHT * 46.5f - CENTER_OFFSET.y - (320.0f * MUSHROOM_MELEE_SCALE) / 2
    },
};

#pragma endregion

#pragma region MUSHROOM_RANGE

Mushroom_Range mushroomRange[MAX_ENEMY]{};
unsigned int MushroomRangeTextureId[MUSHROOM_RANGE_PIC_NUM]{};
const ENEMY_SPAWN_DATA mushroomRangeSpawnData[]
{
    {   //チュートリアルのデカいキノコくん
        GAME_STAGE_T_03,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 42.0f - CENTER_OFFSET.x,
            MAP_BLOCK_HEIGHT * 5.5f - CENTER_OFFSET.y - (320.0f * MUSHROOM_RANGE_SCALE * TUTORIAL_RANGE_SCALE) / 2
        ),
        MAP_BLOCK_WIDTH * 42.0f - CENTER_OFFSET.x,
        MAP_BLOCK_WIDTH * 42.0f - CENTER_OFFSET.x,
        MAP_BLOCK_HEIGHT * 4.5f - CENTER_OFFSET.y - (320.0f * MUSHROOM_RANGE_SCALE * TUTORIAL_RANGE_SCALE) / 2
    },
    // 追加する場合はここへ入力
    {
        GAME_STAGE_S_01,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 17.5f - CENTER_OFFSET.x,
            MAP_BLOCK_HEIGHT * 90.5f - CENTER_OFFSET.y - (320.0f * MUSHROOM_RANGE_SCALE) / 2
        ),
        MAP_BLOCK_WIDTH * 16.5f - CENTER_OFFSET.x,
        MAP_BLOCK_WIDTH * 18.5f - CENTER_OFFSET.x,
        MAP_BLOCK_HEIGHT * 90.5f - CENTER_OFFSET.y - (320.0f * MUSHROOM_RANGE_SCALE) / 2
    },
    {
        GAME_STAGE_S_02,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 15.0f - CENTER_OFFSET.x,
            MAP_BLOCK_HEIGHT * 46.5f - CENTER_OFFSET.y - (320.0f * MUSHROOM_RANGE_SCALE) / 2
        ),
        MAP_BLOCK_WIDTH * 13.5f - CENTER_OFFSET.x,
        MAP_BLOCK_WIDTH * 17.5f - CENTER_OFFSET.x,
        MAP_BLOCK_HEIGHT * 46.5f - CENTER_OFFSET.y - (320.0f * MUSHROOM_RANGE_SCALE) / 2
    },
};

#pragma endregion

// Aimed_Effect
unsigned int AimedEffectTextureId{};
int aimedEffectFrame{};

// Alert_Effect
struct ALERT_EFFECT
{
    Enemy* targetEnemy;
    int frame;
    bool use;
};
ALERT_EFFECT alertEffect[ALERT_EFFECT_MAX]{};
unsigned int AlertEffectTextureId{};

// =========================================================
// プロトタイプ宣言
// =========================================================
// Mushroom Melee
void LoadMushroomMeleeTextures();
void SpawnMushroomMeleeCurrentStage();
void UpdateMushroomMelee(Mushroom_Melee* targetEnemy);
void DrawMushroomMelee(Mushroom_Melee* targetEnemy);
void FinalizeMushroomMelee();

// Mushroom Range
void LoadMushroomRangeTextures();
void SpawnMushroomRangeCurrentStage();
void UpdateMushroomRange(Mushroom_Range* targetEnemy);
void DrawMushroomRange(Mushroom_Range* targetEnemy);
void FinalizeMushroomRange();

// Common Enemy Methods
void SetEnemyState(Enemy* targetEnemy, ENEMY_STATE nextState);
void UpdateEnemySize(Enemy* targetEnemy, const Pic_Data_Enemy* picData);
void UpdateEnemyCollision(Enemy* targetEnemy, float targetSizeRate);
void UpdateEnemyPhysics(Enemy* targetEnemy);
bool CanEnemySeePlayer(const Enemy* targetEnemy, Float2 playerPos, Float2 playerCollisionSize);
Float2 GetNearestPoint(Float2 rayOrigin, const Float2* points, int pointNum);
bool IsPlayerInEnemyAttackRange(const Enemy* targetEnemy, Float2 playerPos);
int GetRandomFrameFromSecond(int minSecond, int maxSecond);

// Aimed_Effect
void DrawAimedEffect(Enemy* targetEnemy);

// Alert_Effect
void SetAlertEffect(Enemy* targetEnemy);
void UpdateAlertEffect();
void DrawAlertEffect();

// =========================================================
// 敵初期化
// =========================================================
void InitializeEnemy()
{
    LoadMushroomMeleeTextures();
    LoadMushroomRangeTextures();

    AimedEffectTextureId = LoadTexture(aimed_effect.FILE_NAME);
    AlertEffectTextureId = LoadTexture(alert_effect.FILE_NAME);

    ReloadEnemyStage();
}

// =========================================================
// 敵更新
// =========================================================
void UpdateEnemy()
{
    for (int i = 0; i < MAX_ENEMY; i++)
    {
        if (mushroomMelee[i].use)
        {
            UpdateMushroomMelee( &mushroomMelee[i]);
        }

        if (mushroomRange[i].use)
        {
            UpdateMushroomRange(&mushroomRange[i]);
        }
    }

    UpdateAlertEffect();
}

// =========================================================
// 敵描画
// =========================================================
void DrawEnemy()
{
    Enemy* targetEffectEnemy{};

    for (int i = 0; i < MAX_ENEMY; i++)
    {
        if (mushroomMelee[i].use)
        {
            DrawMushroomMelee(&mushroomMelee[i]);
            if (mushroomMelee[i].isBeingAimed) targetEffectEnemy = &mushroomMelee[i];
        }

        if (mushroomRange[i].use)
        {
            DrawMushroomRange(&mushroomRange[i]);
            if (mushroomRange[i].isBeingAimed)targetEffectEnemy = &mushroomRange[i];
        }
    }

    // Aimed_Effect
    if (targetEffectEnemy != nullptr)
    {
        aimedEffectFrame++;
        DrawAimedEffect(targetEffectEnemy);
    }
    else
    {
        aimedEffectFrame = 0;
    }

    // Alert_Effect
    DrawAlertEffect();
}

// =========================================================
// 敵終了処理
// =========================================================
void FinalizeEnemy()
{
    FinalizeMushroomMelee();
    FinalizeMushroomRange();

    if (AimedEffectTextureId != 0)
    {
        UnloadTexture(AimedEffectTextureId);
        AimedEffectTextureId = 0;
    }

    if (AlertEffectTextureId != 0)
    {
        UnloadTexture(AlertEffectTextureId);
        AlertEffectTextureId = 0;
    }

    aimedEffectFrame = 0;

    for (int i = 0; i < ALERT_EFFECT_MAX; i++)
    {
        alertEffect[i] = ALERT_EFFECT{};
    }
}

// =========================================================
// 各敵の関数たち
// =========================================================
#pragma region MUSHROOM_MELEE_METHODS

// =========================================================
// 近接キノコ初期化
// =========================================================
// Load Mushroom Melee textures
// =========================================================
void LoadMushroomMeleeTextures()
{
    for (int i = 0; i < MUSHROOM_MELEE_PIC_NUM; i++)
    {
        MushroomMeleeTextureId[i] =
            LoadTexture(mushroom_melee_pic[i].FILE_NAME);
    }
}

// =========================================================
// Spawn Mushroom Melee for current stage
// =========================================================
void SpawnMushroomMeleeCurrentStage()
{
    GAME_STAGE currentStage = GetCurrentGameStage();
    const int spawnDataCount = sizeof(mushroomMeleeSpawnData) / sizeof(mushroomMeleeSpawnData[0]);
    int enemyCnt = 0;

    for (int i = 0; i < spawnDataCount; i++)
    {
        const ENEMY_SPAWN_DATA& spawnData = mushroomMeleeSpawnData[i];
        if (spawnData.myStage != currentStage) continue;
        if (enemyCnt >= MAX_ENEMY) break;
        Mushroom_Melee* targetEnemy = &mushroomMelee[enemyCnt];

        targetEnemy->pos = spawnData.pos;
        targetEnemy->patrolLeft = spawnData.patrolLeft;
        targetEnemy->patrolRight = spawnData.patrolRight;
        targetEnemy->groundY = spawnData.groundY;

        targetEnemy->scale = MUSHROOM_MELEE_SCALE;
        targetEnemy->hp = MUSHROOM_MELEE_HP;
        targetEnemy->atk = MUSHROOM_MELEE_ATK;
        targetEnemy->searchRange = MUSHROOM_MELEE_SEARCH_RANGE;
        targetEnemy->patrolSpeed = MUSHROOM_MELEE_PATROL_SPEED;
        targetEnemy->chaseSpeed = MUSHROOM_MELEE_CHASE_SPEED;
        targetEnemy->gravityAcc = MUSHROOM_MELEE_GRAVITY;
        targetEnemy->attackRange = MUSHROOM_MELEE_ATTACK_RANGE;

        if (currentStage == GAME_STAGE_T_03)
        {
            targetEnemy->hp = MUSHROOM_MELEE_HP * 2.0f;
            targetEnemy->atk = 34.0f;
            targetEnemy->searchRange = MAP_BLOCK_HEIGHT * 10.0f;
            targetEnemy->attackRange *= TUTORIAL_MELEE_SCALE;
        }

        targetEnemy->color = MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f);
        targetEnemy->rotation = 0.0f;
        targetEnemy->vel = MakeFloat2(0.0f, 0.0f);
        targetEnemy->exVel = MakeFloat2(0.0f, 0.0f);

        targetEnemy->state = ENEMY_STATE_IDLE;
        targetEnemy->moveDir = 1;
        targetEnemy->isFacingRight = true;

        targetEnemy->stateTimer =
            GetRandomFrameFromSecond(
                ENEMY_IDLE_MIN_SECOND,
                ENEMY_IDLE_MAX_SECOND
            );

        targetEnemy->animeFrame = 0;

        targetEnemy->isChasing = false;
        targetEnemy->hasShownAlert = false;
        targetEnemy->lostPlayerTimer = 0;
        targetEnemy->lastPlayerPos = targetEnemy->pos;

        targetEnemy->isBeingAimed = false;

        targetEnemy->attackCollisionStartFrame = 5;
        targetEnemy->attackCoolTimer = 0;
        targetEnemy->hasHitPlayer = false;

        targetEnemy->use = true;

        UpdateEnemySize(targetEnemy, &mushroom_melee_pic[targetEnemy->state]);

        targetEnemy->CollisionOffset = MakeFloat2(0.0f, targetEnemy->size.y / 4.0f);
        UpdateEnemyCollision(targetEnemy, MUSHROOM_MELEE_COLLISION_SIZE_RATE);

        enemyCnt++;
    }
}

// =========================================================
// 近接キノコ更新
// =========================================================
void UpdateMushroomMelee(Mushroom_Melee* targetEnemy)
{
    if (!targetEnemy->use) return;
    if (targetEnemy->attackCoolTimer > 0) targetEnemy->attackCoolTimer--;

    Float2 playerPos = GetPlayer()->pos;
    Float2 playerCollisionSize = GetPlayer()->CollisionSize;
    bool canSeePlayer = false;

    // ALERT・HIT・DEAD中は索敵によるState変更を行わない
    if (targetEnemy->state != ENEMY_STATE_ALERT && targetEnemy->state != ENEMY_STATE_ATTACK &&
        targetEnemy->state != ENEMY_STATE_HIT &&targetEnemy->state != ENEMY_STATE_DEAD)
    {
        canSeePlayer = CanEnemySeePlayer(targetEnemy, playerPos, playerCollisionSize);

        if (canSeePlayer)
        {
            targetEnemy->lostPlayerTimer = static_cast<int>ENEMY_LOST_PLAYER_FRAME;
            targetEnemy->lastPlayerPos = playerPos;

            // 初めてPlayerを発見した場合
            if (!targetEnemy->isChasing)
            {
                targetEnemy->isChasing = true;

                // Playerの方向を向く
                if (playerPos.x < targetEnemy->pos.x)
                {
                    targetEnemy->moveDir = -1;
                    targetEnemy->isFacingRight = false;
                }
                else if (playerPos.x > targetEnemy->pos.x)
                {
                    targetEnemy->moveDir = 1;
                    targetEnemy->isFacingRight = true;
                }

                SetEnemyState(targetEnemy, ENEMY_STATE_ALERT);
            }
        }
        else if (targetEnemy->isChasing)
        {
            if (targetEnemy->lostPlayerTimer > 0) targetEnemy->lostPlayerTimer--;
            else
            {
                targetEnemy->isChasing = false;
                targetEnemy->hasShownAlert = false;

                SetEnemyState(
                    targetEnemy,
                    ENEMY_STATE_IDLE
                );
            }
        }
    }

    switch (targetEnemy->state)
    {
    case ENEMY_STATE_IDLE:
        targetEnemy->vel.x = 0.0f;
        targetEnemy->animeFrame++;
        if (targetEnemy->isChasing)
        {
            SetEnemyState(targetEnemy, ENEMY_STATE_ALERT);
            break;
        }
        if (targetEnemy->stateTimer > 0) targetEnemy->stateTimer--;
        else
        {
            targetEnemy->moveDir = (rand() % 2 == 0) ? -1 : 1;
            targetEnemy->isFacingRight = targetEnemy->moveDir > 0;
            SetEnemyState(targetEnemy, ENEMY_STATE_RUN);
        }
        break;

    case ENEMY_STATE_ALERT:
        targetEnemy->vel.x = 0.0f;
        targetEnemy->animeFrame++;

        if (targetEnemy->stateTimer > 0) targetEnemy->stateTimer--;
        else SetEnemyState(targetEnemy, ENEMY_STATE_RUN );

        break;

    case ENEMY_STATE_RUN:
        targetEnemy->animeFrame++;
        if (targetEnemy->isChasing)
        {
            Float2 chaseTarget = canSeePlayer ? playerPos : targetEnemy->lastPlayerPos;

            // 追跡目標をEnemyが移動できる範囲内に制限する
            float chaseTargetX = chaseTarget.x;

            if  (chaseTargetX < targetEnemy->patrolLeft) chaseTargetX = targetEnemy->patrolLeft;
            else if (chaseTargetX > targetEnemy->patrolRight) chaseTargetX = targetEnemy->patrolRight;

            // 実際のプレイヤー方向へ向きを合わせる
            if (chaseTarget.x < targetEnemy->pos.x)
            {
                targetEnemy->moveDir = -1;
                targetEnemy->isFacingRight = false;
            }
            else if (chaseTarget.x > targetEnemy->pos.x)
            {
                targetEnemy->moveDir = 1;
                targetEnemy->isFacingRight = true;
            }

            bool isPlayerInAttackRange = IsPlayerInEnemyAttackRange(targetEnemy, playerPos);
            float chaseDistanceX = fabsf(chaseTargetX - targetEnemy->pos.x);

            // 移動可能な追跡位置まで到達した場合は、その場で停止する
            if (chaseDistanceX <= 5.0f || isPlayerInAttackRange) targetEnemy->vel.x = 0.0f;
            else targetEnemy->vel.x = targetEnemy->chaseSpeed * static_cast<float>(targetEnemy->moveDir);

            if (canSeePlayer && isPlayerInAttackRange && targetEnemy->attackCoolTimer <= 0)
            {
                targetEnemy->hasHitPlayer = false;
                SetEnemyState(targetEnemy, ENEMY_STATE_ATTACK);
            }
        }
        else
        {
            int animeSpeed = mushroom_melee_pic[ENEMY_STATE_RUN].ANIME_SPEED;
            if (animeSpeed <= 0) animeSpeed = 1;
            int runPattern = targetEnemy->animeFrame / animeSpeed;
            targetEnemy->vel.x = targetEnemy->patrolSpeed * static_cast<float>(targetEnemy->moveDir);
            if (targetEnemy->stateTimer > 0)
            {
                targetEnemy->stateTimer--;
            }
            else if (runPattern >= mushroom_melee_pic[ENEMY_STATE_RUN].PATTERN_MAX)
            {
                SetEnemyState(targetEnemy, ENEMY_STATE_IDLE);
            }
        }
        break;

    case ENEMY_STATE_ATTACK:
        {
        targetEnemy->vel.x = 0.0f;
        targetEnemy->animeFrame++;
        int animeSpeed = mushroom_melee_pic[ENEMY_STATE_ATTACK].ANIME_SPEED;
        if (animeSpeed <= 0) animeSpeed = 1;
        int attackPattern = targetEnemy->animeFrame / animeSpeed;
        float playerDistanceX = fabsf(playerPos.x - targetEnemy->pos.x);
        float playerDistanceY = fabsf(playerPos.y - targetEnemy->pos.y);
        bool isPlayerInAttackRange = IsPlayerInEnemyAttackRange(targetEnemy, playerPos);

        // 前隙終了後、第5アニメーションフレームから攻撃判定を開始する
        if (attackPattern >= targetEnemy->attackCollisionStartFrame && attackPattern < mushroom_melee_pic[ENEMY_STATE_ATTACK].PATTERN_MAX)
        {
            if (isPlayerInAttackRange && !targetEnemy->hasHitPlayer)
            {
                // PlayerのHPをtargetEnemy->atk分減らす
                SetPlayerHit(targetEnemy->pos, targetEnemy->atk);
                targetEnemy->hasHitPlayer = true;
            }
        }

        if (attackPattern >= mushroom_melee_pic[ENEMY_STATE_ATTACK].PATTERN_MAX)
        {
            targetEnemy->attackCoolTimer = MUSHROOM_MELEE_ATTACK_COOL_FRAME;
            SetEnemyState(targetEnemy, targetEnemy->isChasing ? ENEMY_STATE_RUN : ENEMY_STATE_IDLE);
        }
        break;
        }

    case ENEMY_STATE_HIT:
        {
        targetEnemy->animeFrame++;
        int animeSpeed = mushroom_melee_pic[ENEMY_STATE_HIT].ANIME_SPEED;
        if (animeSpeed <= 0) animeSpeed = 1;
        int hitPattern = targetEnemy->animeFrame / animeSpeed;
        if (hitPattern >= mushroom_melee_pic[ENEMY_STATE_HIT].PATTERN_MAX)
        {
            SetEnemyState(targetEnemy, targetEnemy->isChasing ? ENEMY_STATE_ALERT : ENEMY_STATE_IDLE);
        }
        break;
        }

    case ENEMY_STATE_DEAD:
        {
        targetEnemy->vel.x = 0.0f;
        int animeSpeed = mushroom_melee_pic[ENEMY_STATE_DEAD].ANIME_SPEED;
        if (animeSpeed <= 0) animeSpeed = 1;

        int lastAnimeFrame = (mushroom_melee_pic[ENEMY_STATE_DEAD].PATTERN_MAX - 1) * animeSpeed;

        if (targetEnemy->animeFrame < lastAnimeFrame) targetEnemy->animeFrame++;
        else targetEnemy->animeFrame = lastAnimeFrame;

        break;
        }
    }

    UpdateEnemyPhysics(targetEnemy);
    UpdateEnemySize(targetEnemy, &mushroom_melee_pic[targetEnemy->state]);

    //------------------ チュートリアルの近接キノコくんをデカくする ---------------------
    float stageScale = 1.0f;
    if (GetCurrentGameStage() == GAME_STAGE_T_03) stageScale = TUTORIAL_MELEE_SCALE;
    targetEnemy->size = MakeFloat2(targetEnemy->size.x * stageScale, targetEnemy->size.y * stageScale);
    //-------------------------------------------------------------------------------

    UpdateEnemyCollision(targetEnemy, MUSHROOM_MELEE_COLLISION_SIZE_RATE);
}

// =========================================================
// 近接キノコ描画
// =========================================================
void DrawMushroomMelee(Mushroom_Melee* targetEnemy)
{
    if (!targetEnemy->use) return;
    int picId = targetEnemy->state;
    if (picId < 0 || picId >= MUSHROOM_MELEE_PIC_NUM)picId = ENEMY_STATE_IDLE;
    //---------------------------------------------------------------------
    // 追跡中に移動可能範囲の端へ到達して停止している場合はIDLE画像を使用する
    if (targetEnemy->state == ENEMY_STATE_RUN && targetEnemy->isChasing && fabsf(targetEnemy->vel.x) <= 0.0001f) picId = ENEMY_STATE_IDLE;
    //---------------------------------------------------------------------
    int animeSpeed = mushroom_melee_pic[picId].ANIME_SPEED;
    if (animeSpeed <= 0) animeSpeed = 1;
    int patternMax = mushroom_melee_pic[picId].PATTERN_MAX;
    if (patternMax <= 0) patternMax = 1;
    int patternNumU = mushroom_melee_pic[picId].PATTERN_NUM_U;
    if (patternNumU <= 0) patternNumU = 1;
    int frame = (targetEnemy->animeFrame / animeSpeed) % patternMax;
    float tx = mushroom_melee_pic[picId].PATTERN_WIDTH * (frame % patternNumU);
    float ty = mushroom_melee_pic[picId].PATTERN_HIGHT * (frame / patternNumU);
    float tw = mushroom_melee_pic[picId].PATTERN_WIDTH;
    float th = mushroom_melee_pic[picId].PATTERN_HIGHT;

    DrawSpriteAnimation_Scroll(
        targetEnemy->pos.x, targetEnemy->pos.y,
        targetEnemy->size.x, targetEnemy->size.y,
        targetEnemy->color, targetEnemy->rotation,
        tx, ty, tw, th,
        MushroomMeleeTextureId[picId],
        targetEnemy->isFacingRight
    );
}

// =========================================================
// 近接キノコ終了処理
// =========================================================
void FinalizeMushroomMelee()
{
    for (int i = 0; i < MUSHROOM_MELEE_PIC_NUM; i++)
    {
        if (MushroomMeleeTextureId[i] != 0)
        {
            UnloadTexture(MushroomMeleeTextureId[i]);
            MushroomMeleeTextureId[i] = 0;
        }
    }
    for (int i = 0; i < MAX_ENEMY; i++) mushroomMelee[i].use = false;
}

#pragma endregion

#pragma region MUSHROOM_MELEE_METHODS

// =========================================================
// 遠距離キノコ初期化
// =========================================================
// Load Mushroom Range textures
// =========================================================
void LoadMushroomRangeTextures()
{
    for (int i = 0; i < MUSHROOM_RANGE_PIC_NUM; i++)
    {
        MushroomRangeTextureId[i] =
            LoadTexture(mushroom_range_pic[i].FILE_NAME);
    }
}

// =========================================================
// Spawn Mushroom Range for current stage
// =========================================================
void SpawnMushroomRangeCurrentStage()
{
    GAME_STAGE currentStage = GetCurrentGameStage();
    const int spawnDataCount = sizeof(mushroomRangeSpawnData) / sizeof(mushroomRangeSpawnData[0]);
    int enemyCnt = 0;

    for (int i = 0; i < spawnDataCount; i++)
    {
        const ENEMY_SPAWN_DATA& spawnData = mushroomRangeSpawnData[i];
        if (spawnData.myStage != currentStage) continue;
        if (enemyCnt >= MAX_ENEMY) break;

        Mushroom_Range* targetEnemy = &mushroomRange[enemyCnt];

        targetEnemy->pos = spawnData.pos;
        targetEnemy->patrolLeft = spawnData.patrolLeft;
        targetEnemy->patrolRight = spawnData.patrolRight;
        targetEnemy->groundY = spawnData.groundY;

        targetEnemy->scale = MUSHROOM_RANGE_SCALE;
        targetEnemy->hp = MUSHROOM_RANGE_HP;
        targetEnemy->atk = MUSHROOM_RANGE_ATK;
        targetEnemy->searchRange = MUSHROOM_RANGE_SEARCH_RANGE;
        targetEnemy->patrolSpeed = MUSHROOM_RANGE_PATROL_SPEED;
        targetEnemy->chaseSpeed = MUSHROOM_RANGE_CHASE_SPEED;
        targetEnemy->gravityAcc = MUSHROOM_RANGE_GRAVITY;
        targetEnemy->attackRange = MUSHROOM_RANGE_ATTACK_RANGE;

        if (currentStage == GAME_STAGE_T_03)
        {
            targetEnemy->hp = MUSHROOM_RANGE_HP * 2.0f;
            targetEnemy->atk = 50.0f;
            targetEnemy->searchRange = MAP_BLOCK_WIDTH * 16;
            targetEnemy->attackRange = MAP_BLOCK_WIDTH * 16;
        }

        targetEnemy->color = MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f);
        targetEnemy->rotation = 0.0f;
        targetEnemy->vel = MakeFloat2(0.0f, 0.0f);
        targetEnemy->exVel = MakeFloat2(0.0f, 0.0f);

        targetEnemy->state = ENEMY_STATE_IDLE;
        targetEnemy->moveDir = 1;
        targetEnemy->isFacingRight = true;

        targetEnemy->stateTimer =
            GetRandomFrameFromSecond(
                ENEMY_IDLE_MIN_SECOND,
                ENEMY_IDLE_MAX_SECOND
            );

        targetEnemy->animeFrame = 0;

        targetEnemy->isChasing = false;
        targetEnemy->hasShownAlert = false;
        targetEnemy->lostPlayerTimer = 0;
        targetEnemy->lastPlayerPos = targetEnemy->pos;

        targetEnemy->isBeingAimed = false;

        targetEnemy->shootPattern = 9;
        targetEnemy->attackCoolTimer = 0;
        targetEnemy->hasShot = false;

        targetEnemy->use = true;

        UpdateEnemySize(targetEnemy, &mushroom_range_pic[ENEMY_STATE_IDLE]);
        targetEnemy->CollisionOffset = MakeFloat2(0.0f, targetEnemy->size.y / 4.0f);
        UpdateEnemyCollision(targetEnemy, MUSHROOM_RANGE_COLLISION_SIZE_RATE);

        enemyCnt++;
    }
}

void UpdateMushroomRange(Mushroom_Range* targetEnemy)
{
    if (!targetEnemy->use) return;
    if (targetEnemy->attackCoolTimer > 0) targetEnemy->attackCoolTimer--;

    Float2 playerPos = GetPlayer()->pos;
    Float2 playerCollisionSize = GetPlayer()->CollisionSize;
    bool canSeePlayer = false;

    // ALERT・ATTACK・HIT・DEAD中は索敵によるState変更を行わない
    if (targetEnemy->state != ENEMY_STATE_ATTACK && targetEnemy->state != ENEMY_STATE_ALERT &&
        targetEnemy->state != ENEMY_STATE_HIT && targetEnemy->state != ENEMY_STATE_DEAD)
    {
        canSeePlayer = CanEnemySeePlayer(targetEnemy, playerPos, playerCollisionSize);

        if (canSeePlayer)
        {
            targetEnemy->lostPlayerTimer = static_cast<int>ENEMY_LOST_PLAYER_FRAME;
            targetEnemy->lastPlayerPos = playerPos;

            // 初めてPlayerを発見した場合
            if (!targetEnemy->isChasing)
            {
                targetEnemy->isChasing = true;

                // Playerの方向を向く
                if (playerPos.x < targetEnemy->pos.x)
                {
                    targetEnemy->moveDir = -1;
                    targetEnemy->isFacingRight = false;
                }
                else if (playerPos.x > targetEnemy->pos.x)
                {
                    targetEnemy->moveDir = 1;
                    targetEnemy->isFacingRight = true;
                }

                SetEnemyState(targetEnemy, ENEMY_STATE_ALERT);
            }
        }
        else if (targetEnemy->isChasing)
        {
            if (targetEnemy->lostPlayerTimer > 0) targetEnemy->lostPlayerTimer--;
            else
            {
                targetEnemy->isChasing = false;
                targetEnemy->hasShownAlert = false;

                SetEnemyState(targetEnemy, ENEMY_STATE_IDLE);
            }
        }
    }

    switch (targetEnemy->state)
    {
    case ENEMY_STATE_IDLE:
        targetEnemy->vel.x = 0.0f;
        targetEnemy->animeFrame++;
        if (targetEnemy->isChasing)
        {
            SetEnemyState(targetEnemy, ENEMY_STATE_ALERT);
            break;
        }
        if (targetEnemy->stateTimer > 0) targetEnemy->stateTimer--;
        else
        {
            targetEnemy->moveDir = (rand() % 2 == 0) ? -1 : 1;
            targetEnemy->isFacingRight = targetEnemy->moveDir > 0;
            SetEnemyState(targetEnemy, ENEMY_STATE_RUN);
        }
        break;

    case ENEMY_STATE_ALERT:
        targetEnemy->vel.x = 0.0f;
        targetEnemy->animeFrame++;

        if (targetEnemy->stateTimer > 0) targetEnemy->stateTimer--;
        else SetEnemyState(targetEnemy, ENEMY_STATE_RUN);

        break;

    case ENEMY_STATE_RUN:
        targetEnemy->animeFrame++;
        if (targetEnemy->isChasing)
        {
            Float2 chaseTarget = canSeePlayer ? playerPos : targetEnemy->lastPlayerPos;

            // 追跡目標をEnemyが移動できる範囲内に制限する
            float chaseTargetX = chaseTarget.x;

            if (chaseTargetX < targetEnemy->patrolLeft) chaseTargetX = targetEnemy->patrolLeft;
            else if (chaseTargetX > targetEnemy->patrolRight) chaseTargetX = targetEnemy->patrolRight;

            // 実際のプレイヤー方向へ向きを合わせる
            if (chaseTarget.x < targetEnemy->pos.x)
            {
                targetEnemy->moveDir = -1;
                targetEnemy->isFacingRight = false;
            }
            else if (chaseTarget.x > targetEnemy->pos.x)
            {
                targetEnemy->moveDir = 1;
                targetEnemy->isFacingRight = true;
            }

            bool isPlayerInAttackRange = IsPlayerInEnemyAttackRange(targetEnemy, playerPos);
            float chaseDistanceX = fabsf(chaseTargetX - targetEnemy->pos.x);

            // 移動可能な追跡位置まで到達した場合は、その場で停止する
            if (chaseDistanceX <= 5.0f || isPlayerInAttackRange) targetEnemy->vel.x = 0.0f;
            else targetEnemy->vel.x = targetEnemy->chaseSpeed * static_cast<float>(targetEnemy->moveDir);

            if (canSeePlayer && isPlayerInAttackRange && targetEnemy->attackCoolTimer <= 0)
            {
                targetEnemy->hasShot = false;
                SetEnemyState(targetEnemy, ENEMY_STATE_ATTACK);
            }
        }
        else
        {
            int animeSpeed = mushroom_range_pic[ENEMY_STATE_RUN].ANIME_SPEED;
            if (animeSpeed <= 0) animeSpeed = 1;
            int runPattern = targetEnemy->animeFrame / animeSpeed;
            targetEnemy->vel.x = targetEnemy->patrolSpeed * static_cast<float>(targetEnemy->moveDir);
            if (targetEnemy->stateTimer > 0)
            {
                targetEnemy->stateTimer--;
            }
            else if (runPattern >= mushroom_range_pic[ENEMY_STATE_RUN].PATTERN_MAX)
            {
                SetEnemyState(targetEnemy, ENEMY_STATE_IDLE);
            }
        }
        break;

    case ENEMY_STATE_ATTACK:
        {
        targetEnemy->vel.x = 0.0f;
        targetEnemy->animeFrame++;

        int animeSpeed = mushroom_range_pic[ENEMY_STATE_ATTACK].ANIME_SPEED;
        if (animeSpeed <= 0) animeSpeed = 1;
        int attackPattern = targetEnemy->animeFrame / animeSpeed;

        // 第10个Patternで発射
        if (attackPattern >= targetEnemy->shootPattern && !targetEnemy->hasShot)
        {
            Float2 shootPos = targetEnemy->CollisionPosition;

            // Enemyの前方から発射
            shootPos.x += targetEnemy->isFacingRight ? targetEnemy->CollisionSize.x / 2.0f : -targetEnemy->CollisionSize.x / 2.0f;
            shootPos.y -= targetEnemy->CollisionSize.y / 2.0f;
            SetEnemyBullet(shootPos, playerPos, targetEnemy->atk, BULLET_BELONGS_TO::MUSH_RANGE);
            targetEnemy->hasShot = true;
        }

        if (attackPattern >= mushroom_range_pic[ENEMY_STATE_ATTACK].PATTERN_MAX)
        {
            targetEnemy->attackCoolTimer = MUSHROOM_RANGE_ATTACK_COOL_FRAME;
            SetEnemyState(targetEnemy, targetEnemy->isChasing ? ENEMY_STATE_RUN : ENEMY_STATE_IDLE);
        }

        break;
        }

    case ENEMY_STATE_HIT:
        {
        targetEnemy->animeFrame++;
        int animeSpeed = mushroom_range_pic[ENEMY_STATE_HIT].ANIME_SPEED;
        if (animeSpeed <= 0) animeSpeed = 1;
        int hitPattern = targetEnemy->animeFrame / animeSpeed;
        if (hitPattern >= mushroom_range_pic[ENEMY_STATE_HIT].PATTERN_MAX)
        {
            SetEnemyState(targetEnemy, targetEnemy->isChasing ? ENEMY_STATE_ALERT : ENEMY_STATE_IDLE);
        }
        break;
        }

    case ENEMY_STATE_DEAD:
        {
        targetEnemy->vel.x = 0.0f;
        int animeSpeed = mushroom_range_pic[ENEMY_STATE_DEAD].ANIME_SPEED;
        if (animeSpeed <= 0) animeSpeed = 1;

        int lastAnimeFrame = (mushroom_range_pic[ENEMY_STATE_DEAD].PATTERN_MAX - 1) * animeSpeed;

        if (targetEnemy->animeFrame < lastAnimeFrame) targetEnemy->animeFrame++;
        else targetEnemy->animeFrame = lastAnimeFrame;

        break;
        }
    }

    UpdateEnemyPhysics(targetEnemy);
    UpdateEnemySize(targetEnemy, &mushroom_range_pic[targetEnemy->state]);

    //------------------ チュートリアルの近接キノコくんをデカくする ---------------------
    float stageScale = 1.0f;
    if (GetCurrentGameStage() == GAME_STAGE_T_03) stageScale = TUTORIAL_RANGE_SCALE;
    targetEnemy->size = MakeFloat2(targetEnemy->size.x * stageScale, targetEnemy->size.y * stageScale);
    //-------------------------------------------------------------------------------

    UpdateEnemyCollision(targetEnemy, MUSHROOM_RANGE_COLLISION_SIZE_RATE);

}

// =========================================================
// 遠距離キノコ描画
// =========================================================
void DrawMushroomRange(Mushroom_Range* targetEnemy)
{
    if (!targetEnemy->use) return;
    int picId = targetEnemy->state;
    if (picId < 0 || picId >= MUSHROOM_RANGE_PIC_NUM) picId = ENEMY_STATE_IDLE;
    //---------------------------------------------------------------------
    // 追跡中に移動可能範囲の端へ到達して停止している場合はIDLE画像を使用する
    if (targetEnemy->state == ENEMY_STATE_RUN && targetEnemy->isChasing && fabsf(targetEnemy->vel.x) <= 0.0001f) picId = ENEMY_STATE_IDLE;
    //---------------------------------------------------------------------
    int animeSpeed =mushroom_range_pic[picId].ANIME_SPEED;
    if (animeSpeed <= 0) animeSpeed = 1;
    int patternMax = mushroom_range_pic[picId].PATTERN_MAX;
    if (patternMax <= 0) patternMax = 1;
    int patternNumU = mushroom_range_pic[picId].PATTERN_NUM_U;
    if (patternNumU <= 0) patternNumU = 1; 

    int frame = (targetEnemy->animeFrame / animeSpeed) % patternMax;
    float tx = mushroom_range_pic[picId].PATTERN_WIDTH * (frame % patternNumU);
    float ty = mushroom_range_pic[picId].PATTERN_HIGHT * (frame / patternNumU);

    DrawSpriteAnimation_Scroll(
        targetEnemy->pos.x, targetEnemy->pos.y,
        targetEnemy->size.x, targetEnemy->size.y,
        targetEnemy->color, targetEnemy->rotation,
        tx, ty,
        mushroom_range_pic[picId].PATTERN_WIDTH,
        mushroom_range_pic[picId].PATTERN_HIGHT,
        MushroomRangeTextureId[picId],
        targetEnemy->isFacingRight
    );
}

// =========================================================
// 遠距離キノコ終了処理
// =========================================================
void FinalizeMushroomRange()
{
    for (int i = 0; i < MUSHROOM_RANGE_PIC_NUM; i++)
    {
        if (MushroomRangeTextureId[i] != 0)
        {
            UnloadTexture(MushroomRangeTextureId[i]);
            MushroomRangeTextureId[i] = 0;
        }
    }

    for (int i = 0; i < MAX_ENEMY; i++)
    {
        mushroomRange[i].use = false;
    }
}

#pragma endregion

// =========================================================
// 状態変更
// =========================================================
void SetEnemyState(Enemy* targetEnemy, ENEMY_STATE nextState)
{
    if (targetEnemy->state == nextState) return;
    targetEnemy->state = nextState;
    targetEnemy->animeFrame = 0;

    switch (nextState)
    {
    case ENEMY_STATE_IDLE:
        targetEnemy->vel.x = 0.0f;
        targetEnemy->stateTimer = GetRandomFrameFromSecond(ENEMY_IDLE_MIN_SECOND, ENEMY_IDLE_MAX_SECOND);
        break;
    case ENEMY_STATE_ALERT:
        targetEnemy->vel.x = 0.0f;
        targetEnemy->stateTimer = ENEMY_ALERT_FRAME;
        // 今回の戦闘で初めてPlayerを発見した時だけ表示
        if (!targetEnemy->hasShownAlert)
        {
            SetAlertEffect(targetEnemy);
            targetEnemy->hasShownAlert = true;
        }
        break;
    case ENEMY_STATE_RUN:
        targetEnemy->stateTimer = GetRandomFrameFromSecond(ENEMY_RUN_MIN_SECOND, ENEMY_RUN_MAX_SECOND);
        break;
    case ENEMY_STATE_ATTACK:
        targetEnemy->vel.x = 0.0f;
        break;
    case ENEMY_STATE_HIT:
        targetEnemy->vel.x = 0.0f;
        targetEnemy->isChasing = true;
        targetEnemy->lostPlayerTimer = static_cast<int>ENEMY_LOST_PLAYER_FRAME;
        targetEnemy->lastPlayerPos = GetPlayer()->pos;
        if (GetPlayer()->pos.x < targetEnemy->pos.x)
        {
            targetEnemy->moveDir = -1;
            targetEnemy->isFacingRight = false;
        }
        else if (GetPlayer()->pos.x > targetEnemy->pos.x)
        {
            targetEnemy->moveDir = 1;
            targetEnemy->isFacingRight = true;
        }
        break;
    case ENEMY_STATE_DEAD:
        targetEnemy->vel.x = 0.0f;
        targetEnemy->isChasing = false;
        targetEnemy->hasShownAlert = false;
        targetEnemy->lostPlayerTimer = 0;
        break;
    }
}

// =========================================================
// Sprite Sheetの単フレームサイズからEnemyサイズを更新
// =========================================================
void UpdateEnemySize(Enemy* targetEnemy, const Pic_Data_Enemy* picData)
{
    if (picData->PATTERN_NUM_U <= 0 || picData->PATTERN_NUM_V <= 0)
    {
        targetEnemy->size = MakeFloat2(0.0f, 0.0f);
        return;
    }
    float frameWidth = picData->PIC_SIZE.x / picData->PATTERN_NUM_U;
    float frameHeight = picData->PIC_SIZE.y / picData->PATTERN_NUM_V;
    targetEnemy->size = MakeFloat2(frameWidth * targetEnemy->scale, frameHeight * targetEnemy->scale);
}

// =========================================================
// Bulletとの当たり判定を更新
// =========================================================
void UpdateEnemyCollision(Enemy* targetEnemy, float targetSizeRate)
{
    if (targetEnemy->state == ENEMY_STATE_DEAD)
    {
        targetEnemy->CollisionPosition = MakeFloat2(0.0f, 0.0f);
        targetEnemy->CollisionSize = MakeFloat2(0.0f, 0.0f);
        return;
    }

    targetEnemy->CollisionPosition = 
        MakeFloat2(
            targetEnemy->pos.x + targetEnemy->CollisionOffset.x,
            targetEnemy->pos.y + targetEnemy->CollisionOffset.y
        );

    targetEnemy->CollisionSize = 
        MakeFloat2(
            targetEnemy->size.x * targetSizeRate,
            targetEnemy->size.y * targetSizeRate
        );
}

// =========================================================
// 敵移動・外部速度・重力
// =========================================================
void UpdateEnemyPhysics(Enemy* targetEnemy)
{
    if (targetEnemy->state == ENEMY_STATE_DEAD) return;
    targetEnemy->vel.x += targetEnemy->exVel.x;
    targetEnemy->vel.y += targetEnemy->exVel.y;
    targetEnemy->exVel = MakeFloat2(0.0f, 0.0f);

    if (targetEnemy->pos.y < targetEnemy->groundY || targetEnemy->vel.y < 0.0f) targetEnemy->vel.y += targetEnemy->gravityAcc;
    float nextPosX = targetEnemy->pos.x + targetEnemy->vel.x;

    if (nextPosX < targetEnemy->patrolLeft)
    {
        targetEnemy->pos.x = targetEnemy->patrolLeft;
        targetEnemy->vel.x = 0.0f;
        targetEnemy->moveDir = 1;
        targetEnemy->isFacingRight = true;
    }
    else if (nextPosX > targetEnemy->patrolRight)
    {
        targetEnemy->pos.x = targetEnemy->patrolRight;
        targetEnemy->vel.x = 0.0f;
        targetEnemy->moveDir = -1;
        targetEnemy->isFacingRight = false;
    }
    else targetEnemy->pos.x = nextPosX;

    targetEnemy->pos.y += targetEnemy->vel.y;
    if (targetEnemy->pos.y >= targetEnemy->groundY)
    {
        targetEnemy->pos.y = targetEnemy->groundY;
        targetEnemy->vel.y = 0.0f;
    }
}

// =========================================================
// プレイヤーを視認できるか
// =========================================================
bool CanEnemySeePlayer(const Enemy* targetEnemy, Float2 playerPos, Float2 playerCollisionSize)
{
    // 追跡前はEnemyの向いている方向だけを視認する
    if (!targetEnemy->isChasing)
    {
        if (targetEnemy->isFacingRight && playerPos.x < targetEnemy->CollisionPosition.x) return false;
        if (!targetEnemy->isFacingRight && playerPos.x > targetEnemy->CollisionPosition.x) return false;
    }

    // Rayの発射位置
    Float2 rayOrigin = MakeFloat2(targetEnemy->CollisionPosition.x, targetEnemy->CollisionPosition.y - targetEnemy->CollisionSize.y / 2.0f);
    float playerLeft   = playerPos.x - playerCollisionSize.x / 2.0f;
    float playerRight  = playerPos.x + playerCollisionSize.x / 2.0f;
    float playerTop    = playerPos.y - playerCollisionSize.y / 2.0f;
    float playerBottom = playerPos.y + playerCollisionSize.y / 2.0f;

    // Playerの上側3点
    Float2 topPoints[3]
    {
        MakeFloat2(playerLeft, playerTop),
        MakeFloat2(playerPos.x, playerTop),
        MakeFloat2(playerRight, playerTop)
    };

    // Playerの左右辺中央2点
    Float2 centerPoints[2]
    {
        MakeFloat2(playerLeft, playerPos.y),
        MakeFloat2(playerRight, playerPos.y)
    };

    // Playerの下側3点
    Float2 downPoints[3]
    {
        MakeFloat2(playerLeft, playerBottom),
        MakeFloat2(playerPos.x, playerBottom),
        MakeFloat2(playerRight, playerBottom)
    };

    // 各グループからRay起点に最も近い点を選択する
    Float2 targetTop    = GetNearestPoint(rayOrigin, topPoints, 3);
    Float2 targetCenter = GetNearestPoint(rayOrigin, centerPoints, 2);
    Float2 targetDown   = GetNearestPoint(rayOrigin, downPoints, 3);

    Float2 toPlayerTop    = MakeFloat2(targetTop.x - rayOrigin.x, targetTop.y - rayOrigin.y);
    Float2 toPlayerCenter = MakeFloat2(targetCenter.x - rayOrigin.x, targetCenter.y - rayOrigin.y);
    Float2 toPlayerDown   = MakeFloat2(targetDown.x - rayOrigin.x, targetDown.y - rayOrigin.y);

    float distanceTop    = sqrtf(toPlayerTop.x * toPlayerTop.x + toPlayerTop.y * toPlayerTop.y);
    float distanceCenter = sqrtf(toPlayerCenter.x * toPlayerCenter.x + toPlayerCenter.y * toPlayerCenter.y);
    float distanceDown   = sqrtf(toPlayerDown.x * toPlayerDown.x + toPlayerDown.y * toPlayerDown.y);

    bool canSeeTop    = distanceTop > 0.0001f && distanceTop <= targetEnemy->searchRange;
    bool canSeeCenter = distanceCenter > 0.0001f && distanceCenter <= targetEnemy->searchRange;
    bool canSeeDown   = distanceDown > 0.0001f && distanceDown <= targetEnemy->searchRange;

    Float2 rayDirTop    = MakeFloat2(0.0f, 0.0f);
    Float2 rayDirCenter = MakeFloat2(0.0f, 0.0f);
    Float2 rayDirDown   = MakeFloat2(0.0f, 0.0f);

    if (canSeeTop) rayDirTop = MakeFloat2(toPlayerTop.x / distanceTop, toPlayerTop.y / distanceTop);
    if (canSeeCenter) rayDirCenter = MakeFloat2(toPlayerCenter.x / distanceCenter, toPlayerCenter.y / distanceCenter);
    if (canSeeDown) rayDirDown = MakeFloat2(toPlayerDown.x / distanceDown, toPlayerDown.y / distanceDown);

    // 遮蔽物検測
    BLOCK* block = GetBlock();
    int blockCount = GetBlockCount();
    for (int i = 0; i < blockCount; i++)
    {
        if (!block[i].use) continue;

        if (canSeeTop)
        {
            float hitDist = 0.0f;
            if (CheckRaycastBox(rayOrigin, rayDirTop, block[i].CollisionPosition, block[i].CollisionSize, hitDist))
            {
                if (hitDist > 0.0f && hitDist < distanceTop) canSeeTop = false;
            }
        }

        if (canSeeCenter)
        {
            float hitDist = 0.0f;
            if (CheckRaycastBox(rayOrigin, rayDirCenter, block[i].CollisionPosition, block[i].CollisionSize, hitDist))
            {
                if (hitDist > 0.0f && hitDist < distanceCenter) canSeeCenter = false; 
            }
        }

        if (canSeeDown)
        {
            float hitDist = 0.0f;
            if (CheckRaycastBox(rayOrigin, rayDirDown, block[i].CollisionPosition, block[i].CollisionSize, hitDist))
            {
                if (hitDist > 0.0f && hitDist < distanceDown) canSeeDown = false;
            }
        }

        // 3本とも遮られた場合は以降を確認しない
        if (!canSeeTop && !canSeeCenter && !canSeeDown) return false;
    }

    // ColorBlock
    COLOR_CHANGE_BLOCK* ccBlock = GetColorChangeBlock();
    for (int i = 0; i < CCBLOCK_MAX; i++)
    {
        if (!ccBlock[i].use) continue;

        if (canSeeTop)
        {
            float hitDist = 0.0f;

            if (CheckRaycastBox(rayOrigin, rayDirTop, ccBlock[i].CollisionPos, ccBlock[i].CollisionSize, hitDist))
            {
                if (hitDist > 0.0f && hitDist < distanceTop) canSeeTop = false;
            }
        }

        if (canSeeCenter)
        {
            float hitDist = 0.0f;

            if (CheckRaycastBox(rayOrigin, rayDirCenter, ccBlock[i].CollisionPos, ccBlock[i].CollisionSize, hitDist))
            {
                if (hitDist > 0.0f && hitDist < distanceCenter) canSeeCenter = false;
            }
        }

        if (canSeeDown)
        {
            float hitDist = 0.0f;

            if (CheckRaycastBox(rayOrigin, rayDirDown, ccBlock[i].CollisionPos, ccBlock[i].CollisionSize, hitDist))
            {
                if (hitDist > 0.0f && hitDist < distanceDown) canSeeDown = false;
            }
        }

        if (!canSeeTop && !canSeeCenter && !canSeeDown) return false;
    }

    // どれか1本でも見えていればPlayerを発見する
    return canSeeTop || canSeeCenter || canSeeDown;
}

// =========================================================
// 指定された点の中からRay起点に最も近い点を取得
// =========================================================
Float2 GetNearestPoint(Float2 rayOrigin, const Float2* points, int pointNum)
{
    Float2 nearestPoint = points[0];
    float nearestDistanceSq =
        (points[0].x - rayOrigin.x) * (points[0].x - rayOrigin.x) +
        (points[0].y - rayOrigin.y) * (points[0].y - rayOrigin.y);

    for (int i = 1; i < pointNum; i++)
    {
        float distanceSq =
            (points[i].x - rayOrigin.x) * (points[i].x - rayOrigin.x) +
            (points[i].y - rayOrigin.y) * (points[i].y - rayOrigin.y);

        if (distanceSq < nearestDistanceSq)
        {
            nearestDistanceSq = distanceSq;
            nearestPoint = points[i];
        }
    }

    return nearestPoint;
}

// =========================================================
// PlayerがEnemyの攻撃範囲内にいるか
// =========================================================
bool IsPlayerInEnemyAttackRange(const Enemy* targetEnemy, Float2 playerPos)
{
    float distanceX = playerPos.x - targetEnemy->CollisionPosition.x;
    float distanceY = playerPos.y - targetEnemy->CollisionPosition.y;
    float distanceSq = distanceX * distanceX + distanceY * distanceY;

    return distanceSq <= targetEnemy->attackRange * targetEnemy->attackRange;
}

// =========================================================
// 秒数の範囲から60FPS用のフレーム数を取得、パトロール用
// =========================================================
int GetRandomFrameFromSecond(int minSecond, int maxSecond)
{
    if (maxSecond < minSecond)
    {
        int temp = minSecond;
        minSecond = maxSecond;
        maxSecond = temp;
    }
    int second = minSecond + rand() % (maxSecond - minSecond + 1);
    return second * 60;
}

// =========================================================
// Aimedエフェクト描画
// =========================================================
void DrawAimedEffect(Enemy* targetEnemy)
{
    if (targetEnemy == nullptr) return;
    if (!targetEnemy->use) return;
    if (targetEnemy->state == ENEMY_STATE_DEAD) return;
    if (targetEnemy->CollisionSize.x <= 0.0f || targetEnemy->CollisionSize.y <= 0.0f) return;

    int animeSpeed = aimed_effect.ANIME_SPEED;
    if (animeSpeed <= 0) animeSpeed = 1;
    int patternMax = aimed_effect.PATTERN_MAX;
    if (patternMax <= 0) patternMax = 1;
    int patternNumU = aimed_effect.PATTERN_NUM_U;
    if (patternNumU <= 0) patternNumU = 1;
    int patternNumV = aimed_effect.PATTERN_NUM_V;
    if (patternNumV <= 0) patternNumV = 1;

    int frame = (aimedEffectFrame / animeSpeed) % patternMax;
    float tx = aimed_effect.PATTERN_WIDTH * (frame % patternNumU);
    float ty = aimed_effect.PATTERN_HIGHT * (frame / patternNumU);
    float tw = aimed_effect.PATTERN_WIDTH;
    float th = aimed_effect.PATTERN_HIGHT;

    float frameWidth = aimed_effect.PIC_SIZE.x / patternNumU;
    float frameHeight = aimed_effect.PIC_SIZE.y / patternNumV;

    float targetWidth = targetEnemy->CollisionSize.x * AIMED_EFFECT_SCALE_RATIO;
    float targetHeight = targetEnemy->CollisionSize.y * AIMED_EFFECT_SCALE_RATIO;

    float scaleX = targetWidth / frameWidth;
    float scaleY = targetHeight / frameHeight;
    float scale = scaleX > scaleY ? scaleX : scaleY;

    float drawWidth = frameWidth * scale;
    float drawHeight = frameHeight * scale;

    DrawSpriteAnimation_Scroll(
        targetEnemy->CollisionPosition.x, targetEnemy->CollisionPosition.y,
        drawWidth, drawHeight, MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f,
        tx, ty, tw, th,
        AimedEffectTextureId,
        true
    );
}

// =========================================================
// Bulletと全Enemyの当たり判定
// =========================================================
bool BulletEnemyCollision(Float2 bulletCollisionPosition, Float2 bulletCollisionSize, Float2 bulletDir, float bulletDamage, int bulletType)
{
    // 近接キノコ
    for (int i = 0; i < MAX_ENEMY; i++)
    {
        Mushroom_Melee* targetEnemy = &mushroomMelee[i];
        if (!targetEnemy->use) continue;
        if (targetEnemy->state == ENEMY_STATE_DEAD) continue;
        if (!CheckBoxCollider(bulletCollisionPosition, targetEnemy->CollisionPosition, bulletCollisionSize, targetEnemy->CollisionSize)) continue;

        bool isAttacking = targetEnemy->state == ENEMY_STATE_ATTACK;
        targetEnemy->hp -= bulletDamage;
        if (!isAttacking)
        {
            if (bulletType == 0)
            {
                targetEnemy->exVel.x += bulletDir.x * MUSHROOM_MELEE_KNOCKBACK_POWER;
                targetEnemy->exVel.y += bulletDir.y * MUSHROOM_MELEE_KNOCKBACK_POWER;
            }
            else if (bulletType == 1)
            {
                targetEnemy->exVel.x += bulletDir.x * (MUSHROOM_MELEE_KNOCKBACK_POWER * 2.0f);
                targetEnemy->exVel.y += bulletDir.y * (MUSHROOM_MELEE_KNOCKBACK_POWER * 2.0f);
            }
        }

        if (targetEnemy->hp <= 0.0f)
        {
            SetEnemyState(targetEnemy, ENEMY_STATE_DEAD);

            //Tutorial_03のキノコ専用
            SetTutorialColorBlockMoving();
        }
        else if (!isAttacking) SetEnemyState(targetEnemy, ENEMY_STATE_HIT);
        return true;
    }

    // 遠隔キノコ
    for (int i = 0; i < MAX_ENEMY; i++)
    {
        Mushroom_Range* targetEnemy = &mushroomRange[i];
        if (!targetEnemy->use) continue;
        if (targetEnemy->state == ENEMY_STATE_DEAD) continue;
        if (!CheckBoxCollider(bulletCollisionPosition, targetEnemy->CollisionPosition, bulletCollisionSize, targetEnemy->CollisionSize)) continue;

        bool isAttacking = targetEnemy->state == ENEMY_STATE_ATTACK;
        targetEnemy->hp -= bulletDamage;
        if (!isAttacking)
        {
            if (bulletType == 0)
            {
                targetEnemy->exVel.x += bulletDir.x * MUSHROOM_RANGE_KNOCKBACK_POWER;
                targetEnemy->exVel.y += bulletDir.y * MUSHROOM_RANGE_KNOCKBACK_POWER;
            }
            else if (bulletType == 1)
            {
                targetEnemy->exVel.x += bulletDir.x * (MUSHROOM_RANGE_KNOCKBACK_POWER * 2.0f);
                targetEnemy->exVel.y += bulletDir.y * (MUSHROOM_RANGE_KNOCKBACK_POWER * 2.0f);
            }
        }
        if (targetEnemy->hp <= 0.0f) SetEnemyState(targetEnemy, ENEMY_STATE_DEAD);
        else if (!isAttacking) SetEnemyState(targetEnemy, ENEMY_STATE_HIT);
        return true;
    }

    // 後で他Enemyを追加する場合は、各Enemy配列をここで順番に確認する
    return false;
}

// =========================================================
// Charge Bullet ExplosionとのAOE当たり判定
// =========================================================
bool ChargeExplosionCollisionEnemy(Float2 explosionPos, float explosionRadius, float explosionDamage)
{
    // 近接キノコ
    for (int i = 0; i < MAX_ENEMY; i++)
    {
        Mushroom_Melee* targetEnemy = &mushroomMelee[i];
        if (!targetEnemy->use) continue;
        if (targetEnemy->state == ENEMY_STATE_DEAD) continue;
        if (!CheckCircleCollider(explosionPos, targetEnemy->CollisionPosition, explosionRadius, targetEnemy->CollisionSize.x)) continue;

        targetEnemy->hp -= explosionDamage;
        Float2 explosionVector = MakeFloat2(targetEnemy->CollisionPosition.x - explosionPos.x, targetEnemy->CollisionPosition.y - explosionPos.y);
        float magnitude = sqrtf(explosionVector.x * explosionVector.x + explosionVector.y * explosionVector.y);
        Float2 dir{};
        if (magnitude > 0.000001f) dir = MakeFloat2(explosionVector.x / magnitude, explosionVector.y / magnitude);
        else dir = MakeFloat2(1.0f, 0.0f);

        targetEnemy->exVel.x += dir.x * (MUSHROOM_MELEE_KNOCKBACK_POWER * 2.0f);
        targetEnemy->exVel.y += dir.y * (MUSHROOM_MELEE_KNOCKBACK_POWER * 2.0f);

        if (targetEnemy->hp <= 0.0f)
        {
            SetEnemyState(targetEnemy, ENEMY_STATE_DEAD);

            //Tutorial_03のキノコ専用
            SetTutorialColorBlockMoving();
        }
        else SetEnemyState(targetEnemy, ENEMY_STATE_HIT);
        return true;
    }

    // 遠隔キノコ
    for (int i = 0; i < MAX_ENEMY; i++)
    {
        Mushroom_Range* targetEnemy = &mushroomRange[i];
        if (!targetEnemy->use) continue;
        if (targetEnemy->state == ENEMY_STATE_DEAD) continue;
        if (!CheckCircleCollider(explosionPos, targetEnemy->CollisionPosition, explosionRadius, targetEnemy->CollisionSize.x)) continue;

        targetEnemy->hp -= explosionDamage;
        Float2 explosionVector = MakeFloat2(targetEnemy->CollisionPosition.x - explosionPos.x, targetEnemy->CollisionPosition.y - explosionPos.y);
        float magnitude = sqrtf(explosionVector.x * explosionVector.x + explosionVector.y * explosionVector.y);
        Float2 dir{};
        if (magnitude > 0.000001f) dir = MakeFloat2(explosionVector.x / magnitude, explosionVector.y / magnitude);
        else dir = MakeFloat2(1.0f, 0.0f);

        targetEnemy->exVel.x += dir.x * (MUSHROOM_RANGE_KNOCKBACK_POWER * 2.0f);
        targetEnemy->exVel.y += dir.y * (MUSHROOM_RANGE_KNOCKBACK_POWER * 2.0f);

        if (targetEnemy->hp <= 0.0f) SetEnemyState(targetEnemy, ENEMY_STATE_DEAD);
        else SetEnemyState(targetEnemy, ENEMY_STATE_HIT);
        return true;
    }

    // 後で他Enemyを追加する場合は、各Enemy配列をここで順番に確認する
    return false;
}

// =========================================================
// Alert Effect生成
// =========================================================
void SetAlertEffect(Enemy* targetEnemy)
{
    if (targetEnemy == nullptr) return;

    // 同じEnemyのEffectがある場合は最初から再生する
    for (int i = 0; i < ALERT_EFFECT_MAX; i++)
    {
        if (!alertEffect[i].use) continue;

        if (alertEffect[i].targetEnemy == targetEnemy)
        {
            alertEffect[i].frame = 0;
            return;
        }
    }

    // 空いている場所に生成する
    for (int i = 0; i < ALERT_EFFECT_MAX; i++)
    {
        if (alertEffect[i].use) continue;

        alertEffect[i].targetEnemy = targetEnemy;
        alertEffect[i].frame = 0;
        alertEffect[i].use = true;
        return;
    }
}

// =========================================================
// Alert Effect更新
// =========================================================
void UpdateAlertEffect()
{
    int animeSpeed = alert_effect.ANIME_SPEED;
    if (animeSpeed <= 0) animeSpeed = 1;

    int endFrame = alert_effect.PATTERN_MAX * animeSpeed;
    for (int i = 0; i < ALERT_EFFECT_MAX; i++)
    {
        if (!alertEffect[i].use) continue;
        Enemy* targetEnemy = alertEffect[i].targetEnemy;

        if (targetEnemy == nullptr || !targetEnemy->use || targetEnemy->state == ENEMY_STATE_DEAD)
        {
            alertEffect[i] = ALERT_EFFECT{};
            continue;
        }

        alertEffect[i].frame++;
        // 一度再生が終わったら終了
        if (alertEffect[i].frame >= endFrame)
        {
            alertEffect[i] = ALERT_EFFECT{};
        }
    }
}

// =========================================================
// Alert Effect描画
// =========================================================
void DrawAlertEffect()
{
    int animeSpeed = alert_effect.ANIME_SPEED; if (animeSpeed <= 0) animeSpeed = 1;
    int patternMax = alert_effect.PATTERN_MAX; if (patternMax <= 0) patternMax = 1;
    int patternNumU = alert_effect.PATTERN_NUM_U; if (patternNumU <= 0) patternNumU = 1;

    for (int i = 0; i < ALERT_EFFECT_MAX; i++)
    {
        if (!alertEffect[i].use) continue;

        Enemy* targetEnemy = alertEffect[i].targetEnemy;
        if (targetEnemy == nullptr) continue;

        int frame = alertEffect[i].frame / animeSpeed;
        if (frame >= patternMax) frame = patternMax - 1;

        float tx = alert_effect.PATTERN_WIDTH * (frame % patternNumU);
        float ty = alert_effect.PATTERN_HIGHT * (frame / patternNumU);

        float frameWidth = alert_effect.PIC_SIZE.x / alert_effect.PATTERN_NUM_U;
        float frameHeight = alert_effect.PIC_SIZE.y /  alert_effect.PATTERN_NUM_V;
        float targetWidth = targetEnemy->CollisionSize.x * ALERT_EFFECT_SCALE_RATIO;
        float targetHeight = targetEnemy->CollisionSize.y * ALERT_EFFECT_SCALE_RATIO;
        float scaleX = targetWidth / frameWidth;
        float scaleY = targetHeight / frameHeight;
        float sizeRatio = scaleX > scaleY ? scaleX : scaleY;
        float drawWidth = frameWidth * sizeRatio;
        float drawHeight = frameHeight * sizeRatio;

        float drawX{};
        float drawY = targetEnemy->CollisionPosition.y - targetEnemy->CollisionSize.y / 2.0f;
        bool flipX{};

        if (targetEnemy->isFacingRight)
        {
            // Enemyが右向きの場合、Collision左上
            drawX = targetEnemy->CollisionPosition.x - targetEnemy->CollisionSize.x / 2.0f;
            flipX = false;
        }
        else
        {
            // Enemyが左向きの場合、Collision右上
            drawX = targetEnemy->CollisionPosition.x + targetEnemy->CollisionSize.x / 2.0f;
            flipX = true;
        }

        DrawSpriteAnimation_Scroll(
            drawX, drawY,
            drawWidth, drawHeight,
            MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f,
            tx, ty,
            alert_effect.PATTERN_WIDTH, alert_effect.PATTERN_HIGHT,
            AlertEffectTextureId, flipX
        );
    }
}

// =========================================================
// Enemy Loader
// =========================================================
void ReloadEnemyStage()
{
    for (int i = 0; i < MAX_ENEMY; i++)
    {
        mushroomMelee[i] = Mushroom_Melee{};
        mushroomRange[i] = Mushroom_Range{};
    }

    aimedEffectFrame = 0;

    for (int i = 0; i < ALERT_EFFECT_MAX; i++)
    {
        alertEffect[i] = ALERT_EFFECT{};
    }

    SpawnMushroomMeleeCurrentStage();
    SpawnMushroomRangeCurrentStage();
}

// =========================================================
// Enemyゲッター
// =========================================================
// 近接キノコ
Mushroom_Melee* GetMushroomMelee()
{
    return &mushroomMelee[0];
}

// 遠隔キノコ
Mushroom_Range* GetMushroomRange()
{
    return &mushroomRange[0];
}