// =========================================================
// portal.h ゲームステージ遷移
// 
// 制作者:		日付：
// =========================================================
#ifndef _PORTAL_H_
#define _PORTAL_H_

//---必ず入れる----
#include "main.h"
//-----------------

//--------------------------------------------------------------
// 構造体 & 列挙体定義 (*´▽｀*)
//--------------------------------------------------------------
enum PORTAL_STATE { INACTIVE, ACTIVE, PORTAL_STATE_MAX};
struct PORTAL
{
	GAME_STAGE myStage{};
	GAME_STAGE targetStage{};
	Float2 pos{};
	PORTAL_STATE state{};

	bool use{ false };
};

// プロトタイプ宣言
void InitializePortal();
void UpdatePortal();
void DrawPortal();
void FinalizePortal();

void ReloadPortalStage();
bool PortalAndPlayerCollision();

PORTAL* GetPortal();

#endif //_PORTAL_H_