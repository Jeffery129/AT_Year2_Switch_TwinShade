// =========================================================
// color_change_block.h
// 
// 制作者:		日付：
// =========================================================
#ifndef _COLOR_CHANGE_BLOCK_H_
#define _COLOR_CHANGE_BLOCK_H_

//---必ず入れる----
#include "main.h"
//-----------------
// 
// =========================================================
// マクロ定義
// =========================================================
#define CCBLOCK_MAX        (100)

//--------------------------------------------------------------
// 構造体 & 列挙体定義 (*´▽｀*)
//--------------------------------------------------------------
enum MOVE_TYPE { STATIC, MOVING };
enum COLOR_CHANGE_TYPE { NO_CHANGE, AUTO_CHANGE };
enum BLOCK_COLOR { BLOCK_RED, BLOCK_BLUE, BLOCK_ORIGIN };

struct COLOR_CHANGE_BLOCK
{
	Float2 pos{};
	Float2 size{};
	Float2 vel{};
	Float2 moveDelta{}; //1フレームの移動値
	Float2 CollisionPos{};
	Float2 CollisionSize{};

	Float2 patrolStartPos{};
	Float2 patrolEndPos{};
	Float2 moveDir{};
	float moveSpeed{};
	bool isMovingToEnd{ true };

	MOVE_TYPE moveType{ MOVE_TYPE::STATIC };
	COLOR_CHANGE_TYPE colorChangeType{ COLOR_CHANGE_TYPE::NO_CHANGE };
	BLOCK_COLOR colorType{ BLOCK_COLOR::BLOCK_ORIGIN };

	bool isSameColorAsPlayer{ false };
	bool use{ false };
};

// プロトタイプ宣言
void InitializeColorChangeBlock();
void UpdateColorChangeBlock();
void DrawColorChangeBlock();
void FinalizeColorChangeBlock();
void ReloadColorChangeBlockStage();

COLOR_CHANGE_BLOCK* GetColorChangeBlock();
bool CanPlayerCollideColorChangeBlock(const COLOR_CHANGE_BLOCK* targetCCBlock);

// Tutorial_03専用
void SetTutorialColorBlockMoving();

#endif //_COLOR_CHANGE_BLOCK_H_