// =========================================================
// animation.h アニメーション
// 
// 制作者:		日付：
// =========================================================
#ifndef _ANIMATION_H_
#define _ANIMATION_H_

//--------------------------------------------------------------
// マクロ定義 (´・ω・`)
//--------------------------------------------------------------
#define PLAYER_PIC_NUM             (9)
#define PLAYER_COLOR_NUM           (2)
#define PLAYER_STATUS_PIC_NUM      (7)
#define PLAYER_EFFECT_PIC_NUM      (2)
//--------------------------------------------------------------
// 構造体定義 (*´▽｀*)
//--------------------------------------------------------------
struct Pic_Data
{
	const char FILE_NAME[256]{};
	int PATTERN_MAX{};
	int PATTERN_NUM_U{};
	int PATTERN_NUM_V{};
	int ANIME_SPEED{};
	float PATTERN_WIDTH{ 1.0f / PATTERN_NUM_U };
	float PATTERN_HIGHT{ 1.0f / PATTERN_NUM_V };
};

//--------------------------------------------------------------
// 変数/配列定義 ヽ(^。^)ノ
//--------------------------------------------------------------
const Pic_Data player_pic[PLAYER_PIC_NUM][PLAYER_COLOR_NUM]
{
    // PLAYER_PIC_IDLE
    {
        { "rom:/Idle_Red.tga",  25, 5, 5, 5 },
        { "rom:/Idle_Blue.tga", 25, 5, 5, 5 },
    },

    // PLAYER_PIC_RUN
    {
        { "rom:/Run_Red.tga",  10, 10, 1, 5 },
        { "rom:/Run_Blue.tga", 10, 10, 1, 5 },
    },

    // PLAYER_PIC_JUMP
    {
        { "rom:/Jump_Red.tga",  4, 4, 1, 5 },
        { "rom:/Jump_Blue.tga", 4, 4, 1, 5 },
    },

    // PLAYER_PIC_FALL
    {
        { "rom:/Fall_Red.tga",  4, 4, 1, 5 },
        { "rom:/Fall_Blue.tga", 4, 4, 1, 5 },
    },

    // PLAYER_PIC_CHARGE
    {
        { "rom:/Aim_Red.tga",  4, 4, 1, 5 },
        { "rom:/Aim_Blue.tga", 4, 4, 1, 5 },
    },

    // PLAYER_PIC_CLING
    {
        { "rom:/WallCling_Red.tga",  4, 4, 1, 8 },
        { "rom:/WallCling_Blue.tga", 4, 4, 1, 8 },
    },

    // PLAYER_PIC_HIT
    {
        { "rom:/Hit_Red.tga",  4, 4, 1, 4 },
        { "rom:/Hit_Blue.tga", 4, 4, 1, 4 },
    },

    // PLAYER_PIC_DEAD
    {
        { "rom:/Dead_Red.tga",  15, 15, 1, 7 },
        { "rom:/Dead_Blue.tga", 15, 15, 1, 7 },
    },

    // PLAYER_PIC_SPRINT
    {
        { "rom:/Sprint_Red.tga",  4, 4, 1, 2 },
        { "rom:/Sprint_Blue.tga", 4, 4, 1, 2 },
    }
};

const Pic_Data player_effect_pic[PLAYER_EFFECT_PIC_NUM][PLAYER_COLOR_NUM]
{
    // CHARGE_EFFECT
    {
        { "rom:/Charge_Red.tga",  12, 6, 2, 3 },
        { "rom:/Charge_Blue.tga", 12, 6, 2, 3 },
    },

    // WALL_CLING_SWEATY
    {
        { "rom:/Cling_Sweaty.tga", 4, 4, 1, 5 },
        { "rom:/Cling_Sweaty.tga", 4, 4, 1, 5 },
    }
};

const Pic_Data player_sprint_shadow[PLAYER_COLOR_NUM]
{
    { "rom:/Sprint_Shadow_Red.tga",  4, 4, 1, 2 },
    { "rom:/Sprint_Shadow_Blue.tga", 4, 4, 1, 2 }
};

const Pic_Data player_status_pic[PLAYER_STATUS_PIC_NUM]
{
    { "rom:/Status_UI_Icon.tga",        24, 6, 4, 5 },
    { "rom:/Status_UI_HP_Green.tga",    22, 11, 2, 4 },
    { "rom:/Status_UI_HP_Orange.tga",   22, 11, 2, 4 },
    { "rom:/Status_UI_HP_Red_Fast.tga", 22, 11, 2, 4 },
    { "rom:/Status_UI_HP_Red_Line.tga", 22, 11, 2, 4 },
    { "rom:/Status_UI_HP_Frame.tga",    1, 1, 1, 5 },
    { "rom:/Status_UI_Hit.tga",         1, 1, 1, 5 },
};

//--------------------------------------------------------------
//--------------------------------------------------------------
// マクロ定義 (´・ω・`) （BOSS版）
//--------------------------------------------------------------
#define BOSS_PIC_NUM             (7)
#define BOSS_COLOR_NUM           (2)
#define BOSS_EFFECT_PIC_NUM      (2)

//--------------------------------------------------------------
// 変数/配列定義 ヽ(^。^)ノ
//--------------------------------------------------------------
const Pic_Data boss_pic[BOSS_PIC_NUM][BOSS_COLOR_NUM]
{
    {//Idle
        { "rom:/Boss_Idle.tga", 8, 8, 1, 5 },
        { "rom:/Boss_Idle.tga", 8, 8, 1, 5 },
    },

    {//Teleport
        { "rom:/Boss_Teleport.tga", 13, 13, 1, 4 },
        { "rom:/Boss_Teleport.tga", 13, 13, 1, 4 },
    },

    {//Left_AOE
        { "rom:/Boss_Left_AOE_Red.tga",  15, 15, 1, 6 },
        { "rom:/Boss_Left_AOE_Blue.tga", 15, 15, 1, 6 },
    },

    {//Right_AOE
        { "rom:/Boss_Right_AOE_Red.tga",  15, 15, 1, 6 },
        { "rom:/Boss_Right_AOE_Blue.tga", 15, 15, 1, 6 },
    },

    {//Random_Thunder
        { "rom:/Boss_Random_Red.tga",  55, 5, 11, 5 },
        { "rom:/Boss_Random_Blue.tga", 55, 5, 11, 5 },
    },

    {//Sweep
        { "rom:/Boss_Sweep_Red.tga",  15, 5, 3, 6 },
        { "rom:/Boss_Sweep_Blue.tga", 15, 5, 3, 6 },
    },

    {//Shoot
        { "rom:/Boss_Shoot_White.tga", 6, 6, 1, 5 },
        { "rom:/Boss_Shoot_White.tga", 6, 6, 1, 5 },
    }
};

const Pic_Data boss_effect[BOSS_EFFECT_PIC_NUM]
{
    { "rom:/Boss_Thunder_Red.tga",  8, 8, 1, 3 },
    { "rom:/Boss_Thunder_Blue.tga", 8, 8, 1, 3 },
};

const Pic_Data boss_warning_text { "rom:/Boss_Warning_Text.tga",  32, 8, 4, 2 };
const Pic_Data boss_thunder_warn_pic { "rom:/Boss_Warn_Arrow.tga", 1, 1, 1, 1 };
const Pic_Data boss_bullet_pic{ "rom:/Boss_Bullet.tga",  2, 2, 1, 3 };

const Pic_Data boss_dead_explosion{ "rom:/Boss_Explosion.tga",  6, 6, 1, 4 };

#endif //_ANIMATION_H_