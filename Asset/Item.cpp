// =========================================================
// 
// 制作者:		日付：
// =========================================================
#include "main.h"
#include "texture.h"
#include "sprite.h"
#include "Item.h"
#include "block.h"
#include "explosion.h"

// ===================================================
// マクロ定義
// ===================================================
#define ITEM_PATTERN_NO_1 (10)// パターン1
#define ITEM_PATTERN_NO_2 (11)// パターン2
#define ITEM_PATTERN_NO_3 (12)// パターン3
#define ITEM_PATTERN_NUM_U (5)// 横パターン数
#define ITEM_PATTERN_NUM_V (5)// 縦パターン数
#define ITEM_PATTERN_WIDTH (1.0f / ITEM_PATTERN_NUM_U)	// 横パターンサイズ
#define ITEM_PATTERN_HIGHT (1.0f / ITEM_PATTERN_NUM_V)	// 縦パターンサイズ

#define ITEM_MAP_LEFT_TOP_X	(SCREEN_WIDTH * 0.5f * -1 + MAP_BLOCK_WIDTH * 0.5f)
#define ITEM_MAP_LEFT_TOP_Y	(SCREEN_HEIGHT * 0.5f * -1 + MAP_BLOCK_HEIGHT * 0.5f)
#define ITEM_SIZE_X			(120.0f)
#define ITEM_SIZE_Y			(120.0f)

Float3 StageItemMap[] =
{//		PositionX									PositionY									 アイテム番号
	//{ITEM_MAP_LEFT_TOP_X + MAP_BLOCK_WIDTH * 1 , ITEM_MAP_LEFT_TOP_Y + MAP_BLOCK_HEIGHT * 5 , ITEM_PATTERN_NO_1},
	//{ITEM_MAP_LEFT_TOP_X + MAP_BLOCK_WIDTH * 1 , ITEM_MAP_LEFT_TOP_Y + MAP_BLOCK_HEIGHT * 8 , ITEM_PATTERN_NO_2},
	//{ITEM_MAP_LEFT_TOP_X + MAP_BLOCK_WIDTH * 7 , ITEM_MAP_LEFT_TOP_Y + MAP_BLOCK_HEIGHT * 3 , ITEM_PATTERN_NO_2},
	//{ITEM_MAP_LEFT_TOP_X + MAP_BLOCK_WIDTH * 12, ITEM_MAP_LEFT_TOP_Y + MAP_BLOCK_HEIGHT * 4 , ITEM_PATTERN_NO_3},

	{0.0f, 0.0f, -1}//終了コード -＞　-1
};


// =========================================================
// グローバル変数
// =========================================================
ITEM item[MAX_ITEM];	// アイテムの実体
unsigned int ItemTextureId;	// テクスチャID

void SetStageItem();

// =========================================================
// アイテム初期化
// =========================================================
void InitializeItem(void)
{
	for (int i = 0; i < MAX_ITEM; i++)
	{
		item[i].pos = MakeFloat2(0.0f, 0.0f);		// 座標
		item[i].vel = MakeFloat2(0.0f, 0.0f);		// 移動値
		item[i].size = MakeFloat2(0.0f, 0.0f);		// サイズ
		item[i].no = 0;								// パターン番号
		item[i].use = false;						// 使用フラグ
		item[i].erased = false;					    // 取得状態
	}

	// テクスチャ読み込み
	ItemTextureId = LoadTexture("rom:/number.tga");

	SetStageItem();
}

// =========================================================
// アイテム更新
// =========================================================
void UpdateItem(void)
{
}

// =========================================================
// アイテム描画
// =========================================================
void DrawItem(void)
{
	for (int i = 0; i < MAX_ITEM; i++)
	{
		if (item[i].use == true)
		{
			DrawSpriteAnimation_Scroll(
				item[i].pos.x, item[i].pos.y,
				item[i].size.x, item[i].size.y,
				{1.0f, 1.0f, 1.0f, 1.0f},
				0.0f,
				ITEM_PATTERN_WIDTH * (item[i].no % ITEM_PATTERN_NUM_U),
				ITEM_PATTERN_HIGHT * (item[i].no / ITEM_PATTERN_NUM_U),
				ITEM_PATTERN_WIDTH,
				ITEM_PATTERN_HIGHT,
				ItemTextureId,
				true
			);
		}
		else
		{
			if (item[i].erased == true)
			{
				SetExplosion(item[i].pos, {200.0f, 200.0f});
				item[i].erased = false;
			}
		}
	}
}

// =========================================================
// アイテム終了処理
// =========================================================
void FinalizeItem(void)
{
	UnloadTexture(ItemTextureId);
}

// =========================================================
// アイテムのアドレス取得
// =========================================================
ITEM* GetItem(void)
{
	return &item[0];
}

// =========================================================
// アイテムのセット処理
// =========================================================
void SetItem(Float2 p, Float2 s, int no)
{
	for (int i = 0; i < MAX_ITEM; i++)
	{
		if (!item[i].use)
		{
			item[i].pos = p;		// 座標
			item[i].size = s;		// サイズ
			item[i].no = no;		// パターン
			item[i].use = true;		// 使用フラグ
			item[i].erased = false; // 取得状態
			break;
		}
	}
}

void SetStageItem()
{
	int i = 0;

	while (StageItemMap[i].z != -1)
	{
		Float2	pos;
		Float2  size;
		pos = MakeFloat2(StageItemMap[i].x, StageItemMap[i].y);
		size = MakeFloat2(ITEM_SIZE_X, ITEM_SIZE_Y);
		SetItem(pos,size,(int)StageItemMap[i].z);

		i++;
	}
}