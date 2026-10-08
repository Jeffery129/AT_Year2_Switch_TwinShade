#include "score.h"

#define SCORE_PATTERN_NUM_U  (5)//パターン数
#define SCORE_PATTERN_NUM_V	 (2)//パターン数
#define SCORE_PATTERN_WIDTH  (1.0f/SCORE_PATTERN_NUM_U)
#define SCORE_PATTERN_HEIGHT (1.0f/SCORE_PATTERN_NUM_V)
#define NUMBER_DISTANCE		 (70.0f)//桁同士の幅

//グローバル変数
NUMBER number[SCORE_KETA];
unsigned int ScoreTextureId;
int score{};

void InitializeScore()
{
	for (int i = 0; i < SCORE_KETA; i++)
	{
		number[i].pos = MakeFloat2(-560.0f, -480.0f);
		number[i].size = MakeFloat2(100.0f, 100.0f);
		number[i].value = 0;
		number[i].use = true;
	}

	ScoreTextureId = LoadTexture("rom:/score.tga");
}

void UpdateScore()
{
}

void DrawScore()
{
	int value = score;

	for (int i = 0; i < SCORE_KETA; i++)
	{
		if (number[i].use == true)
		{
			number[i].value = value % 10;
			value /= 10;

			//桁の表示
			DrawSpriteAnimation(
				number[i].pos.x - (NUMBER_DISTANCE * i),
				number[i].pos.y,
				number[i].size.x,
				number[i].size.y,
				MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f),
				0.0f,
				SCORE_PATTERN_WIDTH * (number[i].value % SCORE_PATTERN_NUM_U),
				SCORE_PATTERN_HEIGHT * (number[i].value / SCORE_PATTERN_NUM_U),
				SCORE_PATTERN_WIDTH,
				SCORE_PATTERN_HEIGHT,
				ScoreTextureId
			);
		}
	}
}

void FinalizeScore()
{
	UnloadTexture(ScoreTextureId);
}

int GetScore()
{
	return score;
}

void SetScore(int num)
{
	score = num;
}

void AddScore(int num)
{
	score += num;
}
