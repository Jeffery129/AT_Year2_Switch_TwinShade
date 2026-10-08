// =========================================================
// spike.h トゲトゲ管理
// 
// 制作者:		日付：
// =========================================================
#ifndef _SPIKE_H_
#define _SPIKE_H_

//---必ず入れる----
#include "main.h"
//-----------------

//--------------------------------------------------------------
// マクロ定義
//--------------------------------------------------------------
#define SPIKE_MAX (100)

//--------------------------------------------------------------
// 構造体 & 列挙体定義 (*´▽｀*)
//--------------------------------------------------------------
enum SPIKE_DIR
{
	SPIKE_UP = 0,
	SPIKE_RIGHT,
	SPIKE_DOWN,
	SPIKE_LEFT
};

struct SPIKE
{
	Float2 pos{};
	Float2 size{};
	Float2 CollisionPos{};
	Float2 CollisionSize{};
	float rotation{};
	float damage{};
	bool use{ false };
};

void InitializeSpike();
void UpdateSpike();
void DrawSpike();
void FinalizeSpike();
void ReloadSpikeStage();

SPIKE* GetSpike();

#endif