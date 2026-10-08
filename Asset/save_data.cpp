// =========================================================
// save_data.cpp ユーザーデータ保存
//
// 制作者:		日付：
// =========================================================
#include "save_data.h"
#include "controller.h"
#include "sound.h"

#include <stdio.h>

// =========================================================
// マクロ定義
// =========================================================
#define SAVE_DATA_FILE_PATH				"host:/user_data.txt"
#define SAVE_DEFAULT_VOLUME				(1.0f)
#define SAVE_DEFAULT_SENSITIVITY		(0.3f)

// =========================================================
// グローバル変数
// =========================================================
GAME_STAGE saveUnlockedStage{ GAME_STAGE_T_01 };
float saveMasterVolume{ SAVE_DEFAULT_VOLUME };
float saveRightStickSensitivity{ SAVE_DEFAULT_SENSITIVITY };

// =========================================================
// プロトタイプ宣言
// =========================================================
void SetDefaultSaveData(void);
bool ValidateSaveData(void);
void ApplySaveData(void);

// =========================================================
// セーブデータ初期化
// =========================================================
bool InitializeSaveData(void)
{
	if (!LoadSaveData())
	{
		SetDefaultSaveData();
		ApplySaveData();
		SaveUserData();
	}

	return true;
}

// =========================================================
// セーブデータ終了処理
// =========================================================
void FinalizeSaveData(void)
{
	SaveOptionData();
}

// =========================================================
// セーブデータ読み込み
// =========================================================
bool LoadSaveData(void)
{
	FILE* file = fopen( SAVE_DATA_FILE_PATH, "r" );

	if (file == nullptr)
	{
		return false;
	}

	int unlockedStage{};
	float masterVolume{};
	float sensitivity{};

	int readCount = fscanf(
		file,
		"%d\n%f\n%f",
		&unlockedStage,
		&masterVolume,
		&sensitivity
	);

	fclose(file);

	if (readCount != 3)
	{
		return false;
	}

	saveUnlockedStage = static_cast<GAME_STAGE>(unlockedStage);

	saveMasterVolume = masterVolume;

	saveRightStickSensitivity = sensitivity;

	if (!ValidateSaveData())
	{
		SetDefaultSaveData();
		ApplySaveData();
		return false;
	}

	ApplySaveData();

	return true;
}

// =========================================================
// セーブデータ書き込み
// =========================================================
bool SaveUserData(void)
{
	saveMasterVolume = GetMasterVolume();

	saveRightStickSensitivity = GetRightStickSensitivity();

	FILE* file = fopen(SAVE_DATA_FILE_PATH, "w");

	if (file == nullptr)
	{
		return false;
	}

	int writeResult = fprintf(
		file,
		"%d\n%.2f\n%.2f\n",
		static_cast<int>(
			saveUnlockedStage
			),
		saveMasterVolume,
		saveRightStickSensitivity
	);

	fflush(file);
	fclose(file);

	if (writeResult <= 0)
	{
		return false;
	}

	return true;
}

// =========================================================
// 解放済みStage設定
// =========================================================
void SetSaveUnlockedStage(GAME_STAGE stage)
{
	if (stage < GAME_STAGE_T_01)
	{
		stage = GAME_STAGE_T_01;
	}

	if (stage >= GAME_STAGE_MAX)
	{
		stage = static_cast<GAME_STAGE>( GAME_STAGE_MAX - 1);
	}

	if (stage <= saveUnlockedStage)
	{
		return;
	}

	saveUnlockedStage = stage;

	SaveUserData();
}

// =========================================================
// 解放済みStage取得
// =========================================================
GAME_STAGE GetSaveUnlockedStage(void)
{
	return saveUnlockedStage;
}

// =========================================================
// Option保存
// =========================================================
void SaveOptionData(void)
{
	saveMasterVolume = GetMasterVolume();

	saveRightStickSensitivity = GetRightStickSensitivity();

	SaveUserData();
}

// =========================================================
// セーブデータリセット
// =========================================================
void ResetSaveData(void)
{
	SetDefaultSaveData();
	ApplySaveData();
	SaveUserData();
}

// =========================================================
// デフォルトデータ設定
// =========================================================
void SetDefaultSaveData(void)
{
	saveUnlockedStage = GAME_STAGE_T_01;

	saveMasterVolume = SAVE_DEFAULT_VOLUME;

	saveRightStickSensitivity = SAVE_DEFAULT_SENSITIVITY;
}

// =========================================================
// セーブデータ確認
// =========================================================
bool ValidateSaveData(void)
{
	if (saveUnlockedStage <
		GAME_STAGE_T_01)
	{
		return false;
	}

	if (saveUnlockedStage >=
		GAME_STAGE_MAX)
	{
		return false;
	}

	if (saveMasterVolume < 0.0f ||
		saveMasterVolume > 1.0f)
	{
		return false;
	}

	if (saveRightStickSensitivity < 0.05f ||
		saveRightStickSensitivity > 1.0f)
	{
		return false;
	}

	return true;
}

// =========================================================
// セーブデータ反映
// =========================================================
void ApplySaveData(void)
{
	SetMasterVolume(
		saveMasterVolume
	);

	SetRightStickSensitivity(
		saveRightStickSensitivity
	);

	ResetRightStickInput();
}
