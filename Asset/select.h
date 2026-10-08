// =========================================================
// select.h タイトルからのステージ選択画面
//
// 制作者:        日付：
// =========================================================
#ifndef _SELECT_H_
#define _SELECT_H_

//---必ず入れる----
#include "main.h"
//-----------------

//--------------------------------------------------------------
// プロトタイプ宣言
//--------------------------------------------------------------
void InitializeSelect(void);
void UpdateSelect(void);
void DrawSelect(void);
void FinalizeSelect(void);

// Stage解放状態
void SetLatestUnlockedStage(GAME_STAGE stage);
GAME_STAGE GetLatestUnlockedStage(void);

#endif //_SELECT_H_
