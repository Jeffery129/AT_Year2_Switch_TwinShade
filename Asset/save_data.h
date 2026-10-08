// =========================================================
// save_data.h ユーザーデータ保存
//
// 制作者:		日付：
// =========================================================
#ifndef _SAVE_DATA_H_
#define _SAVE_DATA_H_

#include "main.h"

// =========================================================
// プロトタイプ宣言
// =========================================================
bool InitializeSaveData(void);
void FinalizeSaveData(void);

bool LoadSaveData(void);
bool SaveUserData(void);

void SetSaveUnlockedStage(GAME_STAGE stage);
GAME_STAGE GetSaveUnlockedStage(void);

void SaveOptionData(void);
void ResetSaveData(void);

#endif //_SAVE_DATA_H_