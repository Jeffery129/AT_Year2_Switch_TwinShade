// =========================================================
// title.cpp タイトルシーン制御
// 
// 制作者:		日付：
// =========================================================
#include "main.h"
#include "title.h"
#include "titlebg.h"
#include "controller.h"
#include "sprite.h"
#include "fade.h"
#include "sound.h"

// =========================================================
// グローバル変数 & 列挙型定義
// =========================================================

// =========================================================
// タイトルシーン初期化
// =========================================================
void InitializeTitle(void)
{
	InitializeOffset();	// オフセットのリセット

	InitializeTitlebg();

	StopBGM();
	PlayBGM(BGM_Title);
}

// =========================================================
// タイトルシーン更新
// =========================================================
void UpdateTitle(void)
{
	UpdateTitlebg();
	
	// シーン切り替え
	if (GetControllerTrigger(NpadButton::Plus::Index) ||
		GetControllerTrigger(NpadButton::Minus::Index) ||
		GetControllerTrigger(NpadButton::A::Index) ||
		GetControllerTrigger(NpadButton::B::Index) ||
		GetControllerTrigger(NpadButton::X::Index) ||
		GetControllerTrigger(NpadButton::Y::Index)
	)
	{	// フェードが動いていないときのみフェードが開始する
		if (GetFade()->state == FADE_NONE)
		{
			PlaySE(UI_Title_Start);
			StartFade(SCENE_SELECT);
		}
	}
}

// =========================================================
// タイトルシーン描画
// =========================================================
void DrawTitle(void)
{
	DrawTitlebg();
}

// =========================================================
// タイトルシーン終了処理
// =========================================================
void FinalizeTitle(void)
{
	FinalizeTitlebg();
}