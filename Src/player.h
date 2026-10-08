// =========================================================
// player.h プレイヤー制御
// 
// 制作者:        日付：
// =========================================================
#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "animation.h"

//--------------------------------------------------------------
// 列挙型定義 (=^・^=)
//--------------------------------------------------------------
enum PLAYER_STATE
{
    PLAYER_STATE_IDLE = 0,
    PLAYER_STATE_WALK,
    PLAYER_STATE_JUMP,
    PLAYER_STATE_FALL,
    PLAYER_STATE_AIM,
    PLAYER_STATE_CLING,
    PLAYER_STATE_HIT,
    PLAYER_STATE_DEAD,
    PLAYER_STATE_SPRINT,

    PLAYER_STATE_MAX        //最後を表す（状態はいくつかすぐ分かるように）
};

enum COLOR_STATE
{
    RED,
    BLUE,
};

// =========================================================
// 構造体宣言
// =========================================================
struct PLAYER
{
    Float2 pos;            // 座標
    Float2 vel;            // 移動値
    Float2 exVel;          // 外部の力による移動値
    Float2 size;           // サイズ
    bool use;              // 使用フラグ

    Float4 color;          // 色
    float rotation;        // 回転角度
    float gravityAcc;      // 加速度
    PLAYER_STATE state;    // 状態

    COLOR_STATE COLORSTATE;    // キャラのColor State
    unsigned int TextureId[PLAYER_PIC_NUM][PLAYER_COLOR_NUM];                   // テクスチャID
    unsigned int Effect_TextureId[PLAYER_EFFECT_PIC_NUM][PLAYER_COLOR_NUM];     // エフェクトのID
    unsigned int Shadow_TextureId[PLAYER_COLOR_NUM];                            // Sprint ShadowのID
    unsigned int Status_UI_TextureId[PLAYER_STATUS_PIC_NUM];                    // Status UIのID

    Float2 CollisionSize;        //当たり判定サイズ
    Float2 CollisionPosition;    //当たり判定の中心座標

    bool isFacingRight;        //プレーヤーの向き
    int coyoteTimer;           //coyoteタイム

    //Basic Status
    float hp;
    int invincibleTimer;       // 無敵時間
    Float2 respawnPos;         // リスポーン位置

    //Aiming
    bool isAiming;
    Float2 aimDir;
    float aimAngle;
    Float2 rayHitPos;
    float rayDistance;

    //Shoot
    int normalHitCnt;

    //Sprint
    int sprintTimer;
    int sprintCoolDownTimer;

    //Cling
    bool isWallCling;
    bool canWallJump;
    int wallDir;
    int clingTimer;
    int wallJumpCoyoteTimer;
};

// =========================================================
// プロトタイプ宣言
// =========================================================
void InitializePlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);
void FinalizePlayer(void);

PLAYER* GetPlayer(void);

//------- AIM ---------
void DrawAimLine();              //描画レーヤー調整用
void DrawChargeEffect();         //描画レーヤー調整用

//------- SPRINT -------
void DrawSprintCoolDownBar();    //描画レーヤー調整用

//------- SHOOT -------
void AddNormalHitCnt(void);

//------- HIT ---------
void SetPlayerHit(Float2 hitSourcePos, float damage);

//------- DEAD ---------
void RevivePlayer(void);
//------- RESPAWN -------
void UpdateRespawnPoint();

//------- STATUS UI --------
void DrawStatusUI(void);

//------- LOAD PLAYER ------
void ReloadPlayerStage();

#endif