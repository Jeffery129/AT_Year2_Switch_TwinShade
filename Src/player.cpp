// =========================================================
// player.cpp プレイヤー制御
// =========================================================
#include "main.h"
#include "texture.h"
#include "sprite.h"
#include "controller.h"
#include "player.h"
#include "collision.h"
#include "block.h"
#include "animation.h"
#include <cmath> // atan2, cos, sin を使用するため
#include "score.h"
#include "item.h"
#include "bullet.h"
#include "sound.h"
#include "game.h"
#include "fade.h"
#include "enemy.h"
#include "enemy_bullet.h"
#include "color_change_block.h"
#include "camera.h"
#include "boss.h"

// =========================================================
// マクロ
// =========================================================
#define PLAYER_SIZE_X (200.0f)
#define PLAYER_SIZE_Y (200.0f)

#define PLAYER_COLLISION_SIZE_W        (105.0f)
#define PLAYER_COLLISION_SIZE_H        (200.0f)

//Switch用パラメーター
#define MOVE_SPEED (15.0f)
#define MOVE_ACC   (0.25f) // 加速時の補間係数
#define MOVE_DAMP  (0.5f) // 減速（ブレーキ）時の補間係数

#define GRAVITY (9.8f / 60.0f * 10.0f)//switch用
#define JUMP_POWER (-28.0f)

#define COYOTE_TIME_FRAMES              (10)
#define WALL_JUMP_COYOTE_TIME_FRAMES    (5)

//--------------- AIM ---------------------
#define AIM_GRAVITY_SCALE           (0.4f)
#define AIM_HORIZON_ACC             (0.2f)
#define AIM_HORIZON_DAMP            (0.75f)
#define AIM_UPWARD_DAMP             (0.8f)
#define AIM_DOWNWARD_DAMP           (0.8f)
#define AIM_MAX_UP_SPEED_RATE       (0.5f)
#define AIM_HORIZON_SPEED_RATE      (0.1f)

//--------------- SHOOT -------------------
#define SHOOT_NORMAL_SCALE          (2.5f)
#define SHOOT_MIN_SCALE             (5.0f) //変更があれば, bulletのcamera shakeも変更して (bullet.cpp)
#define SHOOT_MAX_SCALE             (6.5f) //変更があれば, bulletのcamera shakeも変更して (bullet.cpp)

#define SHOOT_CHARGE_START_FRAME    (10)
#define SHOOT_CHARGE_MAX_FRAME      (50)
#define SHOOT_CHARGE_HALF_FRAME     (SHOOT_CHARGE_START_FRAME + (SHOOT_CHARGE_MAX_FRAME - SHOOT_CHARGE_START_FRAME) / 2)
#define CHARGE_VFX_MIN_SCALE_RATIO  (1.0f)
#define CHARGE_VFX_MAX_SCALE_RATIO  (3.0f)
#define CHARGE_VFX_CHANGE_RATIO     (0.05f)

#define SHOOT_START_OFFSET          (40.0f)

#define BULLET_NORMAL_RECOIL        (1.5f)
#define BULLET_CHARGE_MIN_RECOIL    (10.0f)
#define BULLET_CHARGE_MAX_RECOIL    (25.0f)
#define SHOOT_DOWN_RECOIL_RATE      (0.2f)

//--------------- SPRINT -------------------
#define SPRINT_FRAME                (13)
#define SPRINT_SPEED                (40.0f)
#define SPRINT_COOL_DOWN_FRAME      (1 * 60)
#define SPRINT_BAR_Y_OFFSET         (115.0f)
#define SPRINT_BAR_LENGTH           (100.0f)
//--------------- PLAYER SHADOW ------------
#define PLAYER_SHADOW_MAX           (12)
#define PLAYER_SHADOW_INTERVAL      (2)
#define PLAYER_SHADOW_START_ALPHA   (0.36f)
#define PLAYER_SHADOW_FADE          (0.04f)

//--------------- CLING -------------------
#define WALL_CLING_GRAVITY_SCALE    (0.2f)
#define WALL_CLING_MAX_FALL_SPEED   (1.3f)
#define WALL_JUMP_X_POWER           (MOVE_SPEED * 1.3f)
#define WALL_JUMP_Y_BOOST           (1.0f)
#define WALL_CHECK_WIDTH            (10.0f)
#define WALL_CHECK_HEIGHT           (10.0f)
#define WALL_CHECK_OFFSET           (5.0f)
#define WALL_CLING_MAX_FRAME        (2.5f * 60)
#define WALL_CLING_SWEATY_RATIO     (0.8f)

//---------------- HIT --------------------
#define HIT_BACK_POWER              (10.0f)
#define HIT_INVINCIBLE_FRAME        (1 * 60)

//---------------- DEAD -------------------
#define DEAD_FADE_START_PATTERN     (10)
#define REVIVE_INVINCIBLE_FRAME     (2 * 60)

//---------------- UI ---------------------
//-- ICON --
#define ICON_POS_X                  (MAP_BLOCK_WIDTH * 1.5f)
#define ICON_POS_Y                  (MAP_BLOCK_HEIGHT* 1.5f)
#define ICON_SIZE_X                 (240.0f)
#define ICON_SIZE_Y                 (240.0f)

//-- Hit Frame --
#define FRAME_POS_X                 (-SCREEN_WIDTH / 2.0f + 215.0f)
#define FRAME_POS_Y                 (-SCREEN_HEIGHT / 2.0f + 200.0f)
#define FRAME_SIZE_X                (240.0f * 0.8f)
#define FRAME_SIZE_Y                (35.0f)

//--- HP bar ---
#define HP_BAR_POS_X                (-SCREEN_WIDTH / 2.0f + MAP_BLOCK_WIDTH * 3.0f + 50.0f)
#define HP_BAR_POS_Y                (-SCREEN_HEIGHT / 2.0f + MAP_BLOCK_HEIGHT * 0.5f + 50.0f)
#define HP_BAR_SIZE_X               (340.0f)
#define HP_BAR_SIZE_Y               (120.0f)

//-- SHOOT ENERGY BAR --
#define SHOOT_ENERGY_MAX_CHARGE     (10)
#define SHOOT_ENERGY_BAR_LENGTH     (450.0f * 0.8f)
#define SHOOT_ENERGY_BAR_HEIGHT     (20.0f)
#define SHOOT_ENERGY_BAR_SPACE      (30.0f)
#define SHOOT_ENERGY_BAR_X          (-SCREEN_WIDTH / 2.0f + 215.0f)
#define SHOOT_ENERGY_BAR_Y          (-SCREEN_HEIGHT / 2.0f + 200.0f)
#define SHOOT_ENERGY_BAR_LERP       (0.3f)
#define SHOOT_ENERGY_PREVIEW_SPEED  (22.5f)

// =========================================================
// プロトタイプ宣言
// =========================================================
void PlayerIdle();
void PlayerWalk();
void PlayerJump();
void PlayerFall();
void PlayerHit();
void PlayerDead();

float HandleControl(float speed_rate);
void HandleColorChange();

//------- AIM -------
void PlayerAim();
void ApplyAimPhysics();

//------- CLING -------
void PlayerCling();
void UpdateWallCling();
bool IsTouchingWall(int wallDir);
bool IsPressingToWall(int wallDir);

//------- SHOOT -------
void UpdatePlayerShoot();
//------- ENERGY ------
float GetShootChargeRate();

//------- SPRINT -------
void StartPlayerSprint();
void PlayerSprint();
void EndPlayerSprint();
void DrawSprintCoolDownBar();
////------- SHADOW EFFECT -------
void SetPlayerShadow();
void UpdatePlayerShadow();
void DrawPlayerShadow();

//-------- COLLISION ------
bool RightCollision();
bool LeftCollision();
bool UpCollision();
bool DownCollision();
bool ItemCollision();
bool CheckGrounded();
void ApplyGroundCCBlockMovement();
void UpdateRespawnPoint();

//-------- OTHERS ------
void ResetAllEnemyIsAimed();
void ResetAllEnemyBulletIsAimed();

//-------- PLAYER STATUS --------
void DrawShootEnergyBar(void);   //UIです
void DrawPlayerIcon(void);
void DrawPlayerHp(void);
void DrawPlayerHitFrame(void);

// =========================================================
// グローバル変数
// =========================================================
PLAYER player;    // プレイヤーの実体
Float2 CENTER_OFFSET_PLAYER = MakeFloat2(MAP_BLOCK_WIDTH * 8.5f, MAP_BLOCK_HEIGHT * 5.0f); // 横5行目、縦8と9の真ん中は中心
Float2 player_start_pos[GAME_STAGE_MAX]
{
    // T_01
    MakeFloat2(
        5.0f * MAP_BLOCK_WIDTH - CENTER_OFFSET_PLAYER.x,
        15.5f * MAP_BLOCK_HEIGHT - CENTER_OFFSET_PLAYER.y - PLAYER_COLLISION_SIZE_H / 2
    ),

    // T_02
    MakeFloat2(
        5.5f * MAP_BLOCK_WIDTH - CENTER_OFFSET_PLAYER.x,
        23.5f * MAP_BLOCK_HEIGHT - CENTER_OFFSET_PLAYER.y - PLAYER_COLLISION_SIZE_H / 2
    ),

    // T_03
    MakeFloat2(
        2.5f * MAP_BLOCK_WIDTH - CENTER_OFFSET_PLAYER.x,
        5.5f * MAP_BLOCK_HEIGHT - CENTER_OFFSET_PLAYER.y - PLAYER_COLLISION_SIZE_H / 2
    ),

    // Main_Stage_01
    MakeFloat2(
        3.0f * MAP_BLOCK_WIDTH - CENTER_OFFSET_PLAYER.x,
        96.5f * MAP_BLOCK_HEIGHT - CENTER_OFFSET_PLAYER.y - PLAYER_COLLISION_SIZE_H / 2
    ),

    // Main_Stage_02
    MakeFloat2(
        4.5f * MAP_BLOCK_WIDTH - CENTER_OFFSET_PLAYER.x,
        96.5f * MAP_BLOCK_HEIGHT - CENTER_OFFSET_PLAYER.y - PLAYER_COLLISION_SIZE_H / 2
    ),

    // BOSS_STAGE
    MakeFloat2(
        11.0f * MAP_BLOCK_WIDTH - CENTER_OFFSET_PLAYER.x,
        9.5f * MAP_BLOCK_HEIGHT - CENTER_OFFSET_PLAYER.y - PLAYER_COLLISION_SIZE_H / 2
    ),
};

int g_frame{ 0 };
int g_anime_frame{ 0 };
int g_ShootChargeFrame{ 0 };
int g_ChargeEffectFrame{ 0 };
bool g_WasZRPressed{ false };
bool g_IsHalfChargeSEPlayed{ false };
bool g_IsFullChargeSEPlayed{ false };

//Energy Bar Draw
float g_DrawShootEnergy{ 0.0f };
float g_DrawPreviewLeftLength{ 0.0f };
float g_DrawPreviewRightLength{ 0.0f };

//------- SHADOW EFFECT -------
struct PLAYER_SHADOW
{
    Float2 pos;
    float alpha;
    int frame;
    int colorId;
    bool isFacingRight;
    bool use;
};

PLAYER_SHADOW playerShadow[PLAYER_SHADOW_MAX]{};
int playerShadowTimer{ 0 };

//------- STATUS_UI -------
enum STATUS_UI
{
    ICON = 0,
    HP_GREEN,
    HP_ORANGE,
    HP_RED_FAST,
    HP_RED_LINE,
    HP_FRAME,
    HIT_FRAME
};
int g_ui_frame{ 0 };

