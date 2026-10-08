// =========================================================
// game.h ゲームシーン制御
// 
// 制作者:		日付：
// =========================================================
#ifndef _GAME_H_
#define _GAME_H_

void InitializeGame(void);
void UpdateGame(void);
void DrawGame(void);
void FinalizeGame(void);

void SetGameTimeScaleZero(bool isZero);
bool GetGameTimeScaleZero(void);

//------- DEATH ATM -------
void StartDeathATM(void);
bool GetDeathATM(void);

// Stage
void StartStageChange(GAME_STAGE nextStage);
void ChangeGameStage(GAME_STAGE nextStage);
bool GetStageChanging(void);

#endif