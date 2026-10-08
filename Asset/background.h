// =========================================================
// background.h
// 
// 制作者:		日付：
// =========================================================
#ifndef _BACKGROUND_H_
#define _BACKGROUND_H_

//---必ず入れる----
#include "main.h"
#include "texture.h"
#include "sprite.h"
//-----------------

#define BG_PIC_NUMBER	(7)

// layerに掛けるスクロール倍率、字が大きいほど速く動く
// speed_layer 0 = 動かない
#define BG_PARALLAX_RATE (0.1f)

//--------------------------------------------------------------
// 構造体定義 (*´▽｀*)
//--------------------------------------------------------------
struct BG_Data
{
	const char FILE_NAME[256]{};
	int speed_layer;
};

static BG_Data g_bg[GAME_STAGE_MAX][BG_PIC_NUMBER]
{
	{// STAGE_Tutorial_01
		{ "rom:/Tutorial_BG_1.tga", 0},
		{ "rom:/Tutorial_BG_1.tga", 0},
		{ "rom:/Tutorial_BG_1.tga", 0},
		{ "rom:/Tutorial_BG_1.tga", 0},
		{ "rom:/Tutorial_BG_1.tga", 0},
		{ "rom:/Tutorial_BG_2.tga", 2},
		{ "rom:/Tutorial_BG_3.tga", 4},
	},

	{// STAGE_Tutorial_02
		{ "rom:/Stage1_Corridors_07.tga", 0},
		{ "rom:/Stage1_Corridors_06.tga", 1},
		{ "rom:/Stage1_Corridors_05.tga", 2},
		{ "rom:/Stage1_Corridors_04.tga", 2},
		{ "rom:/Stage1_Corridors_03.tga", 3},
		{ "rom:/Stage1_Corridors_02.tga", 3},
		{ "rom:/Stage1_Corridors_01.tga", 5},
	},

	{// STAGE_Tutorial_03
		{ "rom:/Stage2_Cave_07.tga", 0},
		{ "rom:/Stage2_Cave_06.tga", 1},
		{ "rom:/Stage2_Cave_05.tga", 1},
		{ "rom:/Stage2_Cave_04.tga", 2},
		{ "rom:/Stage2_Cave_03.tga", 3},
		{ "rom:/Stage2_Cave_02.tga", 4},
		{ "rom:/Stage2_Cave_01.tga", 5},
	},

	{// STAGE_Main_Stage_01
		{ "rom:/Stage2_Cave_07.tga", 0},
		{ "rom:/Stage2_Cave_06.tga", 1},
		{ "rom:/Stage2_Cave_05.tga", 1},
		{ "rom:/Stage2_Cave_04.tga", 2},
		{ "rom:/Stage2_Cave_03.tga", 3},
		{ "rom:/Stage2_Cave_02.tga", 4},
		{ "rom:/Stage2_Cave_01.tga", 5},
	},

	{// STAGE_Main_Stage_02
		{ "rom:/Stage1_Corridors_07.tga", 0},
		{ "rom:/Stage1_Corridors_06.tga", 1},
		{ "rom:/Stage1_Corridors_05.tga", 2},
		{ "rom:/Stage1_Corridors_04.tga", 2},
		{ "rom:/Stage1_Corridors_03.tga", 3},
		{ "rom:/Stage1_Corridors_02.tga", 3},
		{ "rom:/Stage1_Corridors_01.tga", 5},
	},

	{// BOSS_STAGE
		{ "rom:/Boss_Stage_Cave_07.tga", 0},
		{ "rom:/Boss_Stage_Cave_06.tga", 1},
		{ "rom:/Boss_Stage_Cave_05.tga", 2},
		{ "rom:/Boss_Stage_Cave_04.tga", 3},
		{ "rom:/Boss_Stage_Cave_03.tga", 3},
		{ "rom:/Boss_Stage_Cave_02.tga", 4},
		{ "rom:/Boss_Stage_Cave_01.tga", 5},
	},
};

//--------------------------------------------------------------
// プロトタイプ宣言
//--------------------------------------------------------------
void InitializeBG();
void UpdateBG();
void DrawBG();
void FinalizeBG();

#endif // _BACKGROUND_H_