// =========================================================
// プレイヤー初期化
// =========================================================
void InitializePlayer(void)
{
    // アニメーションテクスチャの読み込み
    for (int i = 0; i < PLAYER_PIC_NUM; i++)
    {
        for (int j = 0; j < PLAYER_COLOR_NUM; j++)
        {
            player.TextureId[i][j] = LoadTexture(player_pic[i][j].FILE_NAME);
        }
    }

    // プレイヤーエフェクトの読み込み
    for (int i = 0; i < PLAYER_EFFECT_PIC_NUM; i++)
    {
        for (int j = 0; j < PLAYER_COLOR_NUM; j++)
        {
            player.Effect_TextureId[i][j] = LoadTexture(player_effect_pic[i][j].FILE_NAME);
        }
    }

    // Sprint Shadowテクスチャの読み込み
    for (int i = 0; i < PLAYER_COLOR_NUM; i++)
    {
        player.Shadow_TextureId[i] = LoadTexture(player_sprint_shadow[i].FILE_NAME);
    }

    // Player Status UIの読み込み
    for (int i = 0; i < PLAYER_STATUS_PIC_NUM; i++)
    {
        player.Status_UI_TextureId[i] = LoadTexture(player_status_pic[i].FILE_NAME);
    }

    player.size = MakeFloat2(PLAYER_SIZE_X, PLAYER_SIZE_Y);
    player.gravityAcc = GRAVITY;
    player.COLORSTATE = RED;

    ReloadPlayerStage();
}

// =========================================================
// プレイヤー更新
// =========================================================
void UpdatePlayer(void)
{
    g_frame++;
    if (!player.use) return;

    // ==========================================
    // SHOOT ENERGY BAR
    // ==========================================
    if (player.normalHitCnt < 0) player.normalHitCnt = 0;
    else if (player.normalHitCnt > SHOOT_ENERGY_MAX_CHARGE) player.normalHitCnt = SHOOT_ENERGY_MAX_CHARGE;
    g_DrawShootEnergy = LerpFloat(g_DrawShootEnergy, static_cast<float>(player.normalHitCnt), SHOOT_ENERGY_BAR_LERP);

    // ==========================================
    // SHADOW EFFECT
    // ==========================================
    UpdatePlayerShadow();

    // ==========================================
    // 復活後の無敵時間
    // ==========================================
    if (player.invincibleTimer > 0)
    {
        player.invincibleTimer--;

        float elapsedFrame = static_cast<float>(REVIVE_INVINCIBLE_FRAME - player.invincibleTimer);
        player.color.w = 0.65f + 0.35f * sinf(elapsedFrame * 0.3f);

        if (player.invincibleTimer <= 0)
        {
            player.invincibleTimer = 0;
            player.color.w = 1.0f;
        }
    }

    // ==========================================
    // Sprint Cool Down
    // ==========================================
    if (player.sprintCoolDownTimer > 0)
    {
        player.sprintCoolDownTimer--;
    }

    // ==========================================
    // HIT・DEAD・SPRINT以外は通常操作を行う
    // ==========================================
    if (player.state != PLAYER_STATE_HIT && player.state != PLAYER_STATE_DEAD && player.state != PLAYER_STATE_SPRINT)
    {
        // ==========================================
        // Sprint開始
        // ==========================================
        if (GetControllerTrigger(NpadButton::Y::Index) && player.sprintCoolDownTimer <= 0 &&
            player.state != PLAYER_STATE_HIT && player.state != PLAYER_STATE_CLING &&
            GetCurrentGameStage() != GAME_STAGE_T_01)
        {
            StartPlayerSprint();
            PlaySE(SE_Player_Dash);
            playerShadowTimer = 0;
            SetPlayerShadow();
        }

        if (player.state != PLAYER_STATE_SPRINT)
        {
            // ==========================================
            // カラーステート変化
            // ==========================================
            HandleColorChange();

            // ==========================================
            // ZL 照準状態の判定を優先
            // ==========================================
            if (GetControllerPress(NpadButton::ZL::Index) &&
                GetCurrentGameStage() != GAME_STAGE_T_01 &&
                GetCurrentGameStage() != GAME_STAGE_T_02)
            {
                player.state = PLAYER_STATE_AIM;
                player.isAiming = true;
                player.isWallCling = false;
                player.canWallJump = false;
                player.wallDir = 0;
                ApplyAimPhysics();
            }
            else
            {
                if (player.isAiming)
                {
                    ResetRightStickInput();
                }
                player.isAiming = false;
                g_DrawPreviewLeftLength = 0.0f;
                g_DrawPreviewRightLength = 0.0f;

                // ==========================================
                // 通常の移動と慣性の処理
                // ==========================================
                float targetSpeedX = HandleControl(1.0f);

                if (player.state != PLAYER_STATE_CLING)
                {
                    // 滑らかな速度補間 (Lerp)
                    if (targetSpeedX == 0.0f)
                    {
                        player.vel.x = LerpFloat(player.vel.x, targetSpeedX, MOVE_DAMP);
                    }
                    else
                    {
                        player.vel.x = LerpFloat(player.vel.x, targetSpeedX, MOVE_ACC);
                    }

                    if (targetSpeedX == 0.0f && player.vel.x > -0.1f && player.vel.x < 0.1f)
                    {
                        player.vel.x = 0.0f;
                    }
                }
            }

            // ==========================================
            // WallCling判定
            // ==========================================
            if (!player.isAiming && GetCurrentGameStage() != GAME_STAGE_T_01)
            {
                UpdateWallCling();
            }
        }
    }

    // ==========================================
    // 状態遷移 (StateMachine)
    // ==========================================
    switch (player.state)
    {
    case PLAYER_STATE_IDLE:   PlayerIdle();   break;
    case PLAYER_STATE_WALK:   PlayerWalk();   break;
    case PLAYER_STATE_JUMP:   PlayerJump();   break;
    case PLAYER_STATE_FALL:   PlayerFall();   break;
    case PLAYER_STATE_AIM:    PlayerAim();    break;
    case PLAYER_STATE_CLING:  PlayerCling();  break;
    case PLAYER_STATE_HIT:    PlayerHit();    break;
    case PLAYER_STATE_DEAD:   PlayerDead();   break;
    case PLAYER_STATE_SPRINT: PlayerSprint(); break;
    }

    // ==========================================
    // プレイヤー射撃処理
    // ==========================================
    if (player.state != PLAYER_STATE_HIT && player.state != PLAYER_STATE_DEAD && player.state != PLAYER_STATE_SPRINT)
    {
        UpdatePlayerShoot();
    }

    // ==========================================
    // 軸ごとの物理衝突処理
    // ==========================================

    // 足元の移動Blockの実際移動量をPlayer位置に適用
    ApplyGroundCCBlockMovement();

    player.vel.x += player.exVel.x;
    player.pos.x += player.vel.x;

    if (player.vel.x > 0.0f)
    {
        if (RightCollision() && player.state == PLAYER_STATE_SPRINT)
        {
            EndPlayerSprint();
        }
    }
    else if (player.vel.x < 0.0f)
    {
        if (LeftCollision() && player.state == PLAYER_STATE_SPRINT)
        {
            EndPlayerSprint();
        }
    }
    
    player.vel.y += player.exVel.y;
    player.pos.y += player.vel.y;

    if (player.vel.y > 0.0f)
    {
        DownCollision();
    }
    else if (player.vel.y < 0.0f)
    {
        UpCollision();
    }

    player.exVel = MakeFloat2(0.0f, 0.0f); //　加算後をリセット

    // Respawn Point更新
    UpdateRespawnPoint();

    // ==========================================
    // コヨーテタイムの更新
    // ==========================================
    if (CheckGrounded())
    {
        player.coyoteTimer = COYOTE_TIME_FRAMES;

        player.isWallCling = false;
        player.canWallJump = false;
        player.wallDir = 0;
    }
    else
    {
        if (player.coyoteTimer > 0) player.coyoteTimer--;

        // 非照準状態で足場から外れた場合のみ、強制的に落下状態へ移行
        if (!player.isAiming &&
            !player.isWallCling &&
            (player.state == PLAYER_STATE_IDLE || player.state == PLAYER_STATE_WALK))
        {
            player.state = PLAYER_STATE_FALL;
        }
    }

    // ==========================================
    // アイテムとの衝突処理 (スコア加算)
    // ==========================================
    ItemCollision();
}

// =========================================================
// プレイヤー描画
// =========================================================
void DrawPlayer(void)
{
    if (!GetGameTimeScaleZero())
    {
        g_anime_frame++;
        g_ui_frame++;
    }
    if (!player.use) return;

    DrawPlayerShadow();

    // キャラクターアニメーションの描画
    float tx{ 0.0f }, ty{ 0.0f }, tw{ 1.0f }, th{ 1.0f };
    int pic_id = player.state;

    // 配列外アクセス防止：現在の状態に対応するテクスチャがない場合、IDLEの画像で代用
    if (pic_id >= PLAYER_PIC_NUM) pic_id = PLAYER_STATE_IDLE;

    int color_id = player.COLORSTATE;
    if (color_id >= PLAYER_COLOR_NUM) color_id = RED;

    int speed = player_pic[pic_id][color_id].ANIME_SPEED;
    if (speed <= 0) speed = 1; // ゼロ除算の防止

    int frame = g_anime_frame / speed;
    if (player.state == PLAYER_STATE_DEAD)
    {
        if (frame >= player_pic[pic_id][color_id].PATTERN_MAX)
        {
            frame = player_pic[pic_id][color_id].PATTERN_MAX - 1;
        }
    }
    else
    {
        frame %= player_pic[pic_id][color_id].PATTERN_MAX;
    }
    tx = player_pic[pic_id][color_id].PATTERN_WIDTH * (frame % player_pic[pic_id][color_id].PATTERN_NUM_U);
    ty = player_pic[pic_id][color_id].PATTERN_HIGHT * (frame / player_pic[pic_id][color_id].PATTERN_NUM_U);
    tw = player_pic[pic_id][color_id].PATTERN_WIDTH;
    th = player_pic[pic_id][color_id].PATTERN_HIGHT;

    DrawSpriteAnimation_Scroll(
        player.pos.x, player.pos.y,
        player.size.x, player.size.y,
        player.color, player.rotation,
        tx, ty, tw, th,
        player.TextureId[pic_id][color_id], player.isFacingRight
    );

    // Wall Cling汗粒描画
    if (player.isWallCling && player.clingTimer > 0 && player.clingTimer < static_cast<int>(WALL_CLING_MAX_FRAME * WALL_CLING_SWEATY_RATIO))
    {
        int sweat_pic_id = 1;

        int sweat_speed = player_effect_pic[sweat_pic_id][color_id].ANIME_SPEED;
        if (sweat_speed <= 0) sweat_speed = 1; // ゼロ除算の防止

        int sweat_frame = g_anime_frame / sweat_speed;
        sweat_frame %= player_effect_pic[sweat_pic_id][color_id].PATTERN_MAX;

        float sweat_tx = player_effect_pic[sweat_pic_id][color_id].PATTERN_WIDTH * (sweat_frame % player_effect_pic[sweat_pic_id][color_id].PATTERN_NUM_U);
        float sweat_ty = player_effect_pic[sweat_pic_id][color_id].PATTERN_HIGHT * (sweat_frame / player_effect_pic[sweat_pic_id][color_id].PATTERN_NUM_U);
        float sweat_tw = player_effect_pic[sweat_pic_id][color_id].PATTERN_WIDTH;
        float sweat_th = player_effect_pic[sweat_pic_id][color_id].PATTERN_HIGHT;

        // Wall Clingの時間制限を使い切れそうなときの焦り表現
        DrawSpriteAnimation_Scroll(
            player.pos.x, player.pos.y,
            player.size.x, player.size.y,
            player.color, player.rotation,
            sweat_tx, sweat_ty, sweat_tw, sweat_th,
            player.Effect_TextureId[sweat_pic_id][color_id], player.isFacingRight
        );
    }

    // レーザー（レイキャスト）の描画 ーー＞ DrawGame()に出した
    // チャージのエフェクトの描画 ーー＞ DrawGame()に出した
}

// =========================================================
// プレイヤー終了処理
// =========================================================
void FinalizePlayer(void)
{
    for (int i = 0; i < PLAYER_PIC_NUM; i++)
    {
        for (int j = 0; j < PLAYER_COLOR_NUM; j++)
        {
            if (player.TextureId[i][j] != 0)
            {
                UnloadTexture(player.TextureId[i][j]);
                player.TextureId[i][j] = 0;
            }
        }
    }

    for (int i = 0; i < PLAYER_EFFECT_PIC_NUM; i++)
    {
        for (int j = 0; j < PLAYER_COLOR_NUM; j++)
        {
            if (player.Effect_TextureId[i][j] != 0)
            {
                UnloadTexture(player.Effect_TextureId[i][j]);
                player.Effect_TextureId[i][j] = 0;
            }
        }
    }

    for (int i = 0; i < PLAYER_COLOR_NUM; i++)
    {
        if (player.Shadow_TextureId[i] != 0)
        {
            UnloadTexture(player.Shadow_TextureId[i]);
            player.Shadow_TextureId[i] = 0;
        }
    }

    for (int i = 0; i < PLAYER_STATUS_PIC_NUM; i++)
    {
        if (player.Status_UI_TextureId[i] != 0)
        {
            UnloadTexture(player.Status_UI_TextureId[i]);
            player.Status_UI_TextureId[i] = 0;
        }
    }
}

// =========================================================
// 状態マシン 関数群
// =========================================================
void PlayerIdle()
{
    player.vel.y = 0.0f;

    if (player.vel.x > 0.1f || player.vel.x < -0.1f) {
        player.state = PLAYER_STATE_WALK;
    }

    if (player.coyoteTimer > 0 && GetControllerTrigger(NpadButton::B::Index)) {
        PlaySE(SE_Player_Jump);
        player.state = PLAYER_STATE_JUMP;
        player.vel.y = JUMP_POWER;
        player.coyoteTimer = 0;
    }
}

void PlayerWalk()
{
    player.vel.y = 0.0f;

    if (player.vel.x > -0.1f && player.vel.x < 0.1f) {
        player.state = PLAYER_STATE_IDLE;
    }

    if (player.coyoteTimer > 0 && GetControllerTrigger(NpadButton::B::Index)) {
        PlaySE(SE_Player_Jump);
        player.state = PLAYER_STATE_JUMP;
        player.vel.y = JUMP_POWER;
        player.coyoteTimer = 0;
    }
}

void PlayerJump()
{
    player.vel.y += player.gravityAcc;

    if (player.vel.y > 0.0f) {
        player.state = PLAYER_STATE_FALL;
    }
}

void PlayerFall()
{
    player.vel.y += player.gravityAcc;

    if (player.coyoteTimer > 0 && GetControllerTrigger(NpadButton::B::Index)) {
        PlaySE(SE_Player_Jump);
        player.state = PLAYER_STATE_JUMP;
        player.vel.y = JUMP_POWER;
        player.coyoteTimer = 0;
    }
}

void PlayerDead()
{
    // DEAD中も重力を適用しない
    player.vel.y = 0.0f;
    player.vel.x = 0.0f;

    int color_id = player.COLORSTATE;
    if (color_id >= PLAYER_COLOR_NUM) color_id = RED;

    int animeSpeed = player_pic[PLAYER_STATE_DEAD][color_id].ANIME_SPEED;
    if (animeSpeed <= 0) animeSpeed = 1;

    int deadPattern = g_anime_frame / animeSpeed;

    // 死亡処理開始
    if (deadPattern >= DEAD_FADE_START_PATTERN && GetFadeInGame()->state == FADE_NONE)
    {
        // Boss Stage
        if (GetCurrentGameStage() == GAME_STAGE_MAX) // Remember to change to boss stage
        {
            if (!GetDeathATM())
            {
                StartDeathATM();
            }
        }
        // 通常Stage
        else
        {
            StartFadeInGame();
        }
    }


    // 死亡アニメーションの最後のフレームでゲーム時間を停止する
    if (deadPattern >= player_pic[PLAYER_STATE_DEAD][color_id].PATTERN_MAX - 1)
    {
        SetGameTimeScaleZero(true);
    }
}

float HandleControl(float speed_rate)
{
    if (speed_rate < 0.0f) speed_rate = 0.0f;
    Float2 left_stick = GetControllerLeftStick();
    float targetSpeedX = 0.0f;

    if (GetControllerPress(NpadButton::Left::Index) || left_stick.x < -0.1f)
    {
        targetSpeedX = -MOVE_SPEED;
        if (left_stick.x < -0.1f) targetSpeedX = left_stick.x * MOVE_SPEED;
        player.isFacingRight = false;
    }
    else if (GetControllerPress(NpadButton::Right::Index) || left_stick.x > 0.1f)
    {
        targetSpeedX = MOVE_SPEED;
        if (left_stick.x > 0.1f) targetSpeedX = left_stick.x * MOVE_SPEED;
        player.isFacingRight = true;
    }

    return targetSpeedX * speed_rate;
}

void HandleColorChange()
{
    GAME_STAGE currentStage = GetCurrentGameStage();
    if (currentStage < GAME_STAGE_T_02) return;

    if (GetControllerTrigger(NpadButton::L::Index) || GetControllerTrigger(NpadButton::R::Index))
    {
        PlaySE(SE_Player_Switch_Color);

        if (player.COLORSTATE == COLOR_STATE::RED)
        {
            player.COLORSTATE = COLOR_STATE::BLUE;
        }
        else
        {
            player.COLORSTATE = COLOR_STATE::RED;
        }
    }
}

void PlayerAim()
{
    // 全Enemyの狙われている状態をリセット
    ResetAllEnemyIsAimed();
    // 全Enemy_Bulletの狙われている状態をリセット
    ResetAllEnemyBulletIsAimed();

    // ZLボタンを離した場合、照準状態を解除し適切な状態へ遷移
    if (!player.isAiming)
    {
        player.state = CheckGrounded() ? PLAYER_STATE_IDLE : PLAYER_STATE_FALL;
        return;
    }

    // 照準用のレイを計算
    Float2 right_stick = GetControllerRightStick();

    if ((right_stick.x > -0.1f && right_stick.x < 0.1f) && (right_stick.y > -0.1f && right_stick.y < 0.1f))
    { // RightStick入力がない場合、プレイヤーの向きに応じてデフォルト数値をいれる
        if (player.isFacingRight == true)
        {
            right_stick = MakeFloat2(1.0f, 0.0f);
        }
        else
        {
            right_stick = MakeFloat2(-1.0f, 0.0f);
        }
    }

    float rx = right_stick.x;
    float ry = -right_stick.y; // 座標系に合わせてY軸を反転

    player.aimAngle = atan2f(ry, rx);
    player.aimDir.x = cos(player.aimAngle);
    player.aimDir.y = sin(player.aimAngle);

    // 照準方向に合わせてプレイヤーの向きを変更
    if (player.aimDir.x > 0.1f)
    {
        player.isFacingRight = true;
    }
    else if (player.aimDir.x < -0.1f)
    {
        player.isFacingRight = false;
    }

    float closestHit = 2000.0f;

    // ブロック（マップ全体）
    BLOCK* block = GetBlock();
    int blockCount = GetBlockCount();
    for (int i = 0; i < blockCount; i++)
    {
        if (!block[i].use) continue;

        float hitDist = 0.0f;
        if (CheckRaycastBox(player.pos, player.aimDir, block[i].CollisionPosition, block[i].CollisionSize, hitDist))
        {
            if (hitDist > 0.0f && hitDist < closestHit)
            {
                closestHit = hitDist;
            }
        }
    }

    // Color Change Block
    COLOR_CHANGE_BLOCK* ccBlock = GetColorChangeBlock();
    for (int i = 0; i < CCBLOCK_MAX; i++)
    {
        if (!ccBlock[i].use) continue;

        float hitDist = 0.0f;

        if (CheckRaycastBox(player.pos, player.aimDir, ccBlock[i].CollisionPos, ccBlock[i].CollisionSize, hitDist))
        {
            if (hitDist > 0.0f && hitDist < closestHit)
            {
                closestHit = hitDist;
            }
        }
    }

    // Enemy
    Enemy* targetEnemy{};
    ENEMY_BULLET* targetEnemyBullet{};
    BOSS* targetBoss{};
    BOSS_AIM_PART targetBossPart = BOSS_AIM_NONE;

    // 近接キノコ
    Mushroom_Melee* mushroomMelee = GetMushroomMelee();
    for (int i = 0; i < MAX_ENEMY; i++)
    {
        if (!mushroomMelee[i].use) continue;
        if (mushroomMelee[i].state == ENEMY_STATE_DEAD) continue;

        float hitDist = 0.0f;
        if (CheckRaycastBox(player.pos, player.aimDir, mushroomMelee[i].CollisionPosition, mushroomMelee[i].CollisionSize, hitDist))
        {
            if (hitDist > 0.0f && hitDist < closestHit)
            {
                closestHit = hitDist;
                targetEnemy = &mushroomMelee[i];

                targetEnemyBullet = nullptr;
            }
        }
    }

    // 遠隔キノコ
    Mushroom_Range* mushroomRange = GetMushroomRange();
    for (int i = 0; i < MAX_ENEMY; i++)
    {
        if (!mushroomRange[i].use) continue;
        if (mushroomRange[i].state == ENEMY_STATE_DEAD) continue;

        float hitDist = 0.0f;
        if (CheckRaycastBox(player.pos, player.aimDir, mushroomRange[i].CollisionPosition, mushroomRange[i].CollisionSize, hitDist))
        {
            if (hitDist > 0.0f && hitDist < closestHit)
            {
                closestHit = hitDist;
                targetEnemy = &mushroomRange[i];

                targetEnemyBullet = nullptr;
            }
        }
    }

    // Enemy Bullet
    ENEMY_BULLET* enemyBullet = GetEnemyBullet();
    for (int i = 0; i < MAX_ENEMY_BULLET; i++)
    {
        if (!enemyBullet[i].use) continue;
        if (enemyBullet[i].isDestroying) continue;
        if (!enemyBullet[i].canBeDestroyed) continue;

        float hitDist = 0.0f;

        if (CheckRaycastBox(player.pos, player.aimDir, enemyBullet[i].CollisionPosition, enemyBullet[i].CollisionSize, hitDist))
        {
            if (hitDist > 0.0f && hitDist < closestHit)
            {
                closestHit = hitDist;
                targetEnemyBullet = &enemyBullet[i];

                // Enemy Bulletの方が近いため、Enemyを解除
                targetEnemy = nullptr;
            }
        }
    }

    // =========================================================
    // Boss
    // =========================================================
    BOSS* boss = GetBoss();

    if (boss != nullptr && boss->use &&
        boss->introDone && boss->state != BOSS_STATE_TELEPORT)
    {
        // 頭部
        if (boss->HeadCollisionSize.x > 0.0f && boss->HeadCollisionSize.y > 0.0f)
        {
            float hitDist = 0.0f;
            if (CheckRaycastBox(player.pos, player.aimDir, boss->HeadCollisionPosition, boss->HeadCollisionSize, hitDist))
            {
                if (hitDist > 0.0f && hitDist < closestHit)
                {
                    closestHit = hitDist;
                    targetBoss = boss;
                    targetBossPart = BOSS_AIM_HEAD;
                    targetEnemy = nullptr;
                    targetEnemyBullet = nullptr;
                }
            }
        }

        // 腹部Ruby
        if (boss->RubyCollisionSize.x > 0.0f && boss->RubyCollisionSize.y > 0.0f)
        {
            float hitDist = 0.0f;
            if (CheckRaycastBox(player.pos, player.aimDir, boss->RubyCollisionPosition, boss->RubyCollisionSize, hitDist))
            {
                if (hitDist > 0.0f && hitDist < closestHit)
                {
                    closestHit = hitDist;
                    targetBoss = boss;
                    targetBossPart = BOSS_AIM_RUBY;
                    targetEnemy = nullptr;
                    targetEnemyBullet = nullptr;
                }
            }
        }
    }

    // =========================================================
    // 補助照準
    // =========================================================
    player.rayDistance = closestHit;

    Float2 targetPosition{};
    bool hasAimTarget = false;

    if (targetEnemyBullet != nullptr)
    {
        targetEnemyBullet->isBeingAimed = true;
        targetPosition = targetEnemyBullet->CollisionPosition;
        hasAimTarget = true;
    }
    else if (targetBoss != nullptr)
    {
        targetBoss->isBeingAimed = true;
        targetBoss->aimedPart = targetBossPart;

        switch (targetBossPart)
        {
        case BOSS_AIM_HEAD:
            targetBoss->CollisionPosition = targetBoss->HeadCollisionPosition;
            targetBoss->CollisionSize = targetBoss->HeadCollisionSize;
            targetPosition = targetBoss->HeadCollisionPosition;
            hasAimTarget = true;
            break;

        case BOSS_AIM_RUBY:
            targetBoss->CollisionPosition = targetBoss->RubyCollisionPosition;
            targetBoss->CollisionSize = targetBoss->RubyCollisionSize;
            targetPosition = targetBoss->RubyCollisionPosition;
            hasAimTarget = true;
            break;

        default:
            targetBoss->isBeingAimed = false;
            targetBoss->aimedPart = BOSS_AIM_NONE;
            break;
        }
    }
    else if (targetEnemy != nullptr)
    {
        targetEnemy->isBeingAimed = true;
        targetPosition = targetEnemy->CollisionPosition;
        hasAimTarget = true;
    }

    if (hasAimTarget)
    {
        Float2 vectorToTarget = MakeFloat2(
            targetPosition.x - player.pos.x,
            targetPosition.y - player.pos.y
        );

        float magnitude = sqrtf(
            vectorToTarget.x * vectorToTarget.x +
            vectorToTarget.y * vectorToTarget.y
        );

        if (magnitude > 0.000001f)
        {
            player.aimDir = MakeFloat2(
                vectorToTarget.x / magnitude,
                vectorToTarget.y / magnitude
            );

            player.aimAngle = atan2f(vectorToTarget.y, vectorToTarget.x);
            player.rayDistance = magnitude;

            if (player.aimDir.x > 0.1f) player.isFacingRight = true;
            else if (player.aimDir.x < -0.1f) player.isFacingRight = false;
        }
    }

    // 最終的なレイの着弾地点を更新
    player.rayHitPos = MakeFloat2(
        player.pos.x + player.aimDir.x * player.rayDistance,
        player.pos.y + player.aimDir.y * player.rayDistance
    );
}

void ApplyAimPhysics()
{
    // Aim中は横移動を制限する
    float targetSpeedX = HandleControl(AIM_HORIZON_SPEED_RATE);

    // 滑らかな速度補間 (Lerp)
    if (targetSpeedX == 0.0f) {
        player.vel.x *= AIM_HORIZON_DAMP;
    }
    else {
        player.vel.x = LerpFloat(player.vel.x, targetSpeedX, AIM_HORIZON_ACC);
    }

    // 上昇中だけ、ジャンプ初速をAim用の最大上昇速度に制限する
    if (player.vel.y < 0.0f)
    {
        float aimMaxUpSpeed = JUMP_POWER * AIM_MAX_UP_SPEED_RATE;

        // JUMP_POWERは負数なので、より小さい値の方が速い上昇
        if (player.vel.y < aimMaxUpSpeed)
        {
            player.vel.y = aimMaxUpSpeed;
        }

        // 上昇速度を徐々に減衰させる
        player.vel.y *= AIM_UPWARD_DAMP;
    }
    else if (player.vel.y > 0.0f)
    {
        // Aim開始前に蓄積した落下速度を減衰させる
        player.vel.y *= AIM_DOWNWARD_DAMP;
    }

    // Aim中の低重力
    player.vel.y += player.gravityAcc * AIM_GRAVITY_SCALE;
}

void PlayerCling()
{
    if (player.clingTimer > 0) player.clingTimer--;

    if (!player.isWallCling)
    {
        player.state = PLAYER_STATE_FALL;
        return;
    }

    player.vel.x = 0.0f;

    if (player.wallDir > 0)
    {
        player.isFacingRight = false;
    }
    else if (player.wallDir < 0)
    {
        player.isFacingRight = true;
    }

    if (CheckGrounded() != true)
    {
        player.canWallJump = true;
    }

    if (GetControllerTrigger(NpadButton::B::Index) && player.canWallJump)
    {
        player.vel.y = JUMP_POWER * WALL_JUMP_Y_BOOST;
        player.vel.x = -player.wallDir * WALL_JUMP_X_POWER;

        player.isWallCling = false;
        player.canWallJump = false;
        player.wallDir = 0;

        player.clingTimer = 0;
        player.wallJumpCoyoteTimer = 0;

        if (player.vel.x > 0.0f)
        {
            player.isFacingRight = true;
        }
        else if (player.vel.x < 0.0f)
        {
            player.isFacingRight = false;
        }

        PlaySE(SE_Player_Jump);
        player.state = PLAYER_STATE_JUMP;
        return;
    }

    player.vel.y += player.gravityAcc * WALL_CLING_GRAVITY_SCALE;

    if (player.vel.y > WALL_CLING_MAX_FALL_SPEED)
    {
        player.vel.y = WALL_CLING_MAX_FALL_SPEED;
    }
}

// =========================================================
// WallCling
// =========================================================
void UpdateWallCling()
{
    if (CheckGrounded())
    {
        player.isWallCling = false;
        player.canWallJump = false;
        player.wallDir = 0;
        player.clingTimer = 0;
        player.wallJumpCoyoteTimer = 0;

        if (player.state == PLAYER_STATE_CLING)
        {
            player.state = PLAYER_STATE_IDLE;
        }

        return;
    }

    if (player.state == PLAYER_STATE_AIM)
    {
        player.isWallCling = false;
        player.canWallJump = false;
        player.wallDir = 0;
        player.wallJumpCoyoteTimer = 0;
        return;
    }

    if (player.vel.y < 0.0f && player.state != PLAYER_STATE_CLING)
    {
        player.isWallCling = false;
        player.canWallJump = false;
        player.wallDir = 0;
        player.wallJumpCoyoteTimer = 0;
        return;
    }

    bool touchingLeftWall = IsTouchingWall(-1);
    bool touchingRightWall = IsTouchingWall(1);

    if (touchingLeftWall && IsPressingToWall(-1))
    {
        if (!player.isWallCling)
        {
            player.clingTimer = static_cast<int>(WALL_CLING_MAX_FRAME);
        }

        player.isWallCling = true;
        player.canWallJump = true;
        player.wallDir = -1;
        player.wallJumpCoyoteTimer = WALL_JUMP_COYOTE_TIME_FRAMES;
        player.state = PLAYER_STATE_CLING;
        return;
    }

    if (touchingRightWall && IsPressingToWall(1))
    {
        if (!player.isWallCling)
        {
            player.clingTimer = static_cast<int>(WALL_CLING_MAX_FRAME);
        }

        player.isWallCling = true;
        player.canWallJump = true;
        player.wallDir = 1;
        player.wallJumpCoyoteTimer = WALL_JUMP_COYOTE_TIME_FRAMES;
        player.state = PLAYER_STATE_CLING;
        return;
    }

    // 壁方向の入力を離しても、短時間はCling状態を維持する
    if (player.state == PLAYER_STATE_CLING && player.wallJumpCoyoteTimer > 0)
    {
        player.wallJumpCoyoteTimer--;
        player.isWallCling = true;
        player.canWallJump = true;
        return;
    }

    player.isWallCling = false;
    player.canWallJump = false;
    player.wallDir = 0;
    player.clingTimer = 0;
    player.wallJumpCoyoteTimer = 0;

    if (player.state == PLAYER_STATE_CLING)
    {
        player.state = PLAYER_STATE_FALL;
    }
}

bool IsTouchingWall(int wallDir)
{
    float checkX = player.pos.x;

    if (wallDir < 0)
    {
        checkX = player.CollisionPosition.x - (PLAYER_COLLISION_SIZE_W / 2.0f) + WALL_CHECK_OFFSET;
    }
    else if (wallDir > 0)
    {
        checkX = player.CollisionPosition.x + (PLAYER_COLLISION_SIZE_W / 2.0f) - WALL_CHECK_OFFSET;
    }
    else
    {
        return false;
    }

    Float2 checkPos = MakeFloat2(checkX, player.pos.y + (PLAYER_COLLISION_SIZE_W / 2.0f) - WALL_CHECK_HEIGHT);
    Float2 checkSize = MakeFloat2(WALL_CHECK_WIDTH, WALL_CHECK_HEIGHT);

    BLOCK* block = GetBlock();
    int blockCount = GetBlockCount();
    for (int i = 0; i < blockCount; i++)
    {
        if (block[i].use != true) continue;

        if (CheckBoxCollider(
            checkPos, block[i].CollisionPosition,
            checkSize, block[i].CollisionSize))
        {
            return true;
        }
    }

    COLOR_CHANGE_BLOCK* ccBlock = GetColorChangeBlock();
    for (int i = 0; i < CCBLOCK_MAX; i++)
    {
        if (!CanPlayerCollideColorChangeBlock(&ccBlock[i])) continue;

        if (CheckBoxCollider(
            checkPos, ccBlock[i].CollisionPos,
            checkSize, ccBlock[i].CollisionSize) && 
            ccBlock[i].moveType != MOVE_TYPE::MOVING)
        {
            return true;
        }
    }

    return false;
}

bool IsPressingToWall(int wallDir)
{
    Float2 left_stick = GetControllerLeftStick();

    if (wallDir < 0)
    {
        return GetControllerPress(NpadButton::Left::Index) || left_stick.x < -0.1f;
    }

    if (wallDir > 0)
    {
        return GetControllerPress(NpadButton::Right::Index) || left_stick.x > 0.1f;
    }

    return false;
}

