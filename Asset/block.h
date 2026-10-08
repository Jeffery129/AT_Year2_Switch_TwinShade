// =========================================================
// block.h
//
// 制作者:		日付：
// =========================================================
#ifndef _BLOCK_H_
#define _BLOCK_H_

//---必ず入れる----
#include "main.h"
#include "texture.h"
#include "sprite.h"
//-----------------

// =========================================================
// マクロ定義
// =========================================================
#define MAX_BLOCK (10000)                 // ブロックの総数
#define MAX_RESPAWN_POINT (100)
#define MAP_BLOCK_NUM_X (100)             // 横に並ぶブロック数
#define MAP_BLOCK_NUM_Y (100)             // 縦に並ぶブロック数
#define MAP_BLOCK_WIDTH (120.0f)           // ブロック一個分の幅
#define MAP_BLOCK_HEIGHT (120.0f)          // ブロック一個分の高さ

// =========================================================
// ブロック構造体
// =========================================================
struct BLOCK
{
	Float2 pos{};
	Float2 vel{};
	Float2 size{};
	bool use{ false };
	Float2 CollisionSize{};                 // 当たり判定サイズ
	Float2 CollisionPosition{};             // 当たり判定の中心座標
};

// =========================================================
// Stage Map Size
// =========================================================
struct STAGE_MAP_SIZE
{
	int columnCnt{};
	int rowCnt{};
};

// =========================================================
// Respawn Point
// =========================================================
struct RESPAWN_POINT
{
	Float2 pos{};
	Float2 collisionPos{};
	Float2 collisionSize{};
	bool active{ false };
	bool use{ false };
};

// =========================================================
// プロトタイプ宣言
// =========================================================
void InitializeBlock(void);
void UpdateBlock(void);
void DrawBlock(void);
void FinalizeBlock(void);
void ReloadBlockStage(void);
BLOCK* GetBlock(void);
RESPAWN_POINT* GetRespawnPoint(void);
void SetBlock(Float2 pos, Float2 size);
STAGE_MAP_SIZE GetStageMapSize(void);

int GetBlockCount(void);
int GetRespawnPointCount(void);

#endif
