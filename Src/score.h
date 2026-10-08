#pragma once

#include "main.h"
#include "texture.h"
#include "sprite.h"

#define SCORE_KETA	(6)

struct NUMBER
{
	Float2 pos;
	Float2 size;
	int value;
	bool use;
};

// プロトタイプ宣言
void InitializeScore();
void UpdateScore();
void DrawScore();
void FinalizeScore();

int GetScore();
void SetScore(int num);
void AddScore(int num);