// ===================================================
// sound.cpp サウンド処理
// 
// 制作者：		日付：
// ===================================================
#pragma once

#include <nn/atk.h>

//①「.fsid」を出力したプロジェクト名に合わせる
#include "SoundData20260908.fsid"

// プロトタイプ宣言
void InitSound();
void UninitSound();
void UpdateSound();

void PlayBGM(nn::atk::SoundArchive::ItemId soundId);
void StopBGM();
void SetVolumeBGM(float volume, int delayFrame=0);

void PlaySE(nn::atk::SoundArchive::ItemId soundId);

// マスターボリューム設定
void SetMasterVolume(float volume);
float GetMasterVolume();