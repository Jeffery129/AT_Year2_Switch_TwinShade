// =========================================================
// boss.h
// 
// 制作者:		日付：
// =========================================================
#ifndef _BOSS_H_
#define _BOSS_H_

//---必ず入れる----
#include "main.h"
#include "texture.h"
#include "sprite.h"
//-----------------
#include "animation.h"

// =========================================================
// マクロ定義
// =========================================================
#define BOSS_THUNDER_MAX (32)
#define BOSS_BULLET_MAX (128)
#define BOSS_DEAD_EXPLOSION_MAX (16)

//--------------------------------------------------------------
// 列挙型定義 (=^・^=)
//--------------------------------------------------------------
enum BOSS_STATE
{
    BOSS_STATE_IDLE = 0,
    BOSS_STATE_TELEPORT,
    BOSS_STATE_LEFT_AOE,
    BOSS_STATE_RIGHT_AOE,
    BOSS_STATE_RANDOM,
    BOSS_STATE_SWEEP_RTL,
    BOSS_STATE_SHOOT,

    BOSS_STATE_MAX
};

enum BOSS_COLOR_STATE
{
    BOSS_RED = 0,
    BOSS_BLUE,

    BOSS_COLOR_MAX
};

enum BOSS_TELEPORT_TYPE
{
    BOSS_TELEPORT_NONE = 0,
    BOSS_TELEPORT_LEFT,
    BOSS_TELEPORT_RIGHT,
    BOSS_TELEPORT_BASIC
};

enum BOSS_THUNDER_STATE
{
    BOSS_THUNDER_WARN = 0,
    BOSS_THUNDER_ATTACK
};

enum BOSS_INTRO_STATE
{
    BOSS_INTRO_NONE = 0,
    BOSS_INTRO_WARNING,
    BOSS_INTRO_TELEPORT,
    BOSS_INTRO_END
};

enum BOSS_AIM_PART
{
    BOSS_AIM_NONE = 0,
    BOSS_AIM_HEAD,
    BOSS_AIM_RUBY
};

// =========================================================
// Boss構造体
// =========================================================
struct BOSS
{
    Float2 pos;                         // 座標
    Float2 size;                        // サイズ

    // Aim Assit用
    Float2 CollisionSize{};             //
    Float2 CollisionPosition{};         //

    // ALL STATE COLLISION
    Float2 HeadCollisionSize{};       // 頭部Collisionサイズ
    Float2 HeadCollisionPosition{};   // 頭部Collision中心座標

    Float2 RubyCollisionSize{};       // 腹部Ruby Collisionサイズ
    Float2 RubyCollisionPosition{};   // 腹部Ruby Collision中心座標

    float hp{};

    int animeFrame{};

    int idleFrame{};

    int singleCool{};

    int randomCool{};
    int randomCount{};
    int randomTimer{};

    int sweepCool{};

    int shootCool{};
    int shootFrame{};
    int shootTimer{};
    float shootAngle{};
    int shootDirection{ 1 };

    int introFrame{};

    BOSS_STATE state{ BOSS_STATE_IDLE };
    BOSS_STATE nextState{ BOSS_STATE_IDLE };
    BOSS_COLOR_STATE COLORSTATE{ BOSS_RED };
    BOSS_TELEPORT_TYPE teleportType{ BOSS_TELEPORT_NONE };
    BOSS_INTRO_STATE introState{ BOSS_INTRO_NONE };
    BOSS_AIM_PART aimedPart{ BOSS_AIM_NONE };

    unsigned int TextureId[BOSS_PIC_NUM][BOSS_COLOR_NUM];       // テクスチャID
    unsigned int Effect_TextureId[BOSS_EFFECT_PIC_NUM];         // エフェクトのID
    unsigned int WarningTextureId{};                            // Warning イントロのテクスチャID
    unsigned int ThunderWarnTextureId{};                        // Thunder WarnのテクスチャID
    unsigned int BulletTextureId{};                             // Bullet Texture
    
    bool actionDone{};
    bool introDone{};
    bool isBeingAimed{};
    bool use{};
};

// =========================================================
// 雷構造体
// =========================================================
struct BOSS_THUNDER
{
    Float2 pos{};
    Float2 size{};
    Float2 CollisionPosition{};
    Float2 CollisionSize{};

    float damage{};
    float warnScroll{};

    int picId{};
    int warnFrame{};
    int warnMaxFrame{};
    int animeFrame{};

    BOSS_THUNDER_STATE state{ BOSS_THUNDER_WARN };

    bool isSEplayed{ false };
    bool isHorizontal{ false };
    bool use{};
};

// =========================================================
// Boss Bullet構造体
// =========================================================
struct BOSS_BULLET
{
    Float2 pos{};
    Float2 vel{};
    Float2 size{};
    Float2 CollisionPosition{};
    Float2 CollisionSize{};
    float damage{};
    float rotation{};
    int animeFrame{};
    int lifeFrame{};
    bool use{};
};

// =========================================================
// Boss死亡爆発構造体
// =========================================================
struct BOSS_DEAD_EXPLOSION
{
    Float2 pos{};
    Float2 size{};
    float rotation{};
    int animeFrame{};
    bool use{};
};

// =========================================================
// プロトタイプ宣言
// =========================================================
void InitializeBoss();
void UpdateBoss();
void DrawBoss();
void FinalizeBoss();
void ReloadBoss();
void ResetThunder();
void ResetBossBullet();

bool BulletBossCollision(Float2 pos, Float2 size, float damage, int color);

//Bossステージイントロ演出
void StartBossIntro();
void UpdateBossIntro();
void DrawBossWarning();
bool GetBossIntro();

void StartBossShake(float power, int frame);
void DrawBossHpBar();

//Bossと雷のゲッター
BOSS* GetBoss();
BOSS_THUNDER* GetBossThunder();

#endif //_BOSS_H_