// =========================================================
// particle_waterfall.cpp
// Particle管理
// =========================================================
#include "particle_waterfall.h"
#include "main.h"
#include "texture.h"
#include "sprite.h"

// =========================================================
// マクロ定義
// =========================================================
#define PARTICLE_WATERFALL_MAX		(1000)
#define PARTICLE_PATTERN_MAX		(10)
#define PARTICLE_PATTERN_NUM_U		(10)
#define PARTICLE_PATTERN_NUM_V		(1)
#define PARTICLE_ANIME_SPEED		(10)
#define PARTICLE_FRAME_SIZE_X		(50.0f)
#define PARTICLE_FRAME_SIZE_Y		(50.0f)
#define PARTICLE_SELF_ROT_SPEED		(Deg2Rad(2.0f))
#define PARTICLE_LIFE_MIN			(200)
#define PARTICLE_LIFE_RANGE			(100)

// =========================================================
// 構造体定義
// =========================================================
struct PARTICLE_PIC_DATA
{
	const char FILE_NAME[256]{};
};

struct PARTICLE_WATERFALL
{
	Float2 pos{};             // 現在座標
	Float2 emitPos{};         // 生成元の中心座標
	float baseX{};            // Wave移動の基準X座標
	float speedY{};           // 縦方向移動速度
	float scale{};            // 初期Scale
	float selfRot{};          // Particle自身の回転角度
	float directionRot{};     // Particle全体の移動方向
	float wavePhase{};        // Wave位相
	float waveSpeed{};        // Wave速度
	float waveWidth{};        // Wave幅
	int life{};               // 残り寿命
	int maxLife{};            // 初期寿命
	int picIndex{};           // Red・Blue画像番号
	int animeFrame{};         // Animation管理Frame
	bool use{};               // 使用中か
};

// =========================================================
// 画像データ
// =========================================================
const PARTICLE_PIC_DATA g_particlePic[PARTICLE_WATERFALL_PIC_MAX]
{
	{ "rom:/Red_Particle.tga" },
	{ "rom:/Blue_Particle.tga" }
};

// =========================================================
// グローバル変数
// =========================================================
PARTICLE_WATERFALL g_particle[PARTICLE_WATERFALL_MAX]{};
unsigned int g_particleTextureId[PARTICLE_WATERFALL_PIC_MAX]{};
int g_particleFrame{};

// =========================================================
// Particle生成
// =========================================================
static void SetParticleWaterfall(
	float startPosX, float startPosY,
	int posXRange, float directionRot,
	int speedRange, float startSpeedY,
	float startScale, int numPerShoot,
	int picNum
)
{
	if (posXRange <= 0 || speedRange <= 0 || numPerShoot <= 0) return;
	if (picNum < 0 || picNum >= PARTICLE_WATERFALL_PIC_MAX) return;

	for (int count = 0; count < numPerShoot; count++)
	{
		for (int i = 0; i < PARTICLE_WATERFALL_MAX; i++)
		{
			PARTICLE_WATERFALL* target = &g_particle[i];
			if (target->use) continue;

			*target = PARTICLE_WATERFALL{};

			// startPosXを中心に指定範囲内で生成
			float spawnX = startPosX + static_cast<float>(rand() % (posXRange + 1)) - static_cast<float>(posXRange) * 0.5f;

			target->pos = MakeFloat2(spawnX, startPosY);
			target->emitPos = MakeFloat2(startPosX, startPosY);
			target->baseX = spawnX;

			// 元のDX版と同じくYマイナス方向へ移動
			target->speedY = -startSpeedY - static_cast<float>(rand() % speedRange + 1) / 100.0f;

			// Particleごとに大きさとWaveをランダム化
			target->scale = startScale * (0.6f + static_cast<float>(rand() % 80) / 100.0f);
			target->selfRot = 0.0f;
			target->directionRot = directionRot;
			target->wavePhase = Deg2Rad(static_cast<float>(rand() % 360));
			target->waveSpeed = 0.05f + static_cast<float>(rand() % 30) / 1000.0f;
			target->waveWidth = 5.0f + static_cast<float>(rand() % 80) / 10.0f;

			// 寿命と画像を設定
			target->life = PARTICLE_LIFE_MIN + rand() % PARTICLE_LIFE_RANGE;
			target->maxLife = target->life;
			target->picIndex = picNum;

			// 全Particleが同じPatternにならないよう開始Frameをずらす
			target->animeFrame = rand() % (PARTICLE_PATTERN_MAX * PARTICLE_ANIME_SPEED);

			target->use = true;
			break;
		}
	}
}

// =========================================================
// Particle初期化
// =========================================================
void InitializeParticleWaterfall(void)
{
	for (int i = 0; i < PARTICLE_WATERFALL_PIC_MAX; i++)
	{
		g_particleTextureId[i] = LoadTexture(g_particlePic[i].FILE_NAME);
	}

	ResetParticleWaterfall();
}

// =========================================================
// Particle更新
// =========================================================
void UpdateParticleWaterfall(void)
{
	g_particleFrame++;

	for (int i = 0; i < PARTICLE_WATERFALL_MAX; i++)
	{
		PARTICLE_WATERFALL* target = &g_particle[i];
		if (!target->use) continue;

		// 縦移動と横Wave移動
		target->pos.y += target->speedY;
		target->wavePhase += target->waveSpeed;
		target->pos.x = target->baseX + sinf(target->wavePhase) * target->waveWidth;

		// 自転とParticle Animationを更新
		target->selfRot += PARTICLE_SELF_ROT_SPEED;
		target->animeFrame++;
		if (target->animeFrame >= PARTICLE_PATTERN_MAX * PARTICLE_ANIME_SPEED)
		{
			target->animeFrame = 0;
		}

		// 寿命終了時に未使用へ戻す
		target->life--;
		if (target->life <= 0)
		{
			*target = PARTICLE_WATERFALL{};
		}
	}
}

// =========================================================
// Particle生成・描画
// =========================================================
void DrawParticleWaterfall(
	float startPosX, float startPosY,
	int posXRange, float directionRot,
	int speedRange, float startSpeedY,
	float startScale, int numPerShoot,
	int framePerShoot, int picNum
)
{
	if (picNum < 0 || picNum >= PARTICLE_WATERFALL_PIC_MAX) return;
	if (framePerShoot <= 0) framePerShoot = 1;

	// 指定Frameごとに新しいParticleを生成
	if (g_particleFrame % framePerShoot == 0)
	{
		SetParticleWaterfall(
			startPosX, startPosY,
			posXRange, directionRot,
			speedRange, startSpeedY,
			startScale, numPerShoot,
			picNum
		);
	}

	for (int i = 0; i < PARTICLE_WATERFALL_MAX; i++)
	{
		PARTICLE_WATERFALL* target = &g_particle[i];

		// このDraw呼び出しと同じ色のParticleだけ描画
		if (!target->use || target->picIndex != picNum) continue;
		if (g_particleTextureId[target->picIndex] == 0) continue;

		// 現在のAnimation Patternを計算
		int pattern = target->animeFrame / PARTICLE_ANIME_SPEED;
		if (pattern >= PARTICLE_PATTERN_MAX) pattern = PARTICLE_PATTERN_MAX - 1;

		float tx = static_cast<float>(pattern % PARTICLE_PATTERN_NUM_U) / static_cast<float>(PARTICLE_PATTERN_NUM_U);
		float ty = static_cast<float>(pattern / PARTICLE_PATTERN_NUM_U) / static_cast<float>(PARTICLE_PATTERN_NUM_V);
		float tw = 1.0f / static_cast<float>(PARTICLE_PATTERN_NUM_U);
		float th = 1.0f / static_cast<float>(PARTICLE_PATTERN_NUM_V);

		// Particle生成元を中心に移動方向を回転
		float rad = Deg2Rad(target->directionRot);
		float localX = target->pos.x - target->emitPos.x;
		float localY = target->pos.y - target->emitPos.y;
		float drawX = target->emitPos.x + localX * cosf(rad) - localY * sinf(rad);
		float drawY = target->emitPos.y + localX * sinf(rad) + localY * cosf(rad);

		// 寿命に合わせてScaleとAlphaを小さくする
		float lifeRate = static_cast<float>(target->life) / static_cast<float>(target->maxLife);
		if (lifeRate < 0.0f) lifeRate = 0.0f;
		if (lifeRate > 1.0f) lifeRate = 1.0f;

		float drawScale = target->scale * lifeRate;
		unsigned int textureId = g_particleTextureId[target->picIndex];

		DrawSpriteAnimation(
			drawX, drawY,
			PARTICLE_FRAME_SIZE_X * drawScale,
			PARTICLE_FRAME_SIZE_Y * drawScale,
			MakeFloat4(1.0f, 1.0f, 1.0f, lifeRate),
			target->selfRot,
			tx, ty, tw, th,
			textureId
		);
	}
}

// =========================================================
// Particle状態リセット
// =========================================================
void ResetParticleWaterfall(void)
{
	for (int i = 0; i < PARTICLE_WATERFALL_MAX; i++)
	{
		g_particle[i] = PARTICLE_WATERFALL{};
	}

	g_particleFrame = 0;
}

// =========================================================
// Particle終了処理
// =========================================================
void FinalizeParticleWaterfall(void)
{
	for (int i = 0; i < PARTICLE_WATERFALL_PIC_MAX; i++)
	{
		if (g_particleTextureId[i] == 0) continue;

		UnloadTexture(g_particleTextureId[i]);
		g_particleTextureId[i] = 0;
	}

	ResetParticleWaterfall();
}