// =========================================================
// プレイヤー射撃処理
// =========================================================
void UpdatePlayerShoot()
{
    bool isZRPressed = GetControllerPress(NpadButton::ZR::Index);

    // Aim状態以外ではチャージをキャンセルする
    if (!player.isAiming)
    {
        g_ShootChargeFrame = 0;
        g_ChargeEffectFrame = 0;
        g_WasZRPressed = false;
        g_IsHalfChargeSEPlayed = false;
        g_IsFullChargeSEPlayed = false;
        return;
    }

    // ZRを新しく押した瞬間
    if (isZRPressed && !g_WasZRPressed)
    {
        g_ShootChargeFrame = 0;
        g_ChargeEffectFrame = 0;
        g_IsHalfChargeSEPlayed = false;
        g_IsFullChargeSEPlayed = false;
    }

    // ZRを押している間はチャージする
    if (isZRPressed)
    {
        if (g_ShootChargeFrame < SHOOT_CHARGE_MAX_FRAME) g_ShootChargeFrame++;

        if (g_ShootChargeFrame > SHOOT_CHARGE_START_FRAME && player.normalHitCnt >= SHOOT_ENERGY_MAX_CHARGE / 2) g_ChargeEffectFrame++;

        // 5Energyを使用できる段階で一度だけ再生
        if (!g_IsHalfChargeSEPlayed &&
            player.normalHitCnt >= SHOOT_ENERGY_MAX_CHARGE / 2 &&
            g_ShootChargeFrame >= SHOOT_CHARGE_HALF_FRAME)
        {
            PlaySE(SE_Player_Charged);
            g_IsHalfChargeSEPlayed = true;
        }

        // 10Energyを使用できる段階でもう一度だけ再生
        if (!g_IsFullChargeSEPlayed &&
            player.normalHitCnt >= SHOOT_ENERGY_MAX_CHARGE &&
            g_ShootChargeFrame >= SHOOT_CHARGE_MAX_FRAME)
        {
            PlaySE(SE_Player_Charged);
            g_IsFullChargeSEPlayed = true;
        }
    }
    // 前フレームまで押していて、現在離している場合に発射
    else if (g_WasZRPressed)
    {
        // Determine charge before consuming energy
        float chargeRate = GetShootChargeRate();
        float bulletScale = SHOOT_NORMAL_SCALE;
        float recoilForce = BULLET_NORMAL_RECOIL;
        int bulletPicType = BULLET_PIC_NORMAL;

        if (chargeRate >= 1.0f)
        {
            bulletScale = SHOOT_MAX_SCALE;
            recoilForce = BULLET_CHARGE_MAX_RECOIL;
            bulletPicType = BULLET_PIC_CHARGE;
        }
        else if (chargeRate >= 0.5f)
        {
            bulletScale = SHOOT_MIN_SCALE;
            recoilForce = BULLET_CHARGE_MIN_RECOIL;
            bulletPicType = BULLET_PIC_CHARGE;
        }

        int bulletColorType =
            player.COLORSTATE == COLOR_STATE::RED ?
            BULLET_COLOR_RED :
            BULLET_COLOR_BLUE;

        Float2 shootPosition = MakeFloat2(
            player.pos.x + player.aimDir.x * SHOOT_START_OFFSET,
            player.pos.y + player.aimDir.y * SHOOT_START_OFFSET
        );

        SetBullet(
            shootPosition,
            player.aimDir,
            bulletScale,
            bulletPicType,
            bulletColorType
        );

        // Sound
        if (bulletPicType == BULLET_PIC_CHARGE)
        {
            PlaySE(SE_Charge_Shoot);
        }
        else
        {
            PlaySE(SE_Normal_Shoot);
        }

        // Consume energy after charge result is fixed
        if (chargeRate >= 1.0f) player.normalHitCnt -= SHOOT_ENERGY_MAX_CHARGE;
        else if (chargeRate >= 0.5f) player.normalHitCnt -= SHOOT_ENERGY_MAX_CHARGE / 2;

        if (player.normalHitCnt < 0) player.normalHitCnt = 0;

        float recoilForceX = recoilForce;
        float recoilForceY = recoilForce;

        // Reduce downward recoil when shooting upward while falling
        if (player.vel.y >= 0.0f && player.aimDir.y < 0.0f)
        {
            recoilForceY *= SHOOT_DOWN_RECOIL_RATE;
        }

        player.exVel = MakeFloat2(
            player.exVel.x + (-player.aimDir.x * recoilForceX),
            player.exVel.y + (-player.aimDir.y * recoilForceY)
        );

        g_ShootChargeFrame = 0;
        g_ChargeEffectFrame = 0;
        g_IsHalfChargeSEPlayed = false;
        g_IsFullChargeSEPlayed = false;
    }

    // ZRを離した時に次のCharge用SE状態をReset
    if (!isZRPressed)
    {
        g_IsHalfChargeSEPlayed = false;
        g_IsFullChargeSEPlayed = false;
    }

    g_WasZRPressed = isZRPressed;
}
// =========================================================
// Charge段階を取得
// =========================================================
float GetShootChargeRate()
{
    // 最大Energyかつ最大FrameまでChargeした場合
    if (player.normalHitCnt >= SHOOT_ENERGY_MAX_CHARGE &&
        g_ShootChargeFrame >= SHOOT_CHARGE_MAX_FRAME)
    {
        return 1.0f;
    }

    // 半分以上のEnergyかつHalf FrameまでChargeした場合
    if (player.normalHitCnt >= SHOOT_ENERGY_MAX_CHARGE / 2 &&
        g_ShootChargeFrame >= SHOOT_CHARGE_HALF_FRAME)
    {
        return 0.5f;
    }

    // 必要FrameまでChargeできていない
    return 0.0f;
}

// =========================================================
// 照準レー描画
// =========================================================
void DrawAimLine()
{
    if (!player.isAiming) return;

    // 描画の中心点はプレイヤー座標から方向ベクトル×距離の半分だけオフセットさせる
    float drawX = player.pos.x + player.aimDir.x * (player.rayDistance / 2.0f + 75.0f);
    float drawY = player.pos.y + player.aimDir.y * (player.rayDistance / 2.0f + 75.0f);

    // エイムライン描画
    Float4 aimLineColor{}; // ラインの色
    if (player.COLORSTATE == COLOR_STATE::RED) aimLineColor = MakeFloat4(1.0f, 0.45f, 0.45f, 0.8f);
    else aimLineColor = MakeFloat4(0.45f, 0.75f, 1.0f, 1.0f);

    DrawSpriteQuad_Scroll(
        drawX, drawY,
        player.rayDistance - 150.0f,
        3.0f, // レーザーの太さ
        aimLineColor,
        player.aimAngle,
        0, false // 単色を使用 (TextureID = 0)、反転なし
    );
}

// =========================================================
// チャージエフェクト
// =========================================================
void DrawChargeEffect()
{
    // チャージのエフェクトを出す
    if (!player.isAiming) return;
    if (player.normalHitCnt < SHOOT_ENERGY_MAX_CHARGE / 2) return;
    if (g_ShootChargeFrame <= SHOOT_CHARGE_START_FRAME) return;// チャージ開始

    int color_id = player.COLORSTATE;
    if (color_id >= PLAYER_COLOR_NUM) color_id = RED;

    float tx{ 0.0f }, ty{ 0.0f }, tw{ 1.0f }, th{ 1.0f };

    float effect_drawX = player.pos.x + player.aimDir.x * 150.0f;
    float effect_drawY = player.pos.y + player.aimDir.y * 150.0f;
    float scale_rate = CHARGE_VFX_MIN_SCALE_RATIO + g_ChargeEffectFrame * CHARGE_VFX_CHANGE_RATIO;
    if (scale_rate > CHARGE_VFX_MAX_SCALE_RATIO) scale_rate = CHARGE_VFX_MAX_SCALE_RATIO;
    int charge_effect_pic_id = 0; // Reference from animation.h

    int effect_speed = player_effect_pic[charge_effect_pic_id][color_id].ANIME_SPEED;
    if (effect_speed <= 0) effect_speed = 1; // ゼロ除算の防止

    int frame = (g_ChargeEffectFrame / effect_speed) % player_effect_pic[charge_effect_pic_id][color_id].PATTERN_MAX;
    tx = player_effect_pic[charge_effect_pic_id][color_id].PATTERN_WIDTH * (frame % player_effect_pic[charge_effect_pic_id][color_id].PATTERN_NUM_U);
    ty = player_effect_pic[charge_effect_pic_id][color_id].PATTERN_HIGHT * (frame / player_effect_pic[charge_effect_pic_id][color_id].PATTERN_NUM_U);
    tw = player_effect_pic[charge_effect_pic_id][color_id].PATTERN_WIDTH;
    th = player_effect_pic[charge_effect_pic_id][color_id].PATTERN_HIGHT;

    DrawSpriteAnimation_Scroll(
        effect_drawX, effect_drawY,
        128.0f * scale_rate, 128.0f * scale_rate,
        MakeFloat4(1.0f, 1.0f, 1.0f, 0.9f),
        player.aimAngle,
        tx, ty, tw, th,
        player.Effect_TextureId[charge_effect_pic_id][color_id],
        player.isFacingRight
    );
}

// =========================================================
// Sprint開始
// =========================================================
void StartPlayerSprint()
{
    player.state = PLAYER_STATE_SPRINT;
    player.sprintTimer = SPRINT_FRAME;
    player.invincibleTimer = SPRINT_FRAME;

    // Aim・WallClingを解除する
    player.isAiming = false;
    player.isWallCling = false;
    player.canWallJump = false;
    player.wallDir = 0;

    // チャージ状態をリセットする
    g_ShootChargeFrame = 0;
    g_ChargeEffectFrame = 0;
    g_WasZRPressed = false;
    g_IsHalfChargeSEPlayed = false;
    g_IsFullChargeSEPlayed = false;

    // Sprintアニメーションを先頭から再生する
    g_anime_frame = 0;

    // 現在向いている方向へSprintする
    player.vel.x = player.isFacingRight ? SPRINT_SPEED : -SPRINT_SPEED;
}

// =========================================================
// Sprint
// =========================================================
void PlayerSprint()
{
    player.sprintTimer--;
    // Sprint中は向いている方向へ一定速度で移動する
    player.vel.x = player.isFacingRight ? SPRINT_SPEED : -SPRINT_SPEED;
    // 縦方向は通常の重力を適用しない
    player.vel.y = 0;

    playerShadowTimer++;
    if (playerShadowTimer >= PLAYER_SHADOW_INTERVAL)
    {
        playerShadowTimer = 0;
        SetPlayerShadow();
    }

    if (player.sprintTimer <= 0)
    {
        EndPlayerSprint();
    }
}

// =========================================================
// Sprint終了
// =========================================================
void EndPlayerSprint()
{
    player.sprintTimer = 0;
    player.sprintCoolDownTimer = SPRINT_COOL_DOWN_FRAME;

    // Sprint速度を停止する
    player.vel.x = 0.0f;

    playerShadowTimer = 0;

    // 接地状態に応じて通常状態へ戻す
    player.state = CheckGrounded() ? PLAYER_STATE_IDLE : PLAYER_STATE_FALL;

    g_anime_frame = 0;
}

// =========================================================
// Sprint Cool Down Bar描画
// =========================================================
void DrawSprintCoolDownBar()
{
    if (player.sprintCoolDownTimer <= 0) return;
    const float bar_left_start_x = player.pos.x - SPRINT_BAR_LENGTH / 2.0f;
    const float height = 8.0f;
    const Float4 bg_color  = MakeFloat4(0.18f, 0.25f, 0.30f, 0.85f);
    const Float4 bar_color = MakeFloat4(0.45f, 0.90f, 1.00f, 1.00f);

    // Barの背景描画
    DrawSpriteQuad_Scroll(
        player.pos.x, player.pos.y + SPRINT_BAR_Y_OFFSET,
        SPRINT_BAR_LENGTH, height,
        bg_color,
        0, false
    );

    // Sprintの残りCoolDown Bar描画
    float ratio = static_cast<float>(player.sprintCoolDownTimer) / static_cast<float>(SPRINT_COOL_DOWN_FRAME);
    float barLength = SPRINT_BAR_LENGTH * ratio;
    float barCenterX = bar_left_start_x + barLength / 2.0f;
    DrawSpriteQuad_Scroll(
        barCenterX, player.pos.y + SPRINT_BAR_Y_OFFSET,
        barLength, height,
        bar_color,
        0, false
    );
}

// =========================================================
// Sprint Shadow生成
// =========================================================
void SetPlayerShadow()
{
    int colorId = player.COLORSTATE;
    if (colorId >= PLAYER_COLOR_NUM) colorId = RED;

    int animeSpeed = player_pic[PLAYER_STATE_SPRINT][colorId].ANIME_SPEED;
    if (animeSpeed <= 0) animeSpeed = 1;

    for (int i = 0; i < PLAYER_SHADOW_MAX; i++)
    {
        if (playerShadow[i].use) continue;

        playerShadow[i].pos = player.pos;
        playerShadow[i].alpha = PLAYER_SHADOW_START_ALPHA;
        playerShadow[i].frame = (g_anime_frame / animeSpeed) % player_sprint_shadow[colorId].PATTERN_MAX;;
        playerShadow[i].colorId = colorId;
        playerShadow[i].isFacingRight = player.isFacingRight;
        playerShadow[i].use = true;
        return;
    }
}

// =========================================================
// Sprint Shadow更新
// =========================================================
void UpdatePlayerShadow()
{
    for (int i = 0; i < PLAYER_SHADOW_MAX; i++)
    {
        if (!playerShadow[i].use) continue;

        playerShadow[i].alpha -= PLAYER_SHADOW_FADE;

        if (playerShadow[i].alpha <= 0.0f)
        {
            playerShadow[i].alpha = 0.0f;
            playerShadow[i].use = false;
        }
    }
}

// =========================================================
// Sprint Shadow描画
// =========================================================
void DrawPlayerShadow()
{
    for (int i = 0; i < PLAYER_SHADOW_MAX; i++)
    {
        if (!playerShadow[i].use) continue;

        int colorId = playerShadow[i].colorId;
        int frame = playerShadow[i].frame;

        float tx = player_sprint_shadow[colorId].PATTERN_WIDTH * (frame % player_sprint_shadow[colorId].PATTERN_NUM_U);
        float ty = player_sprint_shadow[colorId].PATTERN_HIGHT * (frame / player_sprint_shadow[colorId].PATTERN_NUM_U);

        DrawSpriteAnimation_Scroll(
            playerShadow[i].pos.x,
            playerShadow[i].pos.y,
            player.size.x,
            player.size.y,
            MakeFloat4(1.0f, 1.0f, 1.0f, playerShadow[i].alpha),
            player.rotation,
            tx,
            ty,
            player_sprint_shadow[colorId].PATTERN_WIDTH,
            player_sprint_shadow[colorId].PATTERN_HIGHT,
            player.Shadow_TextureId[colorId],
            playerShadow[i].isFacingRight
        );
    }
}

// =========================================================
// 当たり判定
// =========================================================
bool RightCollision()
{
    player.CollisionPosition = MakeFloat2(player.pos.x + (PLAYER_COLLISION_SIZE_W / 2) - 5.0f, player.pos.y);
    player.CollisionSize = MakeFloat2(10.0f, 150.0f);

    bool ret;
    BLOCK* block = GetBlock();
    int blockCount = GetBlockCount();
    for (int i = 0; i < blockCount; i++)
    {
        if (block[i].use != true) continue;

        ret = CheckBoxCollider(
            player.CollisionPosition, block[i].CollisionPosition,
            player.CollisionSize, block[i].CollisionSize
        );

        if (ret == true)
        {//衝突してる

            if (player.vel.x >= 0.0f)
            {
                player.vel.x = 0.0f;

                //座標の補正
                player.pos.x = block[i].pos.x - ((MAP_BLOCK_WIDTH / 2) + (PLAYER_COLLISION_SIZE_W / 2));
                player.CollisionPosition.x = player.pos.x;
                return true;
            }
        }
    }

    COLOR_CHANGE_BLOCK* ccBlock = GetColorChangeBlock();
    for (int i = 0; i < CCBLOCK_MAX; i++)
    {
        if (!CanPlayerCollideColorChangeBlock(&ccBlock[i])) continue;

        if (CheckBoxCollider(player.CollisionPosition, ccBlock[i].CollisionPos, player.CollisionSize, ccBlock[i].CollisionSize))
        {
            if (player.vel.x >= 0.0f)
            {
                player.vel.x = 0.0f;
                player.pos.x = ccBlock[i].CollisionPos.x - (ccBlock[i].CollisionSize.x / 2.0f + PLAYER_COLLISION_SIZE_W / 2.0f);
                player.CollisionPosition.x = player.pos.x;
                return true;
            }
        }
    }

    return false;
}

bool LeftCollision()
{
    player.CollisionPosition = MakeFloat2(player.pos.x - (PLAYER_COLLISION_SIZE_W / 2) + 5.0f, player.pos.y);
    player.CollisionSize = MakeFloat2(10.0f, 150.0f);

    bool ret;
    BLOCK* block = GetBlock();
    int blockCount = GetBlockCount();
    for (int i = 0; i < blockCount; i++)
    {
        if (block[i].use != true) continue;

        ret = CheckBoxCollider(
            player.CollisionPosition, block[i].CollisionPosition,
            player.CollisionSize, block[i].CollisionSize
        );

        if (ret == true)
        {//衝突してる

            //速度は左へ向くときだけ
            if (player.vel.x <= 0.0f)
            {
                player.vel.x = 0.0f;

                //座標の補正
                player.pos.x = block[i].pos.x + ((MAP_BLOCK_WIDTH / 2) + (PLAYER_COLLISION_SIZE_W / 2));
                player.CollisionPosition.x = player.pos.x;
                return true;
            }
        }
    }

    COLOR_CHANGE_BLOCK* ccBlock = GetColorChangeBlock();
    for (int i = 0; i < CCBLOCK_MAX; i++)
    {
        if (!CanPlayerCollideColorChangeBlock(&ccBlock[i])) continue;

        if (CheckBoxCollider(player.CollisionPosition, ccBlock[i].CollisionPos, player.CollisionSize, ccBlock[i].CollisionSize))
        {
            if (player.vel.x <= 0.0f)
            {
                player.vel.x = 0.0f;
                player.pos.x = ccBlock[i].CollisionPos.x + (ccBlock[i].CollisionSize.x / 2.0f + PLAYER_COLLISION_SIZE_W / 2.0f);
                player.CollisionPosition.x = player.pos.x;
                return true;
            }
        }
    }

    return false;
}

bool UpCollision()
{
    player.CollisionPosition = MakeFloat2(player.pos.x, player.pos.y - (PLAYER_COLLISION_SIZE_H / 2) + 25.0f);
    player.CollisionSize = MakeFloat2(50.0f, 10.0f);

    bool ret;
    BLOCK* block = GetBlock();
    int blockCount = GetBlockCount();
    for (int i = 0; i < blockCount; i++)
    {
        if (block[i].use != true) continue;

        ret = CheckBoxCollider(
            player.CollisionPosition, block[i].CollisionPosition,
            player.CollisionSize, block[i].CollisionSize
        );

        if (ret == true)
        {//衝突してる

            if (player.vel.y <= 0.0f)
            {
                player.vel.y = 0.0f;

                //座標の補正
                player.pos.y = block[i].pos.y + ((MAP_BLOCK_HEIGHT / 2) + (PLAYER_COLLISION_SIZE_H / 2));
                player.CollisionPosition.y = player.pos.y;

                //ぶつかったので、落下状態になる
                if (player.state != PLAYER_STATE_HIT && player.state != PLAYER_STATE_DEAD)
                {
                    player.state = PLAYER_STATE_FALL;
                }
                return true;
            }
        }
    }

    COLOR_CHANGE_BLOCK* ccBlock = GetColorChangeBlock();
    for (int i = 0; i < CCBLOCK_MAX; i++)
    {
        if (!CanPlayerCollideColorChangeBlock(&ccBlock[i])) continue;

        if (CheckBoxCollider(player.CollisionPosition, ccBlock[i].CollisionPos, player.CollisionSize, ccBlock[i].CollisionSize))
        {
            if (player.vel.y <= 0.0f)
            {
                player.vel.y = 0.0f;
                player.pos.y = ccBlock[i].CollisionPos.y + (ccBlock[i].CollisionSize.y / 2.0f + PLAYER_COLLISION_SIZE_H / 2.0f);
                player.CollisionPosition.y = player.pos.y;

                if (player.state != PLAYER_STATE_HIT && player.state != PLAYER_STATE_DEAD)
                {
                    player.state = PLAYER_STATE_FALL;
                }

                return true;
            }
        }
    }

    return false;
}

bool DownCollision()
{
    player.CollisionPosition = MakeFloat2(player.pos.x, player.pos.y + (PLAYER_COLLISION_SIZE_H / 2) - 5.0f);
    player.CollisionSize = MakeFloat2(50.0f, 10.0f);

    bool ret;
    BLOCK* block = GetBlock();
    int blockCount = GetBlockCount();
    for (int i = 0; i < blockCount; i++)
    {
        if (block[i].use != true) continue;

        ret = CheckBoxCollider(
            player.CollisionPosition, block[i].CollisionPosition,
            player.CollisionSize, block[i].CollisionSize
        );

        if (ret == true)
        {//衝突してる

            if (player.vel.y >= 0.0f)
            {
                player.vel.y = 0.0f;

                //座標の補正
                player.pos.y = block[i].pos.y - ((MAP_BLOCK_HEIGHT / 2) + (PLAYER_COLLISION_SIZE_H / 2));
                player.CollisionPosition.y = player.pos.y;

                player.isWallCling = false;
                player.canWallJump = false;
                player.wallDir = 0;

                //下の物体とぶつかったので、落下状態から移動状態になる
                if (player.state == PLAYER_STATE_FALL || player.state == PLAYER_STATE_CLING)
                {
                    player.state = (player.vel.x > 0.1f || player.vel.x < -0.1f) ? PLAYER_STATE_WALK : PLAYER_STATE_IDLE;
                }

                return true;
            }
        }
    }

    COLOR_CHANGE_BLOCK* ccBlock = GetColorChangeBlock();
    for (int i = 0; i < CCBLOCK_MAX; i++)
    {
        if (!CanPlayerCollideColorChangeBlock(&ccBlock[i])) continue;

        if (CheckBoxCollider(player.CollisionPosition, ccBlock[i].CollisionPos, player.CollisionSize, ccBlock[i].CollisionSize))
        {
            if (player.vel.y >= 0.0f)
            {
                player.vel.y = 0.0f;
                player.pos.y = ccBlock[i].CollisionPos.y - (ccBlock[i].CollisionSize.y / 2.0f + PLAYER_COLLISION_SIZE_H / 2.0f);
                player.CollisionPosition.y = player.pos.y;
                player.isWallCling = false;
                player.canWallJump = false;
                player.wallDir = 0;

                if (player.state == PLAYER_STATE_FALL || player.state == PLAYER_STATE_CLING)
                {
                    player.state = (player.vel.x > 0.1f || player.vel.x < -0.1f) ? PLAYER_STATE_WALK : PLAYER_STATE_IDLE;
                }

                return true;
            }
        }
    }

    return false;
}

bool ItemCollision()
{
    player.CollisionPosition = MakeFloat2(player.pos.x, player.pos.y);
    player.CollisionSize = MakeFloat2(PLAYER_COLLISION_SIZE_W, PLAYER_COLLISION_SIZE_H);

    bool ret = false;
    ITEM* item = GetItem();
    for (int i = 0; i < MAX_ITEM; i++)
    {
        if (item[i].use == false) continue;

        ret = CheckBoxCollider(
            player.CollisionPosition, item[i].pos,
            player.CollisionSize, item[i].size
        );

        if (ret == true)
        {
            //sound
            //スコア加算
            AddScore(5);

            //アイテム取得エフェクト
            item[i].use = false;
            item[i].erased = true;
            return true;
        }
    }

    return false;
}

bool CheckGrounded()
{
    Float2 checkPos = MakeFloat2(player.pos.x, player.pos.y + (PLAYER_COLLISION_SIZE_H / 2) - 4.0f);
    Float2 checkSize = MakeFloat2(PLAYER_COLLISION_SIZE_W * 0.8f, 10.0f);

    bool ret;
    BLOCK* block = GetBlock();
    int blockCount = GetBlockCount();
    for (int i = 0; i < blockCount; i++)
    {
        if (block[i].use != true) continue;
        ret = CheckBoxCollider(checkPos, block[i].CollisionPosition, checkSize, block[i].CollisionSize);

        if (ret) return true;
    }

    COLOR_CHANGE_BLOCK* ccBlock = GetColorChangeBlock();
    for (int i = 0; i < CCBLOCK_MAX; i++)
    {
        if (!CanPlayerCollideColorChangeBlock(&ccBlock[i])) continue;

        if (CheckBoxCollider(checkPos, ccBlock[i].CollisionPos, checkSize, ccBlock[i].CollisionSize))
        {
            return true;
        }
    }

    return false;
}

// =========================================================
// Respawn Point更新
// =========================================================
void UpdateRespawnPoint()
{
    if (player.state == PLAYER_STATE_DEAD) return;

    RESPAWN_POINT* point = GetRespawnPoint();
    if (point == nullptr) return;

    for (int i = 0; i < MAX_RESPAWN_POINT; i++)
    {
        if (!point[i].use) continue;
        if (point[i].active) continue;

        if (CheckBoxCollider(
            player.CollisionPosition, point[i].collisionPos,
            player.CollisionSize, point[i].collisionSize))
        {
            // 前のPointを解除
            for (int j = 0; j < MAX_RESPAWN_POINT; j++)
            {
                point[j].active = false;
            }

            // 新しいRespawn座標
            player.respawnPos = MakeFloat2(
                point[i].pos.x,
                point[i].pos.y + MAP_BLOCK_HEIGHT / 2.0f - PLAYER_COLLISION_SIZE_H / 2.0f
            );

            point[i].active = true;
            return;
        }
    }
}

// =========================================================
// 足元の ColorChangeBlock の移動量をPlayerに適用
// =========================================================
void ApplyGroundCCBlockMovement()
{
    Float2 checkPos = MakeFloat2(player.pos.x, player.pos.y + PLAYER_COLLISION_SIZE_H / 2.0f);
    Float2 checkSize = MakeFloat2(PLAYER_COLLISION_SIZE_W * 0.8f, 12.0f);
    COLOR_CHANGE_BLOCK* ccBlock = GetColorChangeBlock();

    for (int i = 0; i < CCBLOCK_MAX; i++)
    {
        if (!CanPlayerCollideColorChangeBlock(&ccBlock[i])) continue;
        if (CheckBoxCollider(checkPos, ccBlock[i].CollisionPos, checkSize, ccBlock[i].CollisionSize))
        {
            player.pos.x += ccBlock[i].moveDelta.x;
            player.pos.y += ccBlock[i].moveDelta.y;
            player.CollisionPosition = player.pos;
            return;
        }
    }
}

// =========================================================
// 全Enemyの狙われている状態をリセット
// =========================================================
void ResetAllEnemyIsAimed()
{
    Mushroom_Melee* mushroomMelee = GetMushroomMelee();
    for (int i = 0; i < MAX_ENEMY; i++)
    {
        if (!mushroomMelee[i].use) continue;
        mushroomMelee[i].isBeingAimed = false;
    }

    Mushroom_Range* mushroomRange = GetMushroomRange();
    for (int i = 0; i < MAX_ENEMY; i++)
    {
        if (!mushroomRange[i].use) continue;
        mushroomRange[i].isBeingAimed = false;
    }

    //これから追加する

    BOSS* boss = GetBoss();
    if (boss != nullptr)
    {
        boss->isBeingAimed = false;
        boss->aimedPart = BOSS_AIM_NONE;
        boss->CollisionPosition = MakeFloat2(0.0f, 0.0f);
        boss->CollisionSize = MakeFloat2(0.0f, 0.0f);
    }
}

// =========================================================
// 全Enemy_Bulletの狙われている状態をリセット
// =========================================================
void ResetAllEnemyBulletIsAimed()
{
    ENEMY_BULLET* enemyBullet = GetEnemyBullet();

    for (int i = 0; i < MAX_ENEMY_BULLET; i++)
    {
        if (!enemyBullet[i].use) continue;
        enemyBullet[i].isBeingAimed = false;
    }
}

// =========================================================
// Normal Bullet命中Energy加算
// =========================================================
void AddNormalHitCnt()
{
    player.normalHitCnt++;

    if (player.normalHitCnt > SHOOT_ENERGY_MAX_CHARGE)
    {
        player.normalHitCnt = SHOOT_ENERGY_MAX_CHARGE;
    }
}

// =========================================================
// プレイヤー攻撃をうける
// =========================================================
void PlayerHit()
{
    // HIT中も重力を適用する
    player.vel.y += player.gravityAcc;

    int color_id = player.COLORSTATE;
    if (color_id >= PLAYER_COLOR_NUM) color_id = RED;

    int animeSpeed = player_pic[PLAYER_STATE_HIT][color_id].ANIME_SPEED;
    if (animeSpeed <= 0) animeSpeed = 1;

    int hitPattern = g_anime_frame / animeSpeed;

    // HITアニメーション終了後、接地状態に合わせて通常状態へ戻る
    if (hitPattern >= player_pic[PLAYER_STATE_HIT][color_id].PATTERN_MAX)
    {
        // 攻撃を受けた後の短い無敵時間
        player.invincibleTimer = HIT_INVINCIBLE_FRAME;
        player.color.w = 0.5f;

        if (CheckGrounded())
        {
            player.state = (player.vel.x > 0.1f || player.vel.x < -0.1f) ? PLAYER_STATE_WALK : PLAYER_STATE_IDLE;
        }
        else
        {
            player.state = PLAYER_STATE_FALL;
        }

        g_anime_frame = 0;
    }
}

void SetPlayerHit(Float2 hitSourcePos, float damage)
{
    if (!player.use) return;
    if (player.state == PLAYER_STATE_HIT || player.state == PLAYER_STATE_DEAD) return;
    if (player.invincibleTimer > 0) return;

    // カメラシェーク
    StartCameraShake(30.0f, 8);

    // Sound Effect
    PlaySE(SE_Player_Hit);

    // ダメージを食らう
    player.hp -= damage;
    if (player.hp < 0.0f) player.hp = 0.0f;

    if (player.hp <= 0.0f)
    {
        PlaySE(SE_Player_Dead);
        player.state = PLAYER_STATE_DEAD;
    }
    else
    {
        player.state = PLAYER_STATE_HIT;
    }
    player.isAiming = false;
    player.isWallCling = false;
    player.canWallJump = false;
    player.wallDir = 0;

    // チャージ状態をリセット
    g_ShootChargeFrame = 0;
    g_ChargeEffectFrame = 0;
    g_WasZRPressed = false;
    g_IsHalfChargeSEPlayed = false;
    g_IsFullChargeSEPlayed = false;

    // HITアニメーションを先頭から再生
    g_anime_frame = 0;

    // 現在の操作・移動速度を停止して、ヒットバックを優先する
    player.vel = MakeFloat2(0.0f, 0.0f);

    // 攻撃元からプレイヤーへ向かう方向を計算する
    Float2 hitDirection = MakeFloat2(player.pos.x - hitSourcePos.x, player.pos.y - hitSourcePos.y);
    if (hitDirection.x <= 0) player.isFacingRight = true;
    else player.isFacingRight = false;

    float magnitude = sqrtf(hitDirection.x * hitDirection.x + hitDirection.y * hitDirection.y);
    if (magnitude > 0.000001f)
    {
        Float2 dirNormalized = MakeFloat2(hitDirection.x / magnitude, hitDirection.y / magnitude);
        player.exVel = MakeFloat2(dirNormalized.x * HIT_BACK_POWER, dirNormalized.y * HIT_BACK_POWER);
    }
    else
    {
        // 攻撃元とプレイヤーが同じ位置の場合
        player.exVel = MakeFloat2(player.isFacingRight ? -HIT_BACK_POWER : HIT_BACK_POWER, 0.0f);
    }
}

// =========================================================
// プレイヤー復活
// =========================================================
void RevivePlayer(void)
{
    // Respawn Pointへ移動
    player.pos = player.respawnPos;
    player.CollisionPosition = player.pos;

    player.hp = 100.0f;
    player.normalHitCnt = 0;
    g_DrawShootEnergy = 0.0f;

    // 復活後の無敵時間
    player.invincibleTimer = REVIVE_INVINCIBLE_FRAME;
    player.color = MakeFloat4(1.0f, 1.0f, 1.0f, 0.5f);

    player.vel = MakeFloat2(0.0f, 0.0f);
    player.exVel = MakeFloat2(0.0f, 0.0f);

    player.isAiming = false;
    player.aimDir = MakeFloat2(1.0f, 0.0f);
    player.aimAngle = 0.0f;
    player.rayHitPos = player.pos;
    player.rayDistance = 0.0f;

    player.isWallCling = false;
    player.canWallJump = false;
    player.wallDir = 0;
    player.clingTimer = 0;
    player.wallJumpCoyoteTimer = 0;

    player.sprintTimer = 0;
    player.sprintCoolDownTimer = 0;

    g_ShootChargeFrame = 0;
    g_ChargeEffectFrame = 0;
    g_WasZRPressed = false;
    g_IsHalfChargeSEPlayed = false;
    g_IsFullChargeSEPlayed = false;

    g_DrawPreviewLeftLength = 0.0f;
    g_DrawPreviewRightLength = 0.0f;

    g_frame = 0;
    g_anime_frame = 0;

    playerShadowTimer = 0;

    for (int i = 0; i < PLAYER_SHADOW_MAX; i++)
    {
        playerShadow[i] = PLAYER_SHADOW{};
    }

    player.state = PLAYER_STATE_IDLE;
}

// =========================================================
// Player Status UI描画
void DrawStatusUI(void)
{
    DrawShootEnergyBar();
    DrawPlayerIcon();
    DrawPlayerHitFrame();
    DrawPlayerHp();
}

// Player Icon
void DrawPlayerIcon(void)
{
    int animeSpeed = player_status_pic[STATUS_UI::ICON].ANIME_SPEED;
    if (animeSpeed <= 0) animeSpeed = 1;
    int patternMax = player_status_pic[STATUS_UI::ICON].PATTERN_MAX;
    if (patternMax <= 0) patternMax = 1;
    int frame = (g_ui_frame / animeSpeed) % patternMax;
    float tx = player_status_pic[STATUS_UI::ICON].PATTERN_WIDTH * (frame % player_status_pic[STATUS_UI::ICON].PATTERN_NUM_U);
    float ty = player_status_pic[STATUS_UI::ICON].PATTERN_HIGHT * (frame / player_status_pic[STATUS_UI::ICON].PATTERN_NUM_U);
    float tw = player_status_pic[STATUS_UI::ICON].PATTERN_WIDTH;
    float th = player_status_pic[STATUS_UI::ICON].PATTERN_HIGHT;

    DrawSpriteAnimation(
        ICON_POS_X - CENTER_OFFSET_PLAYER.x,
        ICON_POS_Y - CENTER_OFFSET_PLAYER.y,
        ICON_SIZE_X, ICON_SIZE_Y,
        MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f,
        tx, ty, tw, th,
        player.Status_UI_TextureId[STATUS_UI::ICON]
    );
}

// Player HP
void DrawPlayerHp(void)
{
    int animeSpeed = player_status_pic[STATUS_UI::HP_GREEN].ANIME_SPEED;
    if (animeSpeed <= 0) animeSpeed = 1;
    int patternMax = player_status_pic[STATUS_UI::HP_GREEN].PATTERN_MAX;
    if (patternMax <= 0) patternMax = 1;
    int frame = (g_ui_frame / animeSpeed) % patternMax;
    float tx = player_status_pic[STATUS_UI::HP_GREEN].PATTERN_WIDTH * (frame % player_status_pic[STATUS_UI::HP_GREEN].PATTERN_NUM_U);
    float ty = player_status_pic[STATUS_UI::HP_GREEN].PATTERN_HIGHT * (frame / player_status_pic[STATUS_UI::HP_GREEN].PATTERN_NUM_U);
    float tw = player_status_pic[STATUS_UI::HP_GREEN].PATTERN_WIDTH;
    float th = player_status_pic[STATUS_UI::HP_GREEN].PATTERN_HIGHT;

    float maxHP = 100.0f;
    float currentHP = player.hp;
    float rate = player.hp / 100.0f;
    unsigned picId = player.Status_UI_TextureId[STATUS_UI::HP_GREEN];

    if (rate < 0.75f && rate >= 0.5f) picId = player.Status_UI_TextureId[STATUS_UI::HP_ORANGE];
    else if (rate >= 0.25f && rate < 0.5f) picId = player.Status_UI_TextureId[STATUS_UI::HP_ORANGE];
    else if (rate >  0.0f && rate < 0.25f) picId = player.Status_UI_TextureId[STATUS_UI::HP_RED_FAST];
    else if (rate == 0.0f) picId = player.Status_UI_TextureId[STATUS_UI::HP_RED_LINE];

    DrawSpriteAnimation(
        HP_BAR_POS_X, HP_BAR_POS_Y,
        HP_BAR_SIZE_X, HP_BAR_SIZE_Y,
        MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f,
        tx, ty, tw, th,
        picId
    );

    // DrawFrame
    DrawSpriteQuad(
        HP_BAR_POS_X + 10.0f,
        HP_BAR_POS_Y - 30.0f,
        HP_BAR_SIZE_X + 80.0f,
        HP_BAR_SIZE_Y,
        MakeFloat4(1.0f, 1.0f, 1.0f, 0.75f), 0.0f,
        player.Status_UI_TextureId[STATUS_UI::HP_FRAME]
    );
}

// Player Hit Gauge
void DrawPlayerHitFrame(void)
{
    DrawSpriteQuad(
        FRAME_POS_X + SHOOT_ENERGY_BAR_LENGTH / 4.0f,
        FRAME_POS_Y,
        FRAME_SIZE_X, FRAME_SIZE_Y,
        MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f,
        player.Status_UI_TextureId[STATUS_UI::HIT_FRAME]
    );

    DrawSpriteQuad(
        FRAME_POS_X + SHOOT_ENERGY_BAR_LENGTH * 0.75f + SHOOT_ENERGY_BAR_SPACE,
        FRAME_POS_Y,
        FRAME_SIZE_X, FRAME_SIZE_Y,
        MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f,
        player.Status_UI_TextureId[STATUS_UI::HIT_FRAME]
    );
}

// =========================================================
// Shoot Energy Bar描画
// =========================================================
void DrawShootEnergyBar()
{
    const float halfMax = SHOOT_ENERGY_MAX_CHARGE / 2.0f;
    const float halfLength = SHOOT_ENERGY_BAR_LENGTH / 2.0f;
    const float middleSpace = SHOOT_ENERGY_BAR_SPACE;
    const float leftStart = SHOOT_ENERGY_BAR_X;
    const float rightStart = leftStart + halfLength + middleSpace;

    const Float4 barBackGroundColor = MakeFloat4(0.0f, 0.0f, 0.0f, 0.8f);
    const Float4 barColor = MakeFloat4(1.00f, 0.88f, 0.25f, 1.00f);
    const Float4 barShadowColor = MakeFloat4(0.38f, 0.22f, 0.04f, 0.90f);
    const Float4 usePreviewColor = player.COLORSTATE == RED ? MakeFloat4(1.00f, 0.42f, 0.48f, 0.95f) : MakeFloat4(0.20f, 0.88f, 1.00f, 0.95f);
    const Float4 usePreviewShadowColor = player.COLORSTATE == RED ? MakeFloat4(0.38f, 0.04f, 0.08f, 0.90f) : MakeFloat4(0.03f, 0.28f, 0.38f, 0.90f);

    // Bar Back Ground
    DrawSpriteQuad(
        leftStart + halfLength / 2, SHOOT_ENERGY_BAR_Y,
        halfLength, SHOOT_ENERGY_BAR_HEIGHT,
        barBackGroundColor,
        0
    );
    DrawSpriteQuad(
        rightStart + halfLength / 2, SHOOT_ENERGY_BAR_Y,
        halfLength, SHOOT_ENERGY_BAR_HEIGHT,
        barBackGroundColor,
        0
    );

    // Main Energy Bar
    float leftRate = g_DrawShootEnergy / halfMax;
    float rightRate = (g_DrawShootEnergy - halfMax) / halfMax;

    if (leftRate < 0.0f) leftRate = 0.0f;
    if (leftRate > 1.0f) leftRate = 1.0f;
    if (rightRate < 0.0f) rightRate = 0.0f;
    if (rightRate > 1.0f) rightRate = 1.0f;

    float leftLength = (halfLength + 2.0f) * leftRate;
    float rightLength = (halfLength + 2.0f) * rightRate;

    if (leftLength > 0.0f)
    {
        DrawSpriteQuad(
            leftStart + leftLength / 2.0f - 3.0f, SHOOT_ENERGY_BAR_Y,
            leftLength + 3.0f, SHOOT_ENERGY_BAR_HEIGHT,
            barColor,
            0
        );
        DrawSpriteQuad(
            leftStart + leftLength / 2.0f - 3.0f, SHOOT_ENERGY_BAR_Y + 8.0f,
            leftLength + 3.0f, SHOOT_ENERGY_BAR_HEIGHT * 0.3f,
            barShadowColor,
            0
        );
    }

    if (rightLength > 0.0f)
    {
        DrawSpriteQuad(
            rightStart + rightLength / 2.0f - 3.0f, SHOOT_ENERGY_BAR_Y,
            rightLength + 3.0f, SHOOT_ENERGY_BAR_HEIGHT,
            barColor,
            0
        );
        DrawSpriteQuad(
            rightStart + rightLength / 2.0f - 3.0f, SHOOT_ENERGY_BAR_Y + 8.0f,
            rightLength + 3.0f, SHOOT_ENERGY_BAR_HEIGHT * 0.3f,
            barShadowColor,
            0
        );
    }

    // =========================================================
    // エネルギー使用予告描画
    // =========================================================
    float chargeRate = GetShootChargeRate();

    if (player.normalHitCnt >= SHOOT_ENERGY_MAX_CHARGE / 2 && chargeRate > 0.0f)
    {
        float useEnergy = ceilf(SHOOT_ENERGY_MAX_CHARGE * chargeRate);
        if (useEnergy > player.normalHitCnt) useEnergy = static_cast<float>(player.normalHitCnt);

        float currentEnergy = g_DrawShootEnergy;
        float remainEnergy = currentEnergy - useEnergy;
        if (remainEnergy < 0.0f) remainEnergy = 0.0f;

        float remainLeftRate = remainEnergy / halfMax;
        float remainRightRate = (remainEnergy - halfMax) / halfMax;

        if (remainLeftRate < 0.0f) remainLeftRate = 0.0f;
        if (remainLeftRate > 1.0f) remainLeftRate = 1.0f;
        if (remainRightRate < 0.0f) remainRightRate = 0.0f;
        if (remainRightRate > 1.0f) remainRightRate = 1.0f;

        float remainLeftLength = halfLength * remainLeftRate;
        float remainRightLength = halfLength * remainRightRate;

        float previewLeftTarget = leftLength - remainLeftLength;
        float previewRightTarget = rightLength - remainRightLength;

        if (previewLeftTarget < 0.0f) previewLeftTarget = 0.0f;
        if (previewRightTarget < 0.0f) previewRightTarget = 0.0f;

        // Preview Lengthは右Barから先に変化させる
        g_DrawPreviewRightLength = MoveTowardsFloat(g_DrawPreviewRightLength, previewRightTarget, SHOOT_ENERGY_PREVIEW_SPEED);

        // 右Barが目標まで到達した後、左Barを変化させる
        if (g_DrawPreviewRightLength >= previewRightTarget)
        {
            g_DrawPreviewRightLength = previewRightTarget;
            g_DrawPreviewLeftLength = MoveTowardsFloat(g_DrawPreviewLeftLength, previewLeftTarget, SHOOT_ENERGY_PREVIEW_SPEED);
        }

        // Main Preview Draw
        float drawPreviewLeftLength = g_DrawPreviewLeftLength;
        float drawPreviewRightLength = g_DrawPreviewRightLength;

        if (drawPreviewLeftLength > leftLength) drawPreviewLeftLength = leftLength;
        if (drawPreviewRightLength > rightLength) drawPreviewRightLength = rightLength;

        // 左Barの右端から左方向へ消費予定範囲を表示
        if (drawPreviewLeftLength > 0.0f)
        {
            DrawSpriteQuad(
                leftStart + leftLength - drawPreviewLeftLength / 2.0f - 2.0f, SHOOT_ENERGY_BAR_Y,
                drawPreviewLeftLength, SHOOT_ENERGY_BAR_HEIGHT,
                usePreviewColor,
                0
            );
            DrawSpriteQuad(
                leftStart + leftLength - drawPreviewLeftLength / 2.0f - 2.0f, SHOOT_ENERGY_BAR_Y + 8.0f,
                drawPreviewLeftLength, SHOOT_ENERGY_BAR_HEIGHT * 0.3f,
                usePreviewShadowColor,
                0
            );
        }

        // 右Barの右端から左方向へ消費予定範囲を表示
        if (drawPreviewRightLength > 0.0f)
        {
            DrawSpriteQuad(
                rightStart + rightLength - drawPreviewRightLength / 2.0f - 2.0f , SHOOT_ENERGY_BAR_Y,
                drawPreviewRightLength, SHOOT_ENERGY_BAR_HEIGHT,
                usePreviewColor,
                0
            );
            DrawSpriteQuad(
                rightStart + rightLength - drawPreviewRightLength / 2.0f - 2.0f, SHOOT_ENERGY_BAR_Y + 8.0f,
                drawPreviewRightLength, SHOOT_ENERGY_BAR_HEIGHT * 0.3f,
                usePreviewShadowColor,
                0
            );
        }
    }
}
// =========================================================

// =========================================================
// プレイヤー取得
// =========================================================
PLAYER* GetPlayer()
{
    return &player;
}

// =========================================================
// Load Player Data
// =========================================================
void ReloadPlayerStage()
{
    GAME_STAGE currentStage = GetCurrentGameStage();
    if (currentStage < GAME_STAGE_T_01 || currentStage >= GAME_STAGE_MAX) return;

    player.pos = player_start_pos[currentStage];
    player.respawnPos = player.pos;
    player.vel = MakeFloat2(0.0f, 0.0f);
    player.exVel = MakeFloat2(0.0f, 0.0f);

    player.use = true;
    player.color = MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f);
    player.rotation = 0.0f;
    player.state = PLAYER_STATE_IDLE;

    player.CollisionSize = MakeFloat2(PLAYER_COLLISION_SIZE_W, PLAYER_COLLISION_SIZE_H);
    player.CollisionPosition = player.pos;

    player.isFacingRight = true;
    player.coyoteTimer = COYOTE_TIME_FRAMES;

    // Full HP and empty energy
    player.hp = 100.0f;
    player.invincibleTimer = 0;
    player.normalHitCnt = 0;

    // Aim
    player.isAiming = false;
    player.aimDir = MakeFloat2(1.0f, 0.0f);
    player.aimAngle = 0.0f;
    player.rayHitPos = player.pos;
    player.rayDistance = 0.0f;

    // Charge
    g_ShootChargeFrame = 0;
    g_ChargeEffectFrame = 0;
    g_WasZRPressed = false;
    g_IsHalfChargeSEPlayed = false;
    g_IsFullChargeSEPlayed = false;

    g_DrawShootEnergy = 0.0f;
    g_DrawPreviewLeftLength = 0.0f;
    g_DrawPreviewRightLength = 0.0f;

    // Sprint
    player.sprintTimer = 0;
    player.sprintCoolDownTimer = 0;

    // Wall cling
    player.isWallCling = false;
    player.canWallJump = false;
    player.wallDir = 0;
    player.clingTimer = 0;
    player.wallJumpCoyoteTimer = 0;

    // Animation
    g_frame = 0;
    g_anime_frame = 0;
    g_ui_frame = 0;

    // Shadow
    playerShadowTimer = 0;
    for (int i = 0; i < PLAYER_SHADOW_MAX; i++)
    {
        playerShadow[i] = PLAYER_SHADOW{};
    }
}