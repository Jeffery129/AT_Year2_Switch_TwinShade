// =========================================================
// spike.cpp トゲトゲ管理
// 
// 制作者:		日付：
// =========================================================
#include "spike.h"
#include "block.h"
#include "player.h"
#include "collision.h"
#include "texture.h"
#include "sprite.h"

// =========================================================
// マクロ定義
// =========================================================
#define SPIKE_SIZE             (120.0f)
#define SPIKE_COLLISION_DEPTH  (50.0f)
#define SPIKE_DAMAGE           (100.0f)
#define SPIKE_COLLISION_MARGIN (20.0f)

// =========================================================
// 画像データ
// =========================================================
struct Pic_Data_Spike
{
	const char FILE_NAME[256]{};
	Float2 PIC_SIZE{};
};

// =========================================================
// 配置データ
// =========================================================
struct SPIKE_DATA
{
	GAME_STAGE stage;
	Float2 pos;
	SPIKE_DIR dir;
	int count;
};

// =========================================================
// グローバル変数
// =========================================================
Float2 CENTER_OFFSET_SPIKE = MakeFloat2(
	MAP_BLOCK_WIDTH * 8.5f,
	MAP_BLOCK_HEIGHT * 5.0f
);

Pic_Data_Spike spike_pic_data =
{
	"rom:/Spike.tga",
	MakeFloat2(SPIKE_SIZE, SPIKE_SIZE)
};

SPIKE spike[SPIKE_MAX]{};
unsigned int SpikeTextureId{};

// =========================================================
// スパイク配置
// =========================================================
const SPIKE_DATA spike_data[]
{
	#pragma region T_01
	{
		GAME_STAGE_T_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 15.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 14.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		3
	},
	{
		GAME_STAGE_T_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 22.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 14.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		3
	},
	{
		GAME_STAGE_T_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 32.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 14.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		5
	},
	{
		GAME_STAGE_T_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 15.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 6.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_RIGHT,
		3
	},
	{
		GAME_STAGE_T_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 5.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 7.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		1
	},
	{
		GAME_STAGE_T_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 25.5f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 2.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_DOWN,
		40
	},
	{
		GAME_STAGE_T_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 3.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 2.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_DOWN,
		3
	},
	#pragma endregion
	#pragma region T_02
	{
		GAME_STAGE_T_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 20.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 23.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		15
	},
	{
		GAME_STAGE_T_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 40.5f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 23.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		6
	},
	{
		GAME_STAGE_T_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 46.5f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 23.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		2
	},
	{
		GAME_STAGE_T_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 44.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 12.5f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_RIGHT,
		4
	},
	{
		GAME_STAGE_T_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 55.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 14.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_LEFT,
		3
	},
	{
		GAME_STAGE_T_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 55.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 7.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_LEFT,
		3
	},
	{
		GAME_STAGE_T_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 23.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 10.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		39
	},
	#pragma endregion
	#pragma region T_03
	{
		GAME_STAGE_T_03,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 10.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 9.5f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_RIGHT,
		4
	},
	{
		GAME_STAGE_T_03,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 16.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 13.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		3
	},
	{
		GAME_STAGE_T_03,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 25.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 13.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		3
	},
	{
		GAME_STAGE_T_03,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 31.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 10.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_LEFT,
		3
	},
	{
		GAME_STAGE_T_03,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 42.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 10.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		15
	},
	#pragma endregion
	#pragma region S_01
	//area1
	{
		GAME_STAGE_S_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 7.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 98.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		5
	},
	{
		GAME_STAGE_S_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 14.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 96.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		5
	},
	{
		GAME_STAGE_S_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 20.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 94.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		3
	},
	{
		GAME_STAGE_S_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 23.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 96.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		3
	},
	{
		GAME_STAGE_S_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 27.5f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 98.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		6
	},
	{
		GAME_STAGE_S_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 28.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 91.5f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_LEFT,
		2
	},
	{
		GAME_STAGE_S_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 14.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 85.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_DOWN,
		7
	},

	//area2
	{
		GAME_STAGE_S_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 29.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 76.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_DOWN,
		5
	},
	{
		GAME_STAGE_S_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 39.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 85.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		5
	},
	{
		GAME_STAGE_S_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 42.5f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 78.0f - CENTER_OFFSET_SPIKE.y + 20.0f
		),
		SPIKE_UP,
		4
	},
	{
		GAME_STAGE_S_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 44.5f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 82.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		2
	},
	{
		GAME_STAGE_S_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 46.5f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 83.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		2
	},

	//area3
	{
		GAME_STAGE_S_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 51.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 60.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_RIGHT,
		9
	},
	{
		GAME_STAGE_S_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 58.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 54.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_DOWN,
		11
	},
	{
		GAME_STAGE_S_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 64.5f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 68.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		14
	},
	{
		GAME_STAGE_S_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 64.5f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 68.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		14
	},
	{
		GAME_STAGE_S_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 64.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 60.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		1
	},
	{
		GAME_STAGE_S_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 65.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 59.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		1
	},
	{
		GAME_STAGE_S_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 74.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 57.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_DOWN,
		3
	},
	{
		GAME_STAGE_S_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 73.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 66.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		3
	},
	{
		GAME_STAGE_S_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 79.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 65.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		9
	},
	{
		GAME_STAGE_S_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 84.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 60.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_LEFT,
		7
	},

	//area4
	{
		GAME_STAGE_S_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 83.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 41.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_LEFT,
		5
	},
	{
		GAME_STAGE_S_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 81.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 36.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_LEFT,
		3
	},
	{
		GAME_STAGE_S_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 73.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 45.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		3
	},
	{
		GAME_STAGE_S_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 82.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 24.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_LEFT,
		15
	},
	{
		GAME_STAGE_S_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 71.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 24.5f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_RIGHT,
		10
	},
	{
		GAME_STAGE_S_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 75.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 13.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_DOWN,
		7
	},
	{
		GAME_STAGE_S_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 64.5f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 13.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_DOWN,
		4
	},
	{
		GAME_STAGE_S_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 62.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 18.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_RIGHT,
		9
	},
	{
		GAME_STAGE_S_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 67.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 25.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_LEFT,
		7
	},
	{
		GAME_STAGE_S_01,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 63.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 29.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		7
	},

	#pragma endregion
	#pragma region S_02
	//area1
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 21.5f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 97.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		24
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 15.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 91.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_DOWN,
		3
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 23.5f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 90.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_DOWN,
		4
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 22.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 94.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_LEFT,
		1
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 25.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 94.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_RIGHT,
		1
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 23.5f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 93.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		2
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 34.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 93.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_LEFT,
		7
	},

	//area2
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 30.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 83.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_RIGHT,
		9
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 38.5f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 77.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_DOWN,
		6
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 42.5f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 79.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_DOWN,
		2
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 42.5f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 82.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		2
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 44.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 89.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		9
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 53.5f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 90.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		10
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 52.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 77.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_DOWN,
		3
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 55.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 81.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_DOWN,
		3
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 54.5f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 85.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		2
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 59.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 88.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_LEFT,
		3
	},

	//area3
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 61.5f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 73.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_DOWN,
		2
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 63.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 71.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_RIGHT,
		3
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 65.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 69.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_DOWN,
		3
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 71.5f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 78.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		8
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 72.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 74.5f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_RIGHT,
		2
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 76.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 74.5f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_LEFT,
		6
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 72.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 68.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_RIGHT,
		3
	},

	// area4
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 69.5f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 50.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_DOWN,
		4
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 66.5f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 57.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		2
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 64.5f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 56.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		2
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 62.5f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 55.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		2
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 60.5f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 58.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		2
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 58.5f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 55.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		2
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 58.5f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 55.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		2
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 56.5f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 56.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		2
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 52.5f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 57.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		6
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 64.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 48.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_LEFT,
		1
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 60.5f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 43.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_DOWN,
		2
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 46.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 53.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_RIGHT,
		5
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 48.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 56.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		3
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 60.5f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 52.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_DOWN,
		2
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 60.5f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 46.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		2
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 69.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 45.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_LEFT,
		3
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 59.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 49.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_LEFT,
		5
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 49.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 42.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_DOWN,
		3
	},

    // area5
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 30.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 54.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_UP,
		25
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 17.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 52.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_RIGHT,
		3
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 38.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 42.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_DOWN,
		5
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 30.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 42.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_DOWN,
		5
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 34.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 45.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_DOWN,
		3
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 26.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 46.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_DOWN,
		3
	},

	//area6
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 23.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 31.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_LEFT,
		21
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 13.0f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 32.5f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_RIGHT,
		26
	},
	{
		GAME_STAGE_S_02,
		MakeFloat2(
			MAP_BLOCK_WIDTH * 14.5f - CENTER_OFFSET_SPIKE.x,
			MAP_BLOCK_HEIGHT * 19.0f - CENTER_OFFSET_SPIKE.y
		),
		SPIKE_DOWN,
		2
	},

	#pragma endregion
};

// =========================================================
// プロトタイプ宣言
// =========================================================
void SetSpikeData(SPIKE* target, const SPIKE_DATA& data);
void SetSpikeCollision(SPIKE* target, SPIKE_DIR dir);

// =========================================================
// 初期化
// =========================================================
void InitializeSpike()
{
	SpikeTextureId = LoadTexture(spike_pic_data.FILE_NAME);
	ReloadSpikeStage();
}

// =========================================================
// 更新
// =========================================================
void UpdateSpike()
{
	PLAYER* player = GetPlayer();
	if (player == nullptr) return;

	for (int i = 0; i < SPIKE_MAX; i++)
	{
		SPIKE* target = &spike[i];
		if (!target->use) continue;

		// Playerとの当たり判定
		if (CheckBoxCollider(
			target->CollisionPos, player->CollisionPosition,
			target->CollisionSize, player->CollisionSize))
		{
			SetPlayerHit(target->CollisionPos, target->damage);
		}
	}
}

// =========================================================
// 描画
// =========================================================
void DrawSpike()
{
	for (int i = 0; i < SPIKE_MAX; i++)
	{
		SPIKE* target = &spike[i];
		if (!target->use) continue;

		float repeatU = target->size.x / spike_pic_data.PIC_SIZE.x;

		DrawSpriteAnimation_Scroll(
			target->pos.x, target->pos.y,
			target->size.x, target->size.y,
			MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f),
			target->rotation,
			0.0f, 0.0f,
			repeatU, 1.0f,
			SpikeTextureId,
			true
		);
	}
}

// =========================================================
// 終了処理
// =========================================================
void FinalizeSpike()
{
	if (SpikeTextureId != 0)
	{
		UnloadTexture(SpikeTextureId);
		SpikeTextureId = 0;
	}

	for (int i = 0; i < SPIKE_MAX; i++)
	{
		spike[i] = SPIKE{};
	}
}

// =========================================================
// Stageデータ再設定
// =========================================================
void ReloadSpikeStage()
{
	// 前Stageの尖刺をクリア
	for (int i = 0; i < SPIKE_MAX; i++)
	{
		spike[i] = SPIKE{};
	}

	GAME_STAGE currentStage = GetCurrentGameStage();
	const int dataCount = sizeof(spike_data) / sizeof(spike_data[0]);
	int spikeCnt = 0;

	// 現在Stageのデータだけ生成
	for (int i = 0; i < dataCount; i++)
	{
		const SPIKE_DATA& data = spike_data[i];

		if (data.stage != currentStage) continue;
		if (data.count <= 0) continue;
		if (spikeCnt >= SPIKE_MAX) break;

		SetSpikeData(&spike[spikeCnt], data);
		spikeCnt++;
	}
}

// =========================================================
// 尖刺データ設定
// =========================================================
void SetSpikeData(SPIKE* target, const SPIKE_DATA& data)
{
	if (target == nullptr) return;

	target->pos = data.pos;
	target->size = MakeFloat2(
		SPIKE_SIZE * static_cast<float>(data.count),
		SPIKE_SIZE
	);
	target->damage = SPIKE_DAMAGE;

	// 向きに合わせて回転
	switch (data.dir)
	{
	case SPIKE_UP:
		target->rotation = Deg2Rad(0.0f);
		break;

	case SPIKE_RIGHT:
		target->rotation = Deg2Rad(90.0f);
		break;

	case SPIKE_DOWN:
		target->rotation = Deg2Rad(180.0f);
		break;

	case SPIKE_LEFT:
		target->rotation = Deg2Rad(270.0f);
		break;

	default:
		target->rotation = Deg2Rad(0.0f);
		break;
	}

	SetSpikeCollision(target, data.dir);
	target->use = true;
}

// =========================================================
// コリジョン設定
// =========================================================
void SetSpikeCollision(SPIKE* target, SPIKE_DIR dir)
{
	if (target == nullptr) return;

	switch (dir)
	{
	case SPIKE_UP:
		// 上向きは画像の下半分
		target->CollisionSize = MakeFloat2(
			target->size.x - SPIKE_COLLISION_MARGIN * 2.0f,
			SPIKE_COLLISION_DEPTH
		);
		target->CollisionPos = MakeFloat2(
			target->pos.x,
			target->pos.y + SPIKE_SIZE / 4.0f
		);
		break;

	case SPIKE_RIGHT:
		// 右向きは画像の左半分
		target->CollisionSize = MakeFloat2(
			SPIKE_COLLISION_DEPTH,
			target->size.x - SPIKE_COLLISION_MARGIN * 2.0f
		);
		target->CollisionPos = MakeFloat2(
			target->pos.x - SPIKE_SIZE / 4.0f,
			target->pos.y
		);
		break;

	case SPIKE_DOWN:
		// 下向きは画像の上半分
		target->CollisionSize = MakeFloat2(
			target->size.x - SPIKE_COLLISION_MARGIN * 2.0f,
			SPIKE_COLLISION_DEPTH
		);
		target->CollisionPos = MakeFloat2(
			target->pos.x,
			target->pos.y - SPIKE_SIZE / 4.0f
		);
		break;

	case SPIKE_LEFT:
		// 左向きは画像の右半分
		target->CollisionSize = MakeFloat2(
			SPIKE_COLLISION_DEPTH,
			target->size.x - SPIKE_COLLISION_MARGIN * 2.0f
		);
		target->CollisionPos = MakeFloat2(
			target->pos.x + SPIKE_SIZE / 4.0f,
			target->pos.y
		);
		break;

	default:
		target->CollisionSize = MakeFloat2(
			target->size.x - SPIKE_COLLISION_MARGIN * 2.0f,
			SPIKE_COLLISION_DEPTH
		);
		target->CollisionPos = MakeFloat2(
			target->pos.x,
			target->pos.y + SPIKE_SIZE / 4.0f
		);
		break;
	}
}

// =========================================================
// 尖刺取得
// =========================================================
SPIKE* GetSpike()
{
	return &spike[0];
}