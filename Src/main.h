// ===================================================
// main.h GM21制作用プロジェクト
// 
// 制作者：		日付：
// ===================================================
#pragma once

#include "system.h"
#include <iostream>

#define SCREEN_WIDTH  1920
#define SCREEN_HEIGHT 1080

// 頂点数(四角形)
#define NUM_VERTEX_QUADS (4)

// 頂点情報
struct VERTEX_3D
{
    Float3 Position;    // 座標
    Float4 Color;       // 色
    Float2 TexCoord;    // テクスチャ座標
};

// シーン列挙体
enum SCENE
{
	SCENE_TITLE = 0,
	SCENE_SELECT,
	SCENE_GAME,
	SCENE_RESULT,

	SCENE_MAX
};

enum GAME_STAGE
{
	GAME_STAGE_T_01 = 0, // 基本の移動
	GAME_STAGE_T_02,	 // 壁ジャンプ・Sprint・パークル要素
	GAME_STAGE_T_03,	 // 戦闘要素
	GAME_STAGE_S_01,
	GAME_STAGE_S_02,
	GAME_STAGE_BOSS,	 // BOSS戦

	GAME_STAGE_MAX
};

//
void SetScene(SCENE next);

//
void SetGameStage(GAME_STAGE targetStage);
GAME_STAGE GetCurrentGameStage();

// 通用関数
// Lerp関数
float LerpFloat(float start, float end, float rate);
Float2 LerpFloat2(Float2 start, Float2 end, float rate);
// MoveTowards
float MoveTowardsFloat(float current, float target, float speed);