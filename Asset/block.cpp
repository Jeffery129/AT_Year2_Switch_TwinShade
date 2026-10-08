// =========================================================
// block.cpp
//
// 制作者:		日付：
// =========================================================
#include <cstdlib>
#include <nn/fs.h>
#include <nn/nn_Log.h>
#include "block.h"

// =========================================================
// グローバル変数
// =========================================================
BLOCK block[MAX_BLOCK]{};
RESPAWN_POINT respawnPoint[MAX_RESPAWN_POINT]{};
Float2 CollisionOffset{};

int blockCount{};
int respawnPointCount{};

// =========================================================
// Stage CSV
// =========================================================
const char* csv_MapPath[GAME_STAGE_MAX]
{
	"rom:/StageMap_T_01.csv",
	"rom:/StageMap_T_02.csv",
	"rom:/StageMap_T_03.csv",
	"rom:/StageMap_S_01.csv",
	"rom:/StageMap_S_02.csv",
	"rom:/BOSS_StageMap.csv",
};

STAGE_MAP_SIZE g_StageMapSize[GAME_STAGE_MAX]{};

// =========================================================
// Stage Block Texture
// =========================================================
const char* blockTexturePath[GAME_STAGE_MAX]
{
	"rom:/Tutorial_Block.tga",
	"rom:/Stage1_Block.tga",
	"rom:/Stage2_Cave_Block.tga",
	"rom:/Stage2_Cave_Block.tga",
	"rom:/Tutorial_Block.tga",
	"rom:/Boss_Block.tga",
};

unsigned int blockTextureID[GAME_STAGE_MAX]{};

// =========================================================
// マップデータ
// 0 = スペース、1 = ブロック、10 = リスポーンポイント
// =========================================================
int MapTip[GAME_STAGE_MAX][MAP_BLOCK_NUM_Y][MAP_BLOCK_NUM_X]{};

// =========================================================
// プロトタイプ宣言
// =========================================================
void ParseMapCSV(const char* csvText, size_t csvSize, int stageNum);
void LoadMapCSV(const char* path, int stageNum);
void LoadAllStageMaps(void);
void SetStageData(void);

// =========================================================
// ブロック初期化
// =========================================================
void InitializeBlock(void)
{
	blockCount = 0;
	respawnPointCount = 0;

	for (int i = 0; i < MAX_BLOCK; i++)
	{
		block[i] = BLOCK{};
		block[i].size = MakeFloat2(MAP_BLOCK_WIDTH, MAP_BLOCK_HEIGHT);
	}

	for (int i = 0; i < MAX_RESPAWN_POINT; i++)
	{
		respawnPoint[i] = RESPAWN_POINT{};
	}

	for (int stageNum = 0; stageNum < GAME_STAGE_MAX; stageNum++)
	{
		g_StageMapSize[stageNum] = STAGE_MAP_SIZE{};

		for (int y = 0; y < MAP_BLOCK_NUM_Y; y++)
		{
			for (int x = 0; x < MAP_BLOCK_NUM_X; x++)
			{
				MapTip[stageNum][y][x] = 0;
			}
		}
	}

	// 全StageのBlock Textureを読み込む
	for (int i = 0; i < GAME_STAGE_MAX; i++)
	{
		blockTextureID[i] = LoadTexture(blockTexturePath[i]);
	}

	// 全StageのCSVを読み込む
	LoadAllStageMaps();

	STAGE_MAP_SIZE mapSize = GetStageMapSize();
	NN_LOG(
		"StageMapSize stage=%d column=%d row=%d\n",
		static_cast<int>(GetCurrentGameStage()),
		mapSize.columnCnt,
		mapSize.rowCnt
	);

	if (mapSize.columnCnt <= 0 || mapSize.rowCnt <= 0)
	{
		NN_LOG("Stage map load failed\n");
		return;
	}

	ReloadBlockStage();
}

// =========================================================
// ブロック更新
// =========================================================
void UpdateBlock(void)
{
	for (int i = 0; i < blockCount; i++)
	{
		if (!block[i].use) continue;

		block[i].CollisionPosition = block[i].pos;
		block[i].CollisionSize = block[i].size;
	}
}

// =========================================================
// ブロック描画
// =========================================================
void DrawBlock(void)
{
	GAME_STAGE currentStage = GetCurrentGameStage();
	if (currentStage < GAME_STAGE_T_01 || currentStage >= GAME_STAGE_MAX) return;

	unsigned int textureID = blockTextureID[currentStage];
	if (textureID == 0) return;

	Float2 screenOffset = GetOffset_Scroll();
	for (int i = 0; i < MAX_BLOCK; i++)
	{
		if (!block[i].use) continue;

		float drawX = block[i].pos.x + screenOffset.x;
		float drawY = block[i].pos.y + screenOffset.y;
		float halfW = block[i].size.x * 0.5f;
		float halfH = block[i].size.y * 0.5f;

		// 画面外なら描画しない
		if (drawX + halfW < -SCREEN_WIDTH * 0.5f - MAP_BLOCK_WIDTH) continue;
		if (drawX - halfW > SCREEN_WIDTH * 0.5f + MAP_BLOCK_WIDTH) continue;

		if (drawY + halfH < -SCREEN_HEIGHT * 0.5f - MAP_BLOCK_HEIGHT) continue;
		if (drawY - halfH > SCREEN_HEIGHT * 0.5f + MAP_BLOCK_HEIGHT) continue;

		DrawSpriteQuad_Scroll(
			block[i].pos.x, block[i].pos.y,
			block[i].size.x, block[i].size.y,
			textureID,
			true
		);
	}
}

// =========================================================
// ブロック終了処理
// =========================================================
void FinalizeBlock(void)
{
	for (int i = 0; i < GAME_STAGE_MAX; i++)
	{
		if (blockTextureID[i] == 0) continue;

		UnloadTexture(blockTextureID[i]);
		blockTextureID[i] = 0;
	}

	for (int i = 0; i < MAX_BLOCK; i++)
	{
		block[i] = BLOCK{};
	}

	for (int i = 0; i < MAX_RESPAWN_POINT; i++)
	{
		respawnPoint[i] = RESPAWN_POINT{};
	}

	for (int stageNum = 0; stageNum < GAME_STAGE_MAX; stageNum++)
	{
		g_StageMapSize[stageNum] = STAGE_MAP_SIZE{};

		for (int y = 0; y < MAP_BLOCK_NUM_Y; y++)
		{
			for (int x = 0; x < MAP_BLOCK_NUM_X; x++)
			{
				MapTip[stageNum][y][x] = 0;
			}
		}
	}
}

// =========================================================
// 現在StageのBlockをReload
// =========================================================
void ReloadBlockStage(void)
{
	for (int i = 0; i < blockCount; i++)
	{
		block[i] = BLOCK{};
	}

	for (int i = 0; i < respawnPointCount; i++)
	{
		respawnPoint[i] = RESPAWN_POINT{};
	}

	blockCount = 0;
	respawnPointCount = 0;

	SetStageData();
}

// =========================================================
// ブロック配列取得
// =========================================================
BLOCK* GetBlock(void)
{
	return &block[0];
}

// =========================================================
// Respawn Point配列取得
// =========================================================
RESPAWN_POINT* GetRespawnPoint(void)
{
	return &respawnPoint[0];
}

// =========================================================
// ブロック設置
// =========================================================
void SetBlock(Float2 pos, Float2 size)
{
	if (blockCount >= MAX_BLOCK)
	{
		NN_LOG("MAX_BLOCK overflow\n");
		return;
	}

	block[blockCount] = BLOCK{};
	block[blockCount].pos = pos;
	block[blockCount].vel = MakeFloat2(0.0f, 0.0f);
	block[blockCount].size = size;
	block[blockCount].CollisionPosition = pos;
	block[blockCount].CollisionSize = size;
	block[blockCount].use = true;

	blockCount++;
}

// =========================================================
// CSV文字列解析
// =========================================================
void ParseMapCSV(const char* csvText, size_t csvSize, int stageNum)
{
	if (csvText == nullptr || csvSize == 0) return;
	if (stageNum < 0 || stageNum >= GAME_STAGE_MAX) return;

	for (int y = 0; y < MAP_BLOCK_NUM_Y; y++)
	{
		for (int x = 0; x < MAP_BLOCK_NUM_X; x++)
		{
			MapTip[stageNum][y][x] = 0;
		}
	}

	g_StageMapSize[stageNum] = STAGE_MAP_SIZE{};

	size_t startIndex = 0;

	// UTF-8 BOMを無視する
	if (csvSize >= 3 &&
		static_cast<unsigned char>(csvText[0]) == 0xEF &&
		static_cast<unsigned char>(csvText[1]) == 0xBB &&
		static_cast<unsigned char>(csvText[2]) == 0xBF)
	{
		startIndex = 3;
	}

	int x = 0;
	int y = 0;
	int maxColumnCnt = 0;
	int value = 0;
	bool hasNumber = false;
	bool hasCellData = false;

	for (size_t i = startIndex; i < csvSize && y < MAP_BLOCK_NUM_Y; i++)
	{
		char currentChar = csvText[i];

		// 数字を読み取る
		if (currentChar >= '0' && currentChar <= '9')
		{
			value = value * 10 + static_cast<int>(currentChar - '0');
			hasNumber = true;
			hasCellData = true;
			continue;
		}

		// カンマで現在Cellを確定
		if (currentChar == ',')
		{
			if (x < MAP_BLOCK_NUM_X)
			{
				MapTip[stageNum][y][x] = hasNumber ? value : 0;
				x++;
			}

			value = 0;
			hasNumber = false;
			hasCellData = true;
			continue;
		}

		// 改行で最後のCellとRowを確定
		if (currentChar == '\n')
		{
			if (x < MAP_BLOCK_NUM_X && (hasNumber || hasCellData))
			{
				MapTip[stageNum][y][x] = hasNumber ? value : 0;
				x++;
			}

			if (x > maxColumnCnt) maxColumnCnt = x;

			// 完全な空行はRow数へ含めない
			if (x > 0) y++;

			x = 0;
			value = 0;
			hasNumber = false;
			hasCellData = false;
			continue;
		}

		// '\r'、スペース、その他文字は無視する
	}

	// ファイル末尾に改行がない場合、最後のRowを確定
	if (y < MAP_BLOCK_NUM_Y && (x > 0 || hasNumber || hasCellData))
	{
		if (x < MAP_BLOCK_NUM_X)
		{
			MapTip[stageNum][y][x] = hasNumber ? value : 0;
			x++;
		}

		if (x > maxColumnCnt) maxColumnCnt = x;
		y++;
	}

	g_StageMapSize[stageNum].columnCnt = maxColumnCnt;
	g_StageMapSize[stageNum].rowCnt = y;
}

// =========================================================
// CSVファイル読み込み Switch
// =========================================================
void LoadMapCSV(const char* path, int stageNum)
{
	if (stageNum < 0 || stageNum >= GAME_STAGE_MAX) return;

	if (path == nullptr)
	{
		NN_LOG("CSV path is null stage=%d\n", stageNum);
		g_StageMapSize[stageNum] = STAGE_MAP_SIZE{};
		return;
	}

	nn::fs::FileHandle file{};
	nn::Result result = nn::fs::OpenFile(&file, path, nn::fs::OpenMode_Read);

	if (result.IsFailure())
	{
		NN_LOG("Failed to open CSV: %s\n", path);
		g_StageMapSize[stageNum] = STAGE_MAP_SIZE{};
		return;
	}

	int64_t fileSize = 0;
	result = nn::fs::GetFileSize(&fileSize, file);

	if (result.IsFailure() || fileSize <= 0)
	{
		NN_LOG("Invalid CSV file size: %s\n", path);
		nn::fs::CloseFile(file);
		g_StageMapSize[stageNum] = STAGE_MAP_SIZE{};
		return;
	}

	size_t bufferSize = static_cast<size_t>(fileSize);
	char* csvBuffer = new char[bufferSize + 1];


	size_t readSize = 0;
	result = nn::fs::ReadFile(&readSize, file, 0, csvBuffer, bufferSize);
	nn::fs::CloseFile(file);

	if (result.IsFailure() || readSize == 0)
	{
		NN_LOG("Failed to read CSV: %s\n", path);
		delete[] csvBuffer;
		g_StageMapSize[stageNum] = STAGE_MAP_SIZE{};
		return;
	}

	csvBuffer[readSize] = '\0';
	ParseMapCSV(csvBuffer, readSize, stageNum);

	NN_LOG(
		"Loaded CSV: %s column=%d row=%d\n",
		path,
		g_StageMapSize[stageNum].columnCnt,
		g_StageMapSize[stageNum].rowCnt
	);

	delete[] csvBuffer;
}

// =========================================================
// 全StageのCSV読み込み
// =========================================================
void LoadAllStageMaps(void)
{
	for (int stageNum = 0; stageNum < GAME_STAGE_MAX; stageNum++)
	{
		LoadMapCSV(csv_MapPath[stageNum], stageNum);
	}
}

// =========================================================
// 現在StageのBlock・Respawn Pointを設定
// =========================================================
void SetStageData(void)
{
	GAME_STAGE currentStage = GetCurrentGameStage();

	if (currentStage < GAME_STAGE_T_01 || currentStage >= GAME_STAGE_MAX)
	{
		NN_LOG("Invalid current stage: %d\n", static_cast<int>(currentStage));
		return;
	}

	STAGE_MAP_SIZE mapSize = GetStageMapSize();

	if (mapSize.columnCnt <= 0 || mapSize.rowCnt <= 0)
	{
		NN_LOG(
			"Invalid map size stage=%d column=%d row=%d\n",
			static_cast<int>(currentStage),
			mapSize.columnCnt,
			mapSize.rowCnt
		);
		return;
	}

	Float2 startPos = MakeFloat2(
		-SCREEN_WIDTH / 2.0f + MAP_BLOCK_WIDTH / 2.0f,
		-SCREEN_HEIGHT / 2.0f + MAP_BLOCK_HEIGHT / 2.0f
	);

	blockCount = 0;
	respawnPointCount = 0;

	for (int y = 0; y < mapSize.rowCnt; y++)
	{
		for (int x = 0; x < mapSize.columnCnt; x++)
		{
			int mapValue = MapTip[currentStage][y][x];

			Float2 mapPos = MakeFloat2(
				startPos.x + MAP_BLOCK_WIDTH * x,
				startPos.y + MAP_BLOCK_HEIGHT * y
			);

			// 通常Block
			if (mapValue == 1)
			{
				SetBlock(
					mapPos,
					MakeFloat2(
						MAP_BLOCK_WIDTH,
						MAP_BLOCK_HEIGHT
					)
				);
			}
			// Respawn Point
			else if (mapValue == 10)
			{
				if (respawnPointCount >= MAX_RESPAWN_POINT)
				{
					NN_LOG(
						"MAX_RESPAWN_POINT overflow stage=%d\n",
						static_cast<int>(currentStage)
					);
					continue;
				}

				respawnPoint[respawnPointCount].pos =
					mapPos;

				respawnPoint[respawnPointCount].collisionPos =
					MakeFloat2(
						mapPos.x,
						mapPos.y - MAP_BLOCK_HEIGHT
					);

				respawnPoint[respawnPointCount].collisionSize =
					MakeFloat2(
						MAP_BLOCK_WIDTH,
						MAP_BLOCK_HEIGHT * 3.0f
					);

				respawnPoint[respawnPointCount].active = false;
				respawnPoint[respawnPointCount].use = true;

				respawnPointCount++;
			}
		}
	}

	NN_LOG(
		"SetStageData stage=%d map=%dx%d block=%d respawn=%d\n",
		static_cast<int>(currentStage),
		mapSize.columnCnt,
		mapSize.rowCnt,
		blockCount,
		respawnPointCount
	);
}

// =========================================================
// カメラ移動範囲制限用マップサイズ取得
// =========================================================
STAGE_MAP_SIZE GetStageMapSize(void)
{
	GAME_STAGE currentStage = GetCurrentGameStage();

	if (currentStage < GAME_STAGE_T_01 || currentStage >= GAME_STAGE_MAX)
	{
		return STAGE_MAP_SIZE{};
	}

	return g_StageMapSize[currentStage];
}

// =========================================================
// Block数取得
// =========================================================
int GetBlockCount(void)
{
	return blockCount;
}

// =========================================================
// Respawn Point数取得
// =========================================================
int GetRespawnPointCount(void)
{
	return respawnPointCount;
}
