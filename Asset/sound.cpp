// ===================================================
// sound.cpp サウンド処理
// 
// 制作者：		日付：
// ===================================================
#include <nn/atk.h>
#include "atk_SampleCommon.h"

#include "main.h"
#include "sound.h"

// ===================================================
// グローバル変数
// ===================================================

//②効果音(WSD)のみ SoundIDリストを作る ※BGMは入れない
static unsigned int g_WSDTable[] =
{
	UI_Title_Start,
	UI_Tab_Switch,
	UI_Stage_Option_Change,
	UI_Stage_Change,
	UI_Open_Menu,
	UI_Close_Menu,
	SE_Player_Hit,
	SE_Player_Dead,
	SE_Player_Jump,
	SE_Player_Switch_Color,
	SE_Player_Dash,
	SE_Player_Charged,
	SE_Normal_Shoot,
	SE_Charge_Shoot,
	SE_Normal_Hit,
	SE_Charge_Hit,
	SE_Boss_Thunder_01,
	SE_Boss_Thunder_02,
	SE_Boss_Thunder_03,
	SE_Boss_Thunder_04,
	SE_Boss_Roar,
	SE_Portal_Next_Stage,
	SE_Game_Clear,
	SE_Boss_Explosion_1,
	SE_Boss_Explosion_2,
	SE_Boss_Explosion_3
};

namespace
{
	//③「.bfsar」を出力したプロジェクト名に合わせる
	const char ArchiveRelativePath[] = "SoundData20260908.bfsar";

    const int SoundHeapSize = 4 * 1024 * 1024;

    nn::atk::SoundHeap          g_SoundHeap;
    nn::atk::FsSoundArchive     g_SoundArchive;
    nn::atk::SoundArchivePlayer g_SoundArchivePlayer;
    nn::atk::SoundDataManager   g_SoundDataManager;

    nn::audio::MemoryPoolType   g_MemoryPool;

    void* g_pMemoryForSoundSystem;
    void* g_pMemoryForSoundHeap;
    void* g_pMemoryForInfoBlock;
    void* g_pMemoryForSoundDataManager;
    void* g_pMemoryForSoundArchivePlayer;
    void* g_pMemoryForStreamBuffer;

    nn::atk::SoundHandle        g_SoundHandleBGM;
    nn::atk::SoundHandle        g_SoundHandleSE;

	// マスターボリューム
	float g_MasterVolume{ 1.0f };
	float g_BgmVolume{ 1.0f };
}


// ===================================================
// サウンドの初期化（InitSystemよりも前に呼ぶこと）
// ===================================================
void InitSound()
{

 	// SoundSystem初期化
	{
		nns::atk::InitializeHeap();
		nns::atk::InitializeFileSystem();


		bool isSuccess = true;

		nn::atk::SoundSystem::SoundSystemParam param;
		std::size_t memSizeForSoundSystem = nn::atk::SoundSystem::GetRequiredMemSize(param);
		g_pMemoryForSoundSystem = nns::atk::Allocate(memSizeForSoundSystem, nn::atk::SoundSystem::WorkMemoryAlignSize);
		isSuccess = nn::atk::SoundSystem::Initialize(
			param,
			reinterpret_cast<uintptr_t>(g_pMemoryForSoundSystem),
			memSizeForSoundSystem);
		NN_ABORT_UNLESS(isSuccess, "cannot initialize SoundSystem");

		// SoundHeap の初期化
		g_pMemoryForSoundHeap = nns::atk::Allocate(SoundHeapSize);
		isSuccess = g_SoundHeap.Create(g_pMemoryForSoundHeap, SoundHeapSize);
		NN_ABORT_UNLESS(isSuccess, "cannot create SoundHeap");

		// SoundArchive の初期化
		const char* archiveAbsolutePath = nns::atk::GetAbsolutePath(ArchiveRelativePath);
		isSuccess = g_SoundArchive.Open(archiveAbsolutePath);
		NN_ABORT_UNLESS(isSuccess, "cannot open SoundArchive(%s)\n", archiveAbsolutePath);

		// SoundArchive のパラメータ情報をメモリにロード
		std::size_t infoBlockSize = g_SoundArchive.GetHeaderSize();
		g_pMemoryForInfoBlock = nns::atk::Allocate(infoBlockSize, nn::atk::FsSoundArchive::BufferAlignSize);
		isSuccess = g_SoundArchive.LoadHeader(g_pMemoryForInfoBlock, infoBlockSize);
		NN_ABORT_UNLESS(isSuccess, "cannot load InfoBlock");

		// SoundDataManager の初期化
		std::size_t memSizeForSoundDataManager = g_SoundDataManager.GetRequiredMemSize(&g_SoundArchive);
		g_pMemoryForSoundDataManager = nns::atk::Allocate(memSizeForSoundDataManager, nn::atk::SoundDataManager::BufferAlignSize);
		isSuccess = g_SoundDataManager.Initialize(
			&g_SoundArchive,
			g_pMemoryForSoundDataManager,
			memSizeForSoundDataManager);
		NN_ABORT_UNLESS(isSuccess, "cannot initialize SoundDataManager");

		// SoundArchivePlayer で用いるストリームバッファの初期化
		// ストリームバッファはメモリプール管理されているヒープから確保する必要があります。
		std::size_t memSizeForStreamBuffer = g_SoundArchivePlayer.GetRequiredStreamBufferSize(&g_SoundArchive);
		g_pMemoryForStreamBuffer = nns::atk::AllocateForMemoryPool(memSizeForStreamBuffer);

		// 専用のヒープをメモリプールにアタッチ
		nn::atk::SoundSystem::AttachMemoryPool(&g_MemoryPool, nns::atk::GetPoolHeapAddress(), nns::atk::GetPoolHeapSize());

		// SoundArchivePlayer の初期化
		std::size_t memSizeForSoundArchivePlayer = g_SoundArchivePlayer.GetRequiredMemSize(&g_SoundArchive);
		g_pMemoryForSoundArchivePlayer = nns::atk::Allocate(memSizeForSoundArchivePlayer, nn::atk::SoundArchivePlayer::BufferAlignSize);
		isSuccess = g_SoundArchivePlayer.Initialize(
			&g_SoundArchive,
			&g_SoundDataManager,
			g_pMemoryForSoundArchivePlayer, memSizeForSoundArchivePlayer,
			g_pMemoryForStreamBuffer, memSizeForStreamBuffer);
		NN_ABORT_UNLESS(isSuccess, "cannot initialize SoundArchivePlayer");
	}

	// SoundData読み込み
	{
		bool isSuccess = true;
		int numWSD = sizeof(g_WSDTable) / sizeof(int);

		for (int i = 0; i < numWSD; i++)
		{
			isSuccess = g_SoundDataManager.LoadData(g_WSDTable[i], &g_SoundHeap);
			NN_ABORT_UNLESS(isSuccess, "LoadData failed.");
		}
	}
}


// ===================================================
// サウンドの終了処理
// ===================================================
void UninitSound()
{
    g_SoundArchivePlayer.Finalize();

    // 専用のヒープをメモリプールからデタッチ
    nn::atk::SoundSystem::DetachMemoryPool(&g_MemoryPool);

    g_SoundDataManager.Finalize();
    g_SoundArchive.Close();
    g_SoundHeap.Destroy();
    nn::atk::SoundSystem::Finalize();

    nns::atk::FreeForMemoryPool(g_pMemoryForStreamBuffer);
    nns::atk::Free(g_pMemoryForSoundArchivePlayer);
    nns::atk::Free(g_pMemoryForSoundDataManager);
    nns::atk::Free(g_pMemoryForInfoBlock);
    nns::atk::Free(g_pMemoryForSoundHeap);
    nns::atk::Free(g_pMemoryForSoundSystem);


	nns::atk::FinalizeFileSystem();
	nns::atk::FinalizeHeap();

}

// ===================================================
// サウンドの更新
// ===================================================
void UpdateSound()
{
    g_SoundArchivePlayer.Update();
}



// ===================================================
// BGM再生
// ===================================================
void PlayBGM(nn::atk::SoundArchive::ItemId soundId)
{
	g_SoundArchivePlayer.StartSound(&g_SoundHandleBGM, soundId);

	// マスターボリューム
	g_SoundHandleBGM.SetVolume(g_BgmVolume * g_MasterVolume, 0);
}

// ===================================================
// BGM停止
// ===================================================
void StopBGM()
{
	g_SoundHandleBGM.Stop(0);
}

// ===================================================
// BGMボリューム調整
// ===================================================
void SetVolumeBGM(float volume, int delayFrame)
{
	//マスターボリューム
	if (volume < 0.0f) volume = 0.0f;
	if (volume > 1.0f) volume = 1.0f;
	g_BgmVolume = volume;

	g_SoundHandleBGM.SetVolume(g_BgmVolume * g_MasterVolume, delayFrame);
}

// ===================================================
// SE再生
// ===================================================
void PlaySE(nn::atk::SoundArchive::ItemId soundId)
{
	g_SoundArchivePlayer.StartSound(&g_SoundHandleSE, soundId);

	// マスターボリューム
	g_SoundHandleSE.SetVolume(g_MasterVolume, 0);
}

// マスターボリューム
// ===================================================
// 主音量設定
// ===================================================
void SetMasterVolume(float volume)
{
	if (volume < 0.0f) volume = 0.0f;
	if (volume > 1.0f) volume = 1.0f;

	g_MasterVolume = volume;

	g_SoundHandleBGM.SetVolume(
		g_BgmVolume * g_MasterVolume,
		0
	);

	g_SoundHandleSE.SetVolume(
		g_MasterVolume,
		0
	);
}

// ===================================================
// 主音量取得
// ===================================================
float GetMasterVolume()
{
	return g_MasterVolume;
}