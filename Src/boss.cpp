// =========================================================
// boss.cpp
// 
// 制作者:		日付：
// =========================================================
#include "boss.h"
#include "block.h"
#include "collision.h"
#include "player.h"
#include "bullet.h"
#include "camera.h"
#include "sound.h"
#include "fade.h"

// =========================================================
// マクロ
// =========================================================
#define BOSS_STAGE_CENTER_OFFSET_X		(-MAP_BLOCK_WIDTH * 8.5f)
#define BOSS_STAGE_CENTER_OFFSET_Y		(-MAP_BLOCK_HEIGHT * 5.0f)

#define BOSS_MAP_LEFT_BOUND_X			(2.0f * MAP_BLOCK_WIDTH + BOSS_STAGE_CENTER_OFFSET_X)
#define BOSS_MAP_RIGHT_BOUND_X			(20.0f * MAP_BLOCK_WIDTH + BOSS_STAGE_CENTER_OFFSET_X)

#define BOSS_PIC_SIZE_X					(660.0f) // Picのサイズ
#define BOSS_PIC_SIZE_Y					(840.0f) // Picのサイズ
#define BOSS_PIC_SCALE					(1.25f)   // 描画のサイズ、コリジョン、オフセットなどにかける数値

#define BOSS_BASIC_POS_X				(11.0f * MAP_BLOCK_WIDTH + BOSS_STAGE_CENTER_OFFSET_X) // LEFT & RIGHT以外のSTATEはこの位置で行う
#define BOSS_BASIC_POS_Y				(9.5f * MAP_BLOCK_HEIGHT + BOSS_STAGE_CENTER_OFFSET_Y - BOSS_PIC_SIZE_Y * 0.5f * BOSS_PIC_SCALE)

#define BOSS_HP							(2000.0f)

#define BOSS_IDLE_FRAME					(40)

#define BOSS_TELEPORT_PATTERN			(7)		// ８フレーム目にテレポート先に移動
#define BOSS_SINGLE_HAND_PATTERN		(12)	// １３フレーム目に攻撃を放つ
#define BOSS_RANDOM_PATTERN				(4)		// ５フレーム目に攻撃を始める
#define BOSS_SWEEP_PATTERN				(12)	// １３フレーム目

#define BOSS_SINGLE_HAND_AOE_COOLDOWN	(30 * 60)
#define BOSS_RANDOM_COOLDOWN			(20 * 60)
#define BOSS_SWEEP_COOLDOWN				(40 * 60)

#define BOSS_RANDOM_INTERVAL			(20)     // ランダム攻撃の間隔、フレーム数
#define BOSS_RANDOM_COUNT				(13)	 // ランダムの回数

// ALL STATE COLLISION
#define BOSS_HEAD_COLLISION_SIZE_X		(150.0f)
#define BOSS_HEAD_COLLISION_SIZE_Y		(150.0f)
#define BOSS_HEAD_COLLISION_OFFSET_Y	(-10.0f) // BOSSの頭部、X軸のオフセットなし

#define BOSS_RUBY_COLLISION_SIZE_X		(120.0f)
#define BOSS_RUBY_COLLISION_SIZE_Y		(120.0f)
#define BOSS_RUBY_COLLISION_OFFSET_Y	(BOSS_PIC_SIZE_Y / 4 + 30.0f) // BOSSの腹の部分、X軸のオフセットなし
//----------------------------------------------------------------

#define BOSS_TELEPORT_LEFT_POS_X		(6.0f * MAP_BLOCK_WIDTH + BOSS_STAGE_CENTER_OFFSET_X)
#define BOSS_TELEPORT_RIGHT_POS_X		(16.0f * MAP_BLOCK_WIDTH + BOSS_STAGE_CENTER_OFFSET_X) // Y座標はBASIC_POS_Yと一緒

#define BOSS_THUNDER_SIZE_X				(120.0f)
#define BOSS_THUNDER_SIZE_Y				(8.0f * MAP_BLOCK_HEIGHT) // 1単位の雷の描画とコリジョンサイズ
#define BOSS_THUNDER_POS_Y				(5.5f * MAP_BLOCK_WIDTH + BOSS_STAGE_CENTER_OFFSET_Y)
#define BOSS_THUNDER_COLLISION_HINCH	(40.0f)

#define BOSS_SINGLE_HAND_WARN_FRAME		(90)
#define BOSS_RANDOM_THUNDER_WARN_FRAME	(45)

// Sweep RTL
#define BOSS_SWEEP_RTL_WARN_FRAME		(180)
#define BOSS_SWEEP_RTL_COOLDOWN			(BOSS_SWEEP_COOLDOWN)

// Sweep横雷
#define BOSS_SWEEP_FIXED_ROW_TOP		(4)
#define BOSS_SWEEP_FIXED_ROW_BOTTOM		(7)
#define BOSS_SWEEP_RANDOM_ROW_COUNT		(2)

#define BOSS_SWEEP_THUNDER_WIDTH		(BOSS_MAP_RIGHT_BOUND_X - BOSS_MAP_LEFT_BOUND_X + MAP_BLOCK_WIDTH)
#define BOSS_SWEEP_THUNDER_HEIGHT		(MAP_BLOCK_HEIGHT)
#define BOSS_SWEEP_THUNDER_CENTER_X		(MAP_BLOCK_WIDTH * 11.0f + BOSS_STAGE_CENTER_OFFSET_X)

#define BOSS_THUNDER_WARN_ALPHA			(1.0f)
#define BOSS_THUNDER_WARN_SCROLL_SPEED  (0.05f)
#define BOSS_THUNDER_DAMAGE				(35.0f) // 35.0f
#define BOSS_THUNDER_END_CHECK_PATTERN	(5)	   // ６フレーム目までコリジョン判定

// BOSSのイントロ演出
#define BOSS_WARNING_TEXT_PIC_SIZE_X	(750.0f * 2.0f)
#define BOSS_WARNING_TEXT_PIC_SIZE_Y	(180.0f * 2.0f)
#define BOSS_WARNING_TEXT_PIC_POS_X		(0.0f)
#define BOSS_WARNING_TEXT_PIC_POS_Y		(-150.0f)

#define BOSS_WARNING_LOOP				(2)
#define BOSS_WARNING_FRAME				(boss_warning_text.PATTERN_MAX * boss_warning_text.ANIME_SPEED * BOSS_WARNING_LOOP) // ２ループする予定

#define BOSS_WARNING_SHAKE_POWER		(15.0f)
#define BOSS_WARNING_SHAKE_FRAME		(8)

#define BOSS_APPEAR_SHAKE_POWER			(40.0f)
#define BOSS_APPEAR_SHAKE_FRAME			(20)

// AIMエフェクト
#define BOSS_AIMED_EFFECT_SCALE_RATIO	(1.5f)

// Boss Shoot
#define BOSS_BULLET_ATK					(15.0f)	 // テスト中ゼロ
#define BOSS_SHOOT_COOLDOWN				(15 * 60)
#define BOSS_SHOOT_DURATION_FRAME		(5 * 60)
#define BOSS_SHOOT_INTERVAL				(6) //頻度
#define BOSS_SHOOT_START_PATTERN		(0)
#define BOSS_SHOOT_RADIUS				(50.0f)
#define BOSS_SHOOT_ANGLE_STEP			(25.0f)
#define BOSS_BULLET_SPEED				(15.0f)
#define BOSS_BULLET_LIFE_FRAME			(3 * 60)
#define BOSS_BULLET_SIZE_X				(80.0f)
#define BOSS_BULLET_SIZE_Y				(80.0f)
#define BOSS_BULLET_COLLISION_SIZE_X	(40.0f)
#define BOSS_BULLET_COLLISION_SIZE_Y	(40.0f)

// Boss Hit Shake
#define BOSS_HIT_SHAKE_POWER			(8.0f)
#define BOSS_HIT_SHAKE_FRAME			(5)

// Boss Dead
#define BOSS_DEAD_FRAME					(3 * 60)
#define BOSS_DEAD_SHAKE_POWER			(15.0f)
#define BOSS_DEAD_SHAKE_FRAME			(BOSS_DEAD_FRAME)

#define BOSS_DEAD_EXPLOSION_INTERVAL	(10)
#define BOSS_DEAD_EXPLOSION_SIZE_X		(150.0f)
#define BOSS_DEAD_EXPLOSION_SIZE_Y		(150.0f)
#define BOSS_DEAD_EXPLOSION_SIZE_MIN	(0.8f)
#define BOSS_DEAD_EXPLOSION_SIZE_MAX	(2.0f)
#define BOSS_DEAD_EXPLOSION_RANGE_X		(260.0f * BOSS_PIC_SCALE)
#define BOSS_DEAD_EXPLOSION_RANGE_Y		(280.0f * BOSS_PIC_SCALE)

// Boss HP Bar
#define BOSS_HP_BAR_POS_X				(0.0f)
#define BOSS_HP_BAR_POS_Y				(SCREEN_HEIGHT / 2.0f - 40.0f)
#define BOSS_HP_BAR_SIZE_X				(800.0f)
#define BOSS_HP_BAR_SIZE_Y				(40.0f)
#define BOSS_HP_BAR_DELAY_LERP			(0.08f)
#define BOSS_HP_BAR_SHAKE_POWER			(20.0f)
#define BOSS_HP_BAR_SHAKE_FRAME			(20)

// =========================================================
// グローバル変数
// =========================================================
BOSS boss{};
BOSS_THUNDER bossThunder[BOSS_THUNDER_MAX]{};
BOSS_BULLET bossBullet[BOSS_BULLET_MAX]{};

const Pic_Data boss_aimed_effect { "rom:/Aimed_Effect.tga", 12, 12, 1, 4 };

unsigned int BossAimedEffectTextureId{};
int bossAimedEffectFrame{};

Float2 bossShakeOffset{};
float bossShakePower{};
int bossShakeFrame{};

// Boss Dead
BOSS_DEAD_EXPLOSION bossDeadExplosion[BOSS_DEAD_EXPLOSION_MAX]{};
unsigned int BossDeadTextureId{};
unsigned int BossDeadExplosionTextureId{};
int bossDeadFrame{};
int bossDeadExplosionTimer{};
bool isBossDead{};

// Boss HP Bar
float bossDrawHp{};
Float2 bossHpBarShakeOffset{};
float bossHpBarShakePower{};
int bossHpBarShakeFrame{};
unsigned int BossNameTextureId{};

// =========================================================
// プロトタイプ宣言
// =========================================================
void BossIdle();
void BossTeleport();
void BossRandom();
void BossLeftAoe();
void BossRightAoe();
void BossSweepRTL();
void BossShoot();

void ChangeBossState(BOSS_STATE state);
void SelectBossAction();
void SelectBossColor();

void UpdateBossCool();
void UpdateBossCollision();
bool CheckBossCollision(Float2 pos, Float2 size);

void SetBossThunder(float x, int warnFrame);
void SetBossLineThunder(float startX, float endX, int warnFrame);
void SetBossSweepThunder();
void SetBossHorizontalThunder(int row, int warnFrame);
void UpdateBossThunder();
void DrawBossThunder();

void SetBossBullet();
void UpdateBossBullet();
void DrawBossBullet();

int GetBossPattern();
bool IsBossAnimationEnd();
bool IsBossBasicPos();
float GetBossSweepRowY(int row);

void DrawBossAimedEffect();

void StartBossShake(float power, int frame);
void UpdateBossShake();

// Dead Methods
void StartBossDead();
void UpdateBossDead();
void DrawBossDead();

void SetBossDeadExplosion();
void UpdateBossDeadExplosion();
void DrawBossDeadExplosion();
void ResetBossDeadExplosion();

// Boss Bar
void UpdateBossHpBar();
void DrawBossHpBar();
void StartBossHpBarShake(float power, int frame);
void UpdateBossHpBarShake();

// =========================================================
// 初期化
// =========================================================
void InitializeBoss()
{
	for (int i = 0; i < BOSS_PIC_NUM; i++)
	{
		for (int j = 0; j < BOSS_COLOR_NUM; j++)
		{
			boss.TextureId[i][j] = LoadTexture(boss_pic[i][j].FILE_NAME);
		}
	}

	for (int i = 0; i < BOSS_EFFECT_PIC_NUM; i++)
	{
		boss.Effect_TextureId[i] = LoadTexture(boss_effect[i].FILE_NAME);
	}

	// Warning Arrow
	boss.ThunderWarnTextureId = LoadTexture(boss_thunder_warn_pic.FILE_NAME);

	// Warning Intro画像
	boss.WarningTextureId = LoadTexture(boss_warning_text.FILE_NAME);

	// Boss Bullet
	boss.BulletTextureId = LoadTexture(boss_bullet_pic.FILE_NAME);

	// Aim Effect
	BossAimedEffectTextureId =LoadTexture(boss_aimed_effect.FILE_NAME);
	bossAimedEffectFrame = 0;

	// Boss Dead
	BossDeadTextureId = LoadTexture("rom:/Boss_Dead.tga");
	BossDeadExplosionTextureId = LoadTexture(boss_dead_explosion.FILE_NAME);

	// Boss Bar
	BossNameTextureId = LoadTexture("rom:/Boss_Name.tga");

	ReloadBoss();
}

// =========================================================
// Stage再設定
// =========================================================
void ReloadBoss()
{
	unsigned int textureId[BOSS_PIC_NUM][BOSS_COLOR_NUM]{};
	unsigned int effectId[BOSS_EFFECT_PIC_NUM]{};
	unsigned int warningId = boss.WarningTextureId;
	unsigned int thunderWarnId = boss.ThunderWarnTextureId;
	unsigned int bulletId = boss.BulletTextureId;

	for (int i = 0; i < BOSS_PIC_NUM; i++)
	{
		for (int j = 0; j < BOSS_COLOR_NUM; j++)
		{
			textureId[i][j] = boss.TextureId[i][j];
		}
	}

	for (int i = 0; i < BOSS_EFFECT_PIC_NUM; i++)
	{
		effectId[i] = boss.Effect_TextureId[i];
	}

	boss = BOSS{};

	for (int i = 0; i < BOSS_PIC_NUM; i++)
	{
		for (int j = 0; j < BOSS_COLOR_NUM; j++)
		{
			boss.TextureId[i][j] = textureId[i][j];
		}
	}

	for (int i = 0; i < BOSS_EFFECT_PIC_NUM; i++)
	{
		boss.Effect_TextureId[i] = effectId[i];
	}

	boss.WarningTextureId = warningId;
	boss.ThunderWarnTextureId = thunderWarnId;
	boss.BulletTextureId = bulletId;

	ResetThunder();
	ResetBossBullet();

	bossDeadFrame = 0;
	bossDeadExplosionTimer = 0;
	isBossDead = false;

	// boss draw
	bossShakeOffset = MakeFloat2(0.0f, 0.0f);
	bossShakePower = 0.0f;
	bossShakeFrame = 0;

	// hp bar draw
	bossHpBarShakeOffset = MakeFloat2(0.0f, 0.0f);
	bossHpBarShakePower = 0.0f;
	bossHpBarShakeFrame = 0;

	// Boss Stage以外は使用しない
	if (GetCurrentGameStage() != GAME_STAGE_BOSS)
	{
		boss.use = false;
		return;
	}

	boss.pos = MakeFloat2(BOSS_BASIC_POS_X, BOSS_BASIC_POS_Y);
	boss.size = MakeFloat2(BOSS_PIC_SIZE_X * BOSS_PIC_SCALE, BOSS_PIC_SIZE_Y * BOSS_PIC_SCALE);

	boss.hp = BOSS_HP;
	bossDrawHp = BOSS_HP;

	boss.singleCool = 600;
	boss.randomCool = 360;
	boss.randomCount = 0;
	boss.randomTimer = 0;
	boss.sweepCool = 1200;
	boss.shootCool = 120;
	boss.shootFrame = 0;
	boss.shootTimer = 0;
	boss.shootAngle = 0.0f;
	boss.shootDirection = 1;

	boss.state = BOSS_STATE_IDLE;
	boss.nextState = BOSS_STATE_IDLE;
	boss.teleportType = BOSS_TELEPORT_NONE;

	boss.animeFrame = 0;
	boss.idleFrame = 0;
	boss.actionDone = false;

	boss.introState = BOSS_INTRO_NONE;
	boss.introFrame = 0;
	boss.introDone = false;

	boss.isBeingAimed = false;
	boss.aimedPart = BOSS_AIM_NONE;
	boss.use = true;

	SelectBossColor();
	UpdateBossCollision();
}

// =========================================================
// 更新
// =========================================================
void UpdateBoss()
{
	if (!boss.use) return;
	UpdateBossHpBar();
	if (isBossDead)
	{
		UpdateBossDead();
		return;
	}
	if (!boss.introDone) return;

	UpdateBossShake();
	UpdateBossCool();

	switch (boss.state)
	{
	case BOSS_STATE_IDLE: BossIdle();				break;
	case BOSS_STATE_TELEPORT: BossTeleport();		break;
	case BOSS_STATE_RANDOM: BossRandom();			break;
	case BOSS_STATE_LEFT_AOE: BossLeftAoe();		break;
	case BOSS_STATE_RIGHT_AOE: BossRightAoe();		break;
	case BOSS_STATE_SWEEP_RTL: BossSweepRTL();		break;
	case BOSS_STATE_SHOOT: BossShoot();				break;
	default: break;
	}

	boss.animeFrame++;

	UpdateBossCollision();
	UpdateBossThunder();
	UpdateBossBullet();
}

// =========================================================
// 描画
// =========================================================
void DrawBoss()
{
	if (!boss.use) return;
	if (isBossDead)
	{
		DrawBossDead();
		return;
	}

	int state = static_cast<int>(boss.state);
	int color = static_cast<int>(boss.COLORSTATE);
	if (state < 0 || state >= BOSS_PIC_NUM) state = BOSS_STATE_IDLE;
	if (color < 0 || color >= BOSS_COLOR_NUM) color = BOSS_RED;

	const Pic_Data& pic = boss_pic[state][color];
	int animeSpeed = pic.ANIME_SPEED;    if (animeSpeed <= 0) animeSpeed = 1;
	int patternMax = pic.PATTERN_MAX;    if (patternMax <= 0) patternMax = 1;
	int patternNumU = pic.PATTERN_NUM_U; if (patternNumU <= 0) patternNumU = 1;

	int frame = boss.animeFrame / animeSpeed;
	if (boss.state == BOSS_STATE_IDLE || boss.state == BOSS_STATE_SHOOT)
	{
		frame %= patternMax;
	}
	else if (frame >= patternMax)
	{
		frame = patternMax - 1;
	}

	float tx = pic.PATTERN_WIDTH * static_cast<float>(frame % patternNumU);
	float ty = pic.PATTERN_HIGHT * static_cast<float>(frame / patternNumU);
	DrawSpriteAnimation_Scroll(
		boss.pos.x + bossShakeOffset.x,
		boss.pos.y + bossShakeOffset.y,
		boss.size.x, boss.size.y,
		MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f),
		0.0f,
		tx, ty, pic.PATTERN_WIDTH, pic.PATTERN_HIGHT,
		boss.TextureId[state][color],
		true
	);

	DrawBossThunder();
	DrawBossBullet();
	if (boss.isBeingAimed)
	{
		bossAimedEffectFrame++;
		DrawBossAimedEffect();
	}
	else
	{
		bossAimedEffectFrame = 0;
	}
}

// =========================================================
// 終了処理
// =========================================================
void FinalizeBoss()
{
	for (int i = 0; i < BOSS_PIC_NUM; i++)
	{
		for (int j = 0; j < BOSS_COLOR_NUM; j++)
		{
			if (boss.TextureId[i][j] == 0) continue;

			UnloadTexture(boss.TextureId[i][j]);
			boss.TextureId[i][j] = 0;
		}
	}

	for (int i = 0; i < BOSS_EFFECT_PIC_NUM; i++)
	{
		if (boss.Effect_TextureId[i] == 0) continue;

		UnloadTexture(boss.Effect_TextureId[i]);
		boss.Effect_TextureId[i] = 0;
	}

	if (boss.WarningTextureId != 0)
	{
		UnloadTexture(boss.WarningTextureId);
		boss.WarningTextureId = 0;
	}

	if (boss.ThunderWarnTextureId != 0)
	{
		UnloadTexture(boss.ThunderWarnTextureId);
		boss.ThunderWarnTextureId = 0;
	}

	if (boss.BulletTextureId != 0)
	{
		UnloadTexture(boss.BulletTextureId);
		boss.BulletTextureId = 0;
	}

	if (BossAimedEffectTextureId != 0)
	{
		UnloadTexture(BossAimedEffectTextureId);
		BossAimedEffectTextureId = 0;
	}

	if (BossDeadTextureId != 0)
	{
		UnloadTexture(BossDeadTextureId);
		BossDeadTextureId = 0;
	}

	if (BossDeadExplosionTextureId != 0)
	{
		UnloadTexture(BossDeadExplosionTextureId);
		BossDeadExplosionTextureId = 0;
	}

	if (BossNameTextureId != 0)
	{
		UnloadTexture(BossNameTextureId);
		BossNameTextureId = 0;
	}

	bossAimedEffectFrame = 0;
	boss = BOSS{};
	ResetThunder();
	ResetBossBullet();

	bossShakeOffset = MakeFloat2(0.0f, 0.0f);
	bossShakePower = 0.0f;
	bossShakeFrame = 0;

	ResetBossDeadExplosion();
	bossDeadFrame = 0;
	bossDeadExplosionTimer = 0;
	isBossDead = false;

	bossDrawHp = 0.0f;
	bossHpBarShakeOffset = MakeFloat2(0.0f, 0.0f);
	bossHpBarShakePower = 0.0f;
	bossHpBarShakeFrame = 0;
}

// =========================================================
// 雷リセット
// =========================================================
void ResetThunder()
{
	for (int i = 0; i < BOSS_THUNDER_MAX; i++)
	{
		bossThunder[i] = BOSS_THUNDER{};
	}
}

// =========================================================
// Boss Bulletリセット
// =========================================================
void ResetBossBullet()
{
	for (int i = 0; i < BOSS_BULLET_MAX; i++)
	{
		bossBullet[i] = BOSS_BULLET{};
	}
}

// =========================================================
// Idle
// =========================================================
void BossIdle()
{
	boss.idleFrame++;
	if (boss.idleFrame < BOSS_IDLE_FRAME) return;

	SelectBossAction();
}

// =========================================================
// Select Boss action
// =========================================================
void SelectBossAction()
{
	PLAYER* player = GetPlayer();
	if (player == nullptr) return;

	bool canSingle = boss.singleCool <= 0;
	bool canRandom = boss.randomCool <= 0;
	bool canSweep = boss.sweepCool <= 0;
	bool canShoot = boss.shootCool <= 0;

	int actionCount = 0;

	if (canSingle) actionCount++;
	if (canRandom) actionCount++;
	if (canSweep) actionCount++;
	if (canShoot) actionCount++;

	if (actionCount <= 0)
	{
		boss.idleFrame = 0;
		return;
	}

	int selectedIndex = rand() % actionCount;
	BOSS_STATE selectedState = BOSS_STATE_IDLE;

	if (canRandom)
	{
		if (selectedIndex == 0)
		{
			selectedState = BOSS_STATE_RANDOM;
		}
		else
		{
			selectedIndex--;
		}
	}

	if (selectedState == BOSS_STATE_IDLE && canSweep)
	{
		if (selectedIndex == 0)
		{
			selectedState = BOSS_STATE_SWEEP_RTL;
		}
		else
		{
			selectedIndex--;
		}
	}

	if (selectedState == BOSS_STATE_IDLE && canShoot)
	{
		if (selectedIndex == 0)
		{
			selectedState = BOSS_STATE_SHOOT;
		}
		else
		{
			selectedIndex--;
		}
	}

	// Random、Sweep、ShootはBasic位置で実行する
	if (selectedState == BOSS_STATE_RANDOM ||
		selectedState == BOSS_STATE_SWEEP_RTL ||
		selectedState == BOSS_STATE_SHOOT)
	{
		if (IsBossBasicPos())
		{
			ChangeBossState(selectedState);
		}
		else
		{
			boss.teleportType = BOSS_TELEPORT_BASIC;
			boss.nextState = selectedState;
			ChangeBossState(BOSS_STATE_TELEPORT);
		}

		return;
	}

	// Single AOE
	if (player->pos.x < BOSS_BASIC_POS_X)
	{
		boss.teleportType = BOSS_TELEPORT_RIGHT;
		boss.nextState = BOSS_STATE_LEFT_AOE;
	}
	else
	{
		boss.teleportType = BOSS_TELEPORT_LEFT;
		boss.nextState = BOSS_STATE_RIGHT_AOE;
	}

	ChangeBossState(BOSS_STATE_TELEPORT);
}

// =========================================================
// Teleport
// =========================================================
void BossTeleport()
{
	int pattern = GetBossPattern();

	// 8枚目の透明Frameで移動
	if (!boss.actionDone && pattern >= BOSS_TELEPORT_PATTERN)
	{
		switch (boss.teleportType)
		{
		case BOSS_TELEPORT_LEFT:
			boss.pos.x = BOSS_TELEPORT_LEFT_POS_X;
			break;

		case BOSS_TELEPORT_RIGHT:
			boss.pos.x = BOSS_TELEPORT_RIGHT_POS_X;
			break;

		case BOSS_TELEPORT_BASIC:
			boss.pos.x = BOSS_BASIC_POS_X;
			break;

		default:
			break;
		}

		boss.pos.y = BOSS_BASIC_POS_Y;
		boss.actionDone = true;
	}

	if (IsBossAnimationEnd())
	{
		ChangeBossState(boss.nextState);
	}
}

// =========================================================
// Random攻撃
// =========================================================
void BossRandom()
{
	int pattern = GetBossPattern();

	// 5枚目から攻撃開始
	if (pattern < BOSS_RANDOM_PATTERN) return;

	if (boss.randomCount >= BOSS_RANDOM_COUNT)
	{
		if (IsBossAnimationEnd())
		{
			ChangeBossState(BOSS_STATE_IDLE);
		}

		return;
	}

	if (boss.randomTimer > 0)
	{
		boss.randomTimer--;
		return;
	}

	int blockCount = static_cast<int>((BOSS_MAP_RIGHT_BOUND_X - BOSS_MAP_LEFT_BOUND_X) / MAP_BLOCK_WIDTH ) + 1;
	if (blockCount <= 0) return;

	float x = BOSS_MAP_LEFT_BOUND_X + static_cast<float>(rand() % blockCount) * MAP_BLOCK_WIDTH;
	SetBossThunder(x, BOSS_RANDOM_THUNDER_WARN_FRAME);

	boss.randomCount++;
	boss.randomTimer = BOSS_RANDOM_INTERVAL - 1;

	if (boss.randomCount >= BOSS_RANDOM_COUNT)
	{
		boss.randomCool = BOSS_RANDOM_COOLDOWN;
	}
}

// =========================================================
// Left AOE
// =========================================================
void BossLeftAoe()
{
	int pattern = GetBossPattern();

	// 13枚目で雷を生成
	if (!boss.actionDone && pattern >= BOSS_SINGLE_HAND_PATTERN)
	{
		float endX = boss.pos.x - MAP_BLOCK_WIDTH;
		SetBossLineThunder(BOSS_MAP_LEFT_BOUND_X, endX, BOSS_SINGLE_HAND_WARN_FRAME);

		boss.singleCool = BOSS_SINGLE_HAND_AOE_COOLDOWN;
		boss.actionDone = true;
	}

	if (IsBossAnimationEnd())
	{
		ChangeBossState(BOSS_STATE_IDLE);
	}
}

// =========================================================
// Right AOE
// =========================================================
void BossRightAoe()
{
	int pattern = GetBossPattern();

	// 13枚目で雷を生成
	if (!boss.actionDone && pattern >= BOSS_SINGLE_HAND_PATTERN)
	{
		float startX = boss.pos.x + MAP_BLOCK_WIDTH;
		SetBossLineThunder(startX, BOSS_MAP_RIGHT_BOUND_X, BOSS_SINGLE_HAND_WARN_FRAME);

		boss.singleCool = BOSS_SINGLE_HAND_AOE_COOLDOWN;
		boss.actionDone = true;
	}

	if (IsBossAnimationEnd())
	{
		ChangeBossState(BOSS_STATE_IDLE);
	}
}

// =========================================================
// Sweep RTL attack
// =========================================================
void BossSweepRTL()
{
	int pattern = GetBossPattern();

	if (!boss.actionDone && pattern >= BOSS_SWEEP_PATTERN)
	{
		SetBossSweepThunder();
		boss.sweepCool = BOSS_SWEEP_RTL_COOLDOWN;
		boss.actionDone = true;
	}

	if (IsBossAnimationEnd())
	{
		ChangeBossState(BOSS_STATE_IDLE);
	}
}

// =========================================================
// Shoot attack
// =========================================================
void BossShoot()
{
	int pattern = GetBossPattern();
	if (pattern < BOSS_SHOOT_START_PATTERN) return;

	boss.shootFrame++;
	if (boss.shootFrame >= BOSS_SHOOT_DURATION_FRAME)
	{
		boss.shootCool = BOSS_SHOOT_COOLDOWN;
		ChangeBossState(BOSS_STATE_IDLE);
		return;
	}

	if (boss.shootTimer > 0)
	{
		boss.shootTimer--;
		return;
	}

	SetBossBullet();

	boss.shootTimer = BOSS_SHOOT_INTERVAL - 1;
	boss.shootAngle += BOSS_SHOOT_ANGLE_STEP * static_cast<float>(boss.shootDirection);

	if (boss.shootAngle >= 360.0f)
	{
		boss.shootAngle -= 360.0f;
	}
	else if (boss.shootAngle < 0.0f)
	{
		boss.shootAngle += 360.0f;
	}
}

// =========================================================
// State変更
// =========================================================
void ChangeBossState(BOSS_STATE state)
{
	boss.state = state;

	boss.animeFrame = 0;
	boss.idleFrame = 0;
	boss.randomCount = 0;
	boss.randomTimer = 0;
	boss.shootFrame = 0;
	boss.shootTimer = 0;
	boss.shootAngle = 0.0f;

	boss.actionDone = false;

	if (state == BOSS_STATE_SHOOT)
	{
		boss.shootDirection = rand() % 2 == 0 ? 1 : -1;
	}

	if (state == BOSS_STATE_IDLE)
	{
		SelectBossColor();
		boss.teleportType = BOSS_TELEPORT_NONE;
		boss.nextState = BOSS_STATE_IDLE;
	}
}

// =========================================================
// Color選択
// =========================================================
void SelectBossColor()
{
	boss.COLORSTATE = static_cast<BOSS_COLOR_STATE>(rand() % BOSS_COLOR_MAX);
}

// =========================================================
// Cooldown更新
// =========================================================
void UpdateBossCool()
{
	if (boss.singleCool > 0) boss.singleCool--;
	if (boss.randomCool > 0) boss.randomCool--;
	if (boss.sweepCool > 0) boss.sweepCool--;
	if (boss.shootCool > 0) boss.shootCool--;
}

// =========================================================
// Boss Collision更新
// =========================================================
void UpdateBossCollision()
{
	float scale = BOSS_PIC_SCALE;

	// 頭部Collision
	boss.HeadCollisionSize = MakeFloat2(BOSS_HEAD_COLLISION_SIZE_X * scale, BOSS_HEAD_COLLISION_SIZE_Y * scale);
	boss.HeadCollisionPosition = MakeFloat2(boss.pos.x, boss.pos.y + BOSS_HEAD_COLLISION_OFFSET_Y * scale);

	// 腹部Ruby Collision
	boss.RubyCollisionSize = MakeFloat2(BOSS_RUBY_COLLISION_SIZE_X * scale, BOSS_RUBY_COLLISION_SIZE_Y * scale);
	boss.RubyCollisionPosition = MakeFloat2(boss.pos.x, boss.pos.y + BOSS_RUBY_COLLISION_OFFSET_Y * scale);

	// 現在Aim中の部位をAim Assist用Collisionへ反映
	switch (boss.aimedPart)
	{
	case BOSS_AIM_HEAD:
		boss.CollisionPosition = boss.HeadCollisionPosition;
		boss.CollisionSize = boss.HeadCollisionSize;
		break;

	case BOSS_AIM_RUBY:
		boss.CollisionPosition = boss.RubyCollisionPosition;
		boss.CollisionSize = boss.RubyCollisionSize;
		break;

	default:
		boss.CollisionPosition = MakeFloat2(0.0f, 0.0f);
		boss.CollisionSize = MakeFloat2(0.0f, 0.0f);
		break;
	}
}

// =========================================================
// Boss共通Collision判定
// =========================================================
bool CheckBossCollision(Float2 pos, Float2 size)
{
	if (!boss.use) return false;
	if (!boss.introDone) return false;

	if (CheckBoxCollider(pos, boss.HeadCollisionPosition, size, boss.HeadCollisionSize)) return true; // 頭部
	if (CheckBoxCollider(pos, boss.RubyCollisionPosition, size, boss.RubyCollisionSize))return true; // 腹部Ruby

	return false;
}

// =========================================================
// 雷生成
// =========================================================
void SetBossThunder(float x, int warnFrame)
{
	if (warnFrame <= 0) warnFrame = 1;

	for (int i = 0; i < BOSS_THUNDER_MAX; i++)
	{
		if (bossThunder[i].use) continue;

		BOSS_THUNDER* thunder = &bossThunder[i];
		*thunder = BOSS_THUNDER{};

		thunder->pos = MakeFloat2(x, BOSS_THUNDER_POS_Y);
		thunder->size = MakeFloat2(BOSS_THUNDER_SIZE_X, BOSS_THUNDER_SIZE_Y);

		thunder->CollisionPosition = thunder->pos;
		thunder->CollisionSize = thunder->size;
		thunder->damage = BOSS_THUNDER_DAMAGE;
		thunder->picId = rand() % BOSS_EFFECT_PIC_NUM;

		thunder->warnFrame = 0;
		thunder->warnMaxFrame = warnFrame;
		thunder->animeFrame = 0;
		thunder->warnScroll = 0.0f;

		thunder->state = BOSS_THUNDER_WARN;
		thunder->isSEplayed = false;
		thunder->isHorizontal = false;
		thunder->use = true;

		return;
	}
}

// =========================================================
// 雷範囲生成
// =========================================================
void SetBossLineThunder(float startX, float endX, int warnFrame)
{
	if (startX > endX)
	{
		float temp = startX;
		startX = endX;
		endX = temp;
	}

	for (float x = startX; x <= endX + 0.1f; x += MAP_BLOCK_WIDTH)
	{
		SetBossThunder(x, warnFrame);
	}
}

// =========================================================
// Set horizontal thunder
// =========================================================
void SetBossHorizontalThunder(int row, int warnFrame)
{
	if (warnFrame <= 0) warnFrame = 1;

	for (int i = 0; i < BOSS_THUNDER_MAX; i++)
	{
		if (bossThunder[i].use) continue;

		BOSS_THUNDER* thunder = &bossThunder[i];
		*thunder = BOSS_THUNDER{};

		thunder->pos = MakeFloat2(BOSS_SWEEP_THUNDER_CENTER_X, GetBossSweepRowY(row));
		thunder->size = MakeFloat2(BOSS_SWEEP_THUNDER_WIDTH, BOSS_SWEEP_THUNDER_HEIGHT);

		thunder->CollisionPosition = thunder->pos;
		thunder->CollisionSize = thunder->size;
		thunder->damage = BOSS_THUNDER_DAMAGE;
		thunder->picId = rand() % BOSS_EFFECT_PIC_NUM;
		thunder->warnFrame = 0;
		thunder->warnMaxFrame = warnFrame;
		thunder->animeFrame = 0;
		thunder->warnScroll = 0.0f;
		thunder->state = BOSS_THUNDER_WARN;
		thunder->isSEplayed = false;
		thunder->isHorizontal = true;
		thunder->use = true;

		return;
	}
}

// =========================================================
// Set Sweep horizontal thunders // Row 2 and Row 9 are fixed // Select 3 unique rows from Row 3 to Row 8
// =========================================================
void SetBossSweepThunder()
{
	SetBossHorizontalThunder(BOSS_SWEEP_FIXED_ROW_TOP, BOSS_SWEEP_RTL_WARN_FRAME);
	SetBossHorizontalThunder(BOSS_SWEEP_FIXED_ROW_BOTTOM, BOSS_SWEEP_RTL_WARN_FRAME);

	int rowList[3] { 1, 2, 3 };

	for (int i = 2; i > 0; i--)
	{
		int randomIndex = rand() % (i + 1);

		int temp = rowList[i];
		rowList[i] = rowList[randomIndex];
		rowList[randomIndex] = temp;
	}

	for (int i = 0; i < BOSS_SWEEP_RANDOM_ROW_COUNT; i++)
	{
		switch (rowList[i])
		{
		case 1:
			SetBossHorizontalThunder(2, BOSS_SWEEP_RTL_WARN_FRAME);
			SetBossHorizontalThunder(3, BOSS_SWEEP_RTL_WARN_FRAME);
			break;
		case 2:
			SetBossHorizontalThunder(5, BOSS_SWEEP_RTL_WARN_FRAME);
			SetBossHorizontalThunder(6, BOSS_SWEEP_RTL_WARN_FRAME);
			break;
		case 3:
		default:
			SetBossHorizontalThunder(8, BOSS_SWEEP_RTL_WARN_FRAME);
			SetBossHorizontalThunder(9, BOSS_SWEEP_RTL_WARN_FRAME);
			break;
		}
	}
}

// =========================================================
// 雷更新
// =========================================================
void UpdateBossThunder()
{
	PLAYER* player = GetPlayer();
	if (player == nullptr) return;

	for (int i = 0; i < BOSS_THUNDER_MAX; i++)
	{
		BOSS_THUNDER* thunder = &bossThunder[i];

		if (!thunder->use) continue;
		if (thunder->state == BOSS_THUNDER_WARN)
		{
			thunder->warnFrame++;
			if (thunder->warnFrame >= thunder->warnMaxFrame)
			{
				thunder->state = BOSS_THUNDER_ATTACK;
				thunder->animeFrame = 0;
			}

			if (thunder->isHorizontal)
			{
				thunder->warnScroll += BOSS_THUNDER_WARN_SCROLL_SPEED;
				if (thunder->warnScroll >= 1.0f) thunder->warnScroll -= 1.0f;
			}
			else
			{
				thunder->warnScroll -= BOSS_THUNDER_WARN_SCROLL_SPEED;
				if (thunder->warnScroll < 0.0f) thunder->warnScroll += 1.0f;
			}

			continue;
		}

		if (!thunder->isSEplayed)
		{
			int thunderSEindex = rand() % 4;
			switch (thunderSEindex)
			{
			case 0:
				PlaySE(SE_Boss_Thunder_01);
				break;
			case 1:
				PlaySE(SE_Boss_Thunder_02);
				break;
			case 2:
				PlaySE(SE_Boss_Thunder_03);
				break;
			case 3:
			default:
				PlaySE(SE_Boss_Thunder_04);
				break;
			}

			thunder->isSEplayed = true;
		}

		const Pic_Data& pic = boss_effect[thunder->picId];

		int animeSpeed = pic.ANIME_SPEED;
		if (animeSpeed <= 0)animeSpeed = 1;

		thunder->animeFrame++;

		Float2 thunderFinalCollisionSize{};
		if (thunder->isHorizontal)
		{
			thunderFinalCollisionSize =
				MakeFloat2(
					thunder->CollisionSize.x,
					thunder->CollisionSize.y - BOSS_THUNDER_COLLISION_HINCH
				);
		}
		else
		{
			thunderFinalCollisionSize =
				MakeFloat2(
					thunder->CollisionSize.x - BOSS_THUNDER_COLLISION_HINCH,
					thunder->CollisionSize.y
				);
		}

		if (CheckBoxCollider(
			thunder->CollisionPosition, player->CollisionPosition,
			thunderFinalCollisionSize, player->CollisionSize) &&
			thunder->animeFrame <= BOSS_THUNDER_END_CHECK_PATTERN * animeSpeed)
		{
			SetPlayerHit(thunder->CollisionPosition, thunder->damage);
		}

		if (thunder->animeFrame >= pic.PATTERN_MAX * animeSpeed)
		{
			*thunder = BOSS_THUNDER{};
		}
	}
}

// =========================================================
// 雷描画
// =========================================================
void DrawBossThunder()
{
	for (int i = 0; i < BOSS_THUNDER_MAX; i++)
	{
		BOSS_THUNDER* thunder = &bossThunder[i];
		if (!thunder->use) continue;
		if (thunder->state == BOSS_THUNDER_WARN)
		{
			int warnMaxFrame = thunder->warnMaxFrame;
			if (warnMaxFrame <= 0) warnMaxFrame = 1;

			float rate = static_cast<float>(thunder->warnFrame) / static_cast<float>(warnMaxFrame);
			if (rate < 0.0f) rate = 0.0f;
			if (rate > 1.0f) rate = 1.0f;

			if (thunder->isHorizontal)
			{
				float repeatV = thunder->size.x / BOSS_THUNDER_SIZE_X;

				DrawSpriteAnimation_Scroll(
					thunder->pos.x, thunder->pos.y,
					thunder->size.y, thunder->size.x,
					MakeFloat4(1.0f, 1.0f, 1.0f, BOSS_THUNDER_WARN_ALPHA * rate), Deg2Rad(90.0f),
					0.0f, thunder->warnScroll,
					1.0f, repeatV,
					boss.ThunderWarnTextureId,
					true
				);
			}
			else
			{
				float repeatV = thunder->size.y / BOSS_THUNDER_SIZE_X;

				DrawSpriteAnimation_Scroll(
					thunder->pos.x, thunder->pos.y,
					thunder->size.x, thunder->size.y,
					MakeFloat4(1.0f, 1.0f, 1.0f, BOSS_THUNDER_WARN_ALPHA * rate), 0.0f,
					0.0f, thunder->warnScroll,
					1.0f, repeatV,
					boss.ThunderWarnTextureId,
					true
				);
			}

			continue;
		}

		const Pic_Data& pic = boss_effect[thunder->picId];
		int animeSpeed = pic.ANIME_SPEED;
		if (animeSpeed <= 0) animeSpeed = 1;

		int patternMax = pic.PATTERN_MAX;
		if (patternMax <= 0) patternMax = 1;

		int patternNumU = pic.PATTERN_NUM_U;
		if (patternNumU <= 0) patternNumU = 1;

		int frame = thunder->animeFrame / animeSpeed;
		if (frame >= patternMax) frame = patternMax - 1;

		float tx = pic.PATTERN_WIDTH * static_cast<float>(frame % patternNumU);
		float ty = pic.PATTERN_HIGHT * static_cast<float>(frame / patternNumU);

		float drawSizeX = thunder->size.x;
		float drawSizeY = thunder->size.y;
		float rotation = 0.0f;

		if (thunder->isHorizontal)
		{
			drawSizeX = thunder->size.y;
			drawSizeY = thunder->size.x;
			rotation = Deg2Rad(90.0f);
		}

		DrawSpriteAnimation_Scroll(
			thunder->pos.x, thunder->pos.y,
			drawSizeX, drawSizeY,
			MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f), rotation,
			tx, ty,
			pic.PATTERN_WIDTH, pic.PATTERN_HIGHT,
			boss.Effect_TextureId[thunder->picId],
			true
		);
	}
}

// =========================================================
// Set Boss Bullet
// =========================================================
void SetBossBullet()
{
	float angle = Deg2Rad(boss.shootAngle);
	Float2 direction = MakeFloat2(cosf(angle), sinf(angle));

	Float2 headPosition = boss.HeadCollisionPosition;
	Float2 startPosition = MakeFloat2(
		headPosition.x + direction.x * BOSS_SHOOT_RADIUS,
		headPosition.y + direction.y * BOSS_SHOOT_RADIUS
	);

	for (int i = 0; i < BOSS_BULLET_MAX; i++)
	{
		if (bossBullet[i].use) continue;

		BOSS_BULLET* bullet = &bossBullet[i];
		*bullet = BOSS_BULLET{};

		bullet->pos = startPosition;
		bullet->vel = MakeFloat2(
			direction.x * BOSS_BULLET_SPEED,
			direction.y * BOSS_BULLET_SPEED
		);

		bullet->size = MakeFloat2(
			BOSS_BULLET_SIZE_X,
			BOSS_BULLET_SIZE_Y
		);

		bullet->CollisionPosition = bullet->pos;
		bullet->CollisionSize = MakeFloat2(
			BOSS_BULLET_COLLISION_SIZE_X,
			BOSS_BULLET_COLLISION_SIZE_Y
		);

		bullet->damage = BOSS_BULLET_ATK;
		bullet->rotation = angle;
		bullet->animeFrame = 0;
		bullet->lifeFrame = BOSS_BULLET_LIFE_FRAME;
		bullet->use = true;

		return;
	}
}

// =========================================================
// Boss Bullet更新
// =========================================================
void UpdateBossBullet()
{
	PLAYER* player = GetPlayer();
	if (player == nullptr) return;

	for (int i = 0; i < BOSS_BULLET_MAX; i++)
	{
		BOSS_BULLET* bullet = &bossBullet[i];

		if (!bullet->use) continue;

		bullet->pos.x += bullet->vel.x;
		bullet->pos.y += bullet->vel.y;

		bullet->CollisionPosition = bullet->pos;

		bullet->animeFrame++;
		bullet->lifeFrame--;

		if (CheckBoxCollider(
			bullet->CollisionPosition, player->CollisionPosition,
			bullet->CollisionSize, player->CollisionSize))
		{
			if (player->invincibleTimer > 0) continue;

			SetPlayerHit(bullet->CollisionPosition, bullet->damage);
			*bullet = BOSS_BULLET{};
			continue;
		}

		if (bullet->lifeFrame <= 0)
		{
			*bullet = BOSS_BULLET{};
		}
	}
}

// =========================================================
// Boss Bullet描画
// =========================================================
void DrawBossBullet()
{
	if (boss.BulletTextureId == 0) return;

	int animeSpeed = boss_bullet_pic.ANIME_SPEED;
	if (animeSpeed <= 0) animeSpeed = 1;
	int patternMax = boss_bullet_pic.PATTERN_MAX;
	if (patternMax <= 0) patternMax = 1;
	int patternNumU = boss_bullet_pic.PATTERN_NUM_U;
	if (patternNumU <= 0) patternNumU = 1;

	for (int i = 0; i < BOSS_BULLET_MAX; i++)
	{
		BOSS_BULLET* bullet = &bossBullet[i];

		if (!bullet->use) continue;

		int frame = (bullet->animeFrame / animeSpeed) % patternMax;
		float tx = boss_bullet_pic.PATTERN_WIDTH * static_cast<float>(frame % patternNumU);
		float ty = boss_bullet_pic.PATTERN_HIGHT * static_cast<float>(frame / patternNumU);

		DrawSpriteAnimation_Scroll(
			bullet->pos.x, bullet->pos.y,
			bullet->size.x, bullet->size.y,
			MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f),
			bullet->rotation,
			tx, ty,
			boss_bullet_pic.PATTERN_WIDTH,
			boss_bullet_pic.PATTERN_HIGHT,
			boss.BulletTextureId,
			true
		);
	}
}

// =========================================================
// Player Bulletとの当たり判定
// =========================================================
bool BulletBossCollision(Float2 pos, Float2 size, float damage, int color)
{
	if (!boss.use) return false;
	if (isBossDead) return false;
	if (!boss.introDone) return false;
	if (boss.state == BOSS_STATE_TELEPORT) return false;

	// HeadまたはRubyに当たったか
	if (!CheckBossCollision(pos, size)) return false;

	float finalDamage = damage;
	bool isDoubleDamage = false;

	if (boss.state != BOSS_STATE_IDLE)
	{
		// IDLE以外は同じ色でダメージ2倍
		bool sameColor = 
			(boss.COLORSTATE == BOSS_RED && color == BULLET_COLOR_RED) ||
			(boss.COLORSTATE == BOSS_BLUE && color == BULLET_COLOR_BLUE);

		if (sameColor)
		{
			finalDamage *= 2.0f;
			isDoubleDamage = true;
		}
	}

	boss.hp -= finalDamage;
	if (boss.hp < 0.0f) boss.hp = 0.0f;

	if (isDoubleDamage)
	{
		StartBossHpBarShake(
			BOSS_HP_BAR_SHAKE_POWER,
			BOSS_HP_BAR_SHAKE_FRAME
		);
	}

	if (boss.hp <= 0.0f)
	{
		StartBossDead();
	}
	else
	{
		StartBossShake(
			BOSS_HIT_SHAKE_POWER,
			BOSS_HIT_SHAKE_FRAME
		);
	}

	return true;
}

// =========================================================
// 現在Pattern取得
// =========================================================
int GetBossPattern()
{
	int state = static_cast<int>(boss.state);
	int color = static_cast<int>(boss.COLORSTATE);

	if (state < 0 || state >= BOSS_PIC_NUM) state = BOSS_STATE_IDLE;
	if (color < 0 || color >= BOSS_COLOR_NUM) color = BOSS_RED;

	int animeSpeed = boss_pic[state][color].ANIME_SPEED;
	if (animeSpeed <= 0) animeSpeed = 1;

	return boss.animeFrame / animeSpeed;
}

// =========================================================
// Sweep横雷のY座標取得
// =========================================================
float GetBossSweepRowY(int row)
{
	return static_cast<float>(row) * MAP_BLOCK_HEIGHT + BOSS_STAGE_CENTER_OFFSET_Y;
}

// =========================================================
// Animation終了判定, 終了してから次の状態にいく
// =========================================================
bool IsBossAnimationEnd()
{
	int state = static_cast<int>(boss.state);
	int color = static_cast<int>(boss.COLORSTATE);

	if (state < 0 || state >= BOSS_PIC_NUM) state = BOSS_STATE_IDLE;
	if (color < 0 || color >= BOSS_COLOR_NUM) color = BOSS_RED;

	const Pic_Data& pic = boss_pic[state][color];

	int animeSpeed = pic.ANIME_SPEED;
	if (animeSpeed <= 0) animeSpeed = 1;

	return boss.animeFrame >= pic.PATTERN_MAX * animeSpeed - 1;
}

// =========================================================
// Basic位置判定
// =========================================================
bool IsBossBasicPos()
{
	return
		fabsf(boss.pos.x - BOSS_BASIC_POS_X) < 0.1f &&
		fabsf(boss.pos.y - BOSS_BASIC_POS_Y) < 0.1f;
}

// =========================================================
// BOSS INTRO演出
// =========================================================
void StartBossIntro()
{
	if (GetCurrentGameStage() != GAME_STAGE_BOSS) return;
	if (!boss.use) return;
	if (boss.introDone) return;
	if (boss.introState != BOSS_INTRO_NONE) return;

	boss.introState = BOSS_INTRO_WARNING;
	boss.introFrame = 0;

	boss.state = BOSS_STATE_IDLE;
	boss.animeFrame = 0;
	boss.idleFrame = 0;
	boss.actionDone = false;

	boss.pos = MakeFloat2(BOSS_BASIC_POS_X, BOSS_BASIC_POS_Y);

	PLAYER* player = GetPlayer();
	if (player != nullptr)
	{
		// Playerを出生位置で待機
		player->state = PLAYER_STATE_IDLE;
		player->vel = MakeFloat2(0.0f, 0.0f);
		player->exVel = MakeFloat2(0.0f, 0.0f);
		player->isAiming = false;
	}

	PlaySE(SE_Boss_Roar);
}

// =========================================================
// Boss登場更新
// =========================================================
void UpdateBossIntro()
{
	if (GetCurrentGameStage() != GAME_STAGE_BOSS) return;
	if (!boss.use) return;

	switch (boss.introState)
	{
	case BOSS_INTRO_WARNING:
		// 弱いCamera Shake
		if (boss.introFrame % BOSS_WARNING_SHAKE_FRAME == 0)
		{
			StartCameraShake(
				BOSS_WARNING_SHAKE_POWER,
				BOSS_WARNING_SHAKE_FRAME
			);
		}

		boss.introFrame++;

		// Warningを2回再生後、Teleportへ移行
		if (boss.introFrame >= BOSS_WARNING_FRAME)
		{
			boss.introState = BOSS_INTRO_TELEPORT;
			boss.introFrame = 0;

			boss.state = BOSS_STATE_TELEPORT;
			boss.animeFrame = 0;
			boss.actionDone = false;

			boss.teleportType = BOSS_TELEPORT_BASIC;
			boss.nextState = BOSS_STATE_IDLE;
		}
		break;

	case BOSS_INTRO_TELEPORT:
	{
		int color = static_cast<int>(boss.COLORSTATE);
		if (color < 0 || color >= BOSS_COLOR_NUM)
		{
			color = BOSS_RED;
		}

		const Pic_Data& pic = boss_pic[BOSS_STATE_TELEPORT][color];

		int animeSpeed = pic.ANIME_SPEED;
		if (animeSpeed <= 0) animeSpeed = 1;

		int pattern = boss.animeFrame / animeSpeed;

		// 8枚目でBasic位置へ移動
		if (!boss.actionDone &&
			pattern >= BOSS_TELEPORT_PATTERN)
		{
			boss.pos = MakeFloat2(BOSS_BASIC_POS_X, BOSS_BASIC_POS_Y);
			boss.actionDone = true;
		}

		boss.animeFrame++;

		// Teleport終了
		if (boss.animeFrame >= pic.PATTERN_MAX * animeSpeed)
		{
			
			boss.introState = BOSS_INTRO_END;
			boss.introFrame = 0;
			boss.introDone = true;

			boss.state = BOSS_STATE_IDLE;
			boss.animeFrame = 0;
			boss.idleFrame = 0;
			boss.actionDone = false;

			boss.teleportType = BOSS_TELEPORT_NONE;
			boss.nextState = BOSS_STATE_IDLE;

			// 登場完了時のCamera Shake
			PlaySE(SE_Boss_Thunder_01);
			PlaySE(SE_Boss_Thunder_04);
			StartCameraShake(BOSS_APPEAR_SHAKE_POWER, BOSS_APPEAR_SHAKE_FRAME);
		}
		break;
	}

	case BOSS_INTRO_NONE:
	case BOSS_INTRO_END:
	default:
		break;
	}
}

// =========================================================
// Warning描画
// =========================================================
void DrawBossWarning()
{
	if (!boss.use) return;
	if (boss.introState != BOSS_INTRO_WARNING) return;
	if (boss.WarningTextureId == 0) return;

	int animeSpeed = boss_warning_text.ANIME_SPEED;
	if (animeSpeed <= 0) animeSpeed = 1;

	int patternMax = boss_warning_text.PATTERN_MAX;
	if (patternMax <= 0) patternMax = 1;

	int patternNumU = boss_warning_text.PATTERN_NUM_U;
	if (patternNumU <= 0) patternNumU = 1;

	// 32Patternを2回ループ
	int frame = (boss.introFrame / animeSpeed) % patternMax;
	float tx = boss_warning_text.PATTERN_WIDTH * static_cast<float>(frame % patternNumU);
	float ty = boss_warning_text.PATTERN_HIGHT * static_cast<float>(frame / patternNumU);

	DrawSpriteAnimation(
		BOSS_WARNING_TEXT_PIC_POS_X, BOSS_WARNING_TEXT_PIC_POS_Y,
		BOSS_WARNING_TEXT_PIC_SIZE_X, BOSS_WARNING_TEXT_PIC_SIZE_Y,
		MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f ), 0.0f,
		tx, ty,
		boss_warning_text.PATTERN_WIDTH,
		boss_warning_text.PATTERN_HIGHT,
		boss.WarningTextureId
	);
}

// =========================================================
// Boss登場状態取得
// =========================================================
bool GetBossIntro()
{
	if (GetCurrentGameStage() != GAME_STAGE_BOSS) return false;
	if (!boss.use) return false;

	return
		boss.introState == BOSS_INTRO_WARNING ||
		boss.introState == BOSS_INTRO_TELEPORT;
}

// =========================================================
// Boss Aimed Effect描画
// =========================================================
void DrawBossAimedEffect()
{
	if (!boss.use) return;
	if (!boss.introDone) return;
	if (!boss.isBeingAimed) return;
	if (boss.state == BOSS_STATE_TELEPORT) return;

	Float2 aimedPos{};
	Float2 aimedSize{};

	switch (boss.aimedPart)
	{
	case BOSS_AIM_HEAD:
		aimedPos = boss.HeadCollisionPosition;
		aimedSize = boss.HeadCollisionSize;
		break;

	case BOSS_AIM_RUBY:
		aimedPos = boss.RubyCollisionPosition;
		aimedSize = boss.RubyCollisionSize;
		break;

	default:
		return;
	}

	if (aimedSize.x <= 0.0f || aimedSize.y <= 0.0f) return;

	int animeSpeed = boss_aimed_effect.ANIME_SPEED;
	if (animeSpeed <= 0) animeSpeed = 1;
	int patternMax = boss_aimed_effect.PATTERN_MAX;
	if (patternMax <= 0) patternMax = 1;
	int patternNumU = boss_aimed_effect.PATTERN_NUM_U;
	if (patternNumU <= 0) patternNumU = 1;

	int frame = (bossAimedEffectFrame / animeSpeed) % patternMax;
	float tx = boss_aimed_effect.PATTERN_WIDTH * static_cast<float>(frame % patternNumU);
	float ty = boss_aimed_effect.PATTERN_HIGHT * static_cast<float>(frame / patternNumU);

	float drawSize = aimedSize.x > aimedSize.y ?
		aimedSize.x :
		aimedSize.y;
	drawSize *= BOSS_AIMED_EFFECT_SCALE_RATIO;

	DrawSpriteAnimation_Scroll(
		aimedPos.x, aimedPos.y, drawSize, drawSize,
		MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f,
		tx, ty,
		boss_aimed_effect.PATTERN_WIDTH,
		boss_aimed_effect.PATTERN_HIGHT,
		BossAimedEffectTextureId,
		true
	);
}

// =========================================================
// Boss振動開始
// =========================================================
void StartBossShake(float power, int frame)
{
	if (power < 0.0f)
	{
		power = -power;
	}

	if (power <= 0.0f || frame <= 0)
	{
		bossShakeOffset = MakeFloat2(0.0f, 0.0f);
		bossShakePower = 0.0f;
		bossShakeFrame = 0;
		return;
	}

	bossShakePower = power;
	bossShakeFrame = frame;
}

// =========================================================
// Boss振動更新
// =========================================================
void UpdateBossShake()
{
	if (bossShakeFrame <= 0)
	{
		bossShakeOffset = MakeFloat2(0.0f, 0.0f);
		bossShakePower = 0.0f;
		bossShakeFrame = 0;
		return;
	}

	const int randomRange = 2001;
	float randomX = static_cast<float>(rand() % randomRange - 1000) / 1000.0f;
	float randomY = static_cast<float>(rand() % randomRange - 1000) / 1000.0f;

	bossShakeOffset = MakeFloat2(
		randomX * bossShakePower,
		randomY * bossShakePower
	);

	bossShakeFrame--;
	if (bossShakeFrame <= 0)
	{
		bossShakeOffset = MakeFloat2(0.0f, 0.0f);
		bossShakePower = 0.0f;
		bossShakeFrame = 0;
	}
}

// =========================================================
// Boss HP Bar更新
// =========================================================
void UpdateBossHpBar()
{
	if (bossDrawHp < boss.hp) bossDrawHp = boss.hp;
	else bossDrawHp = LerpFloat(bossDrawHp, boss.hp, BOSS_HP_BAR_DELAY_LERP);

	if (fabsf(bossDrawHp - boss.hp) < 0.05f) bossDrawHp = boss.hp;

	if (bossDrawHp < 0.0f) bossDrawHp = 0.0f;
	if (bossDrawHp > BOSS_HP) bossDrawHp = BOSS_HP;

	UpdateBossHpBarShake();
}

// =========================================================
// Boss HP Bar振動開始
// =========================================================
void StartBossHpBarShake(float power, int frame)
{
	if (power < 0.0f) power = -power;
	if (power <= 0.0f || frame <= 0)
	{
		bossHpBarShakeOffset = MakeFloat2(0.0f, 0.0f);
		bossHpBarShakePower = 0.0f;
		bossHpBarShakeFrame = 0;
		return;
	}

	bossHpBarShakePower = power;
	bossHpBarShakeFrame = frame;
}

// =========================================================
// Boss HP Bar振動更新
// =========================================================
void UpdateBossHpBarShake()
{
	if (bossHpBarShakeFrame <= 0)
	{
		bossHpBarShakeOffset = MakeFloat2(0.0f, 0.0f);
		bossHpBarShakePower = 0.0f;
		bossHpBarShakeFrame = 0;
		return;
	}

	float randomX = static_cast<float>(rand() % 2001 - 1000) / 1000.0f;
	float randomY = static_cast<float>(rand() % 2001 - 1000) / 1000.0f;

	bossHpBarShakeOffset = MakeFloat2(
		randomX * bossHpBarShakePower,
		randomY * bossHpBarShakePower
	);

	bossHpBarShakeFrame--;
	if (bossHpBarShakeFrame <= 0)
	{
		bossHpBarShakeOffset = MakeFloat2(0.0f, 0.0f);
		bossHpBarShakePower = 0.0f;
		bossHpBarShakeFrame = 0;
	}
}

// =========================================================
// Boss HP Bar描画
// =========================================================
void DrawBossHpBar()
{
	if (!boss.use) return;
	if (!boss.introDone) return;
	float hpRate = boss.hp / BOSS_HP;
	float drawHpRate = bossDrawHp / BOSS_HP;
	if (hpRate < 0.0f) hpRate = 0.0f;
	if (hpRate > 1.0f) hpRate = 1.0f;
	if (drawHpRate < 0.0f) drawHpRate = 0.0f;
	if (drawHpRate > 1.0f) drawHpRate = 1.0f;
	float drawPosX = BOSS_HP_BAR_POS_X + bossHpBarShakeOffset.x;
	float drawPosY = BOSS_HP_BAR_POS_Y + bossHpBarShakeOffset.y;
	// HP Bar背景
	DrawSpriteQuad(
		drawPosX, drawPosY,
		BOSS_HP_BAR_SIZE_X,
		BOSS_HP_BAR_SIZE_Y,
		MakeFloat4(0.055f, 0.065f, 0.085f, 0.94f),
		0
	);
	// 遅延HP Bar
	float delayWidth = BOSS_HP_BAR_SIZE_X * drawHpRate;
	if (delayWidth > 0.0f)
	{
		float delayStartX = drawPosX - BOSS_HP_BAR_SIZE_X / 2.0f;
		float delayCenterX = delayStartX + delayWidth / 2.0f;
		DrawSpriteQuad(
			delayCenterX, drawPosY,
			delayWidth, BOSS_HP_BAR_SIZE_Y,
			MakeFloat4(0.76f, 0.59f, 0.31f, 1.0f),
			0
		);
	}
	// 現在HP Bar
	float hpWidth = BOSS_HP_BAR_SIZE_X * hpRate;
	if (hpWidth > 0.0f)
	{
		float hpStartX = drawPosX - BOSS_HP_BAR_SIZE_X / 2.0f;
		float hpCenterX = hpStartX + hpWidth / 2.0f;
		DrawSpriteQuad(
			hpCenterX, drawPosY,
			hpWidth, BOSS_HP_BAR_SIZE_Y,
			MakeFloat4(0.68f, 0.20f, 0.25f, 1.0f),
			0
		);
	}
	// HP Bar上部のHighlight
	float highlightWidth = BOSS_HP_BAR_SIZE_X * hpRate;
	if (highlightWidth > 0.0f)
	{
		float highlightStartX = drawPosX - BOSS_HP_BAR_SIZE_X / 2.0f;
		float highlightCenterX = highlightStartX + highlightWidth / 2.0f;
		DrawSpriteQuad(
			highlightCenterX,
			drawPosY - BOSS_HP_BAR_SIZE_Y * 0.30f,
			highlightWidth,
			BOSS_HP_BAR_SIZE_Y * 0.18f,
			MakeFloat4(0.88f, 0.42f, 0.45f, 0.52f),
			0
		);
	}
	// Boss Name
	DrawSpriteQuad(
		drawPosX, drawPosY,
		500.0f * 0.6f, 80.0f * 0.6f,
		MakeFloat4(1.0f, 1.0f, 1.0f, 0.8f),
		BossNameTextureId
	);
}

// =========================================================
// Boss死亡開始
// =========================================================
void StartBossDead()
{
	if (isBossDead) return;

	isBossDead = true;
	bossDeadFrame = 0;
	bossDeadExplosionTimer = 0;

	boss.isBeingAimed = false;
	boss.aimedPart = BOSS_AIM_NONE;
	boss.CollisionPosition = MakeFloat2(0.0f, 0.0f);
	boss.CollisionSize = MakeFloat2(0.0f, 0.0f);
	boss.HeadCollisionPosition = MakeFloat2(0.0f, 0.0f);
	boss.HeadCollisionSize = MakeFloat2(0.0f, 0.0f);
	boss.RubyCollisionPosition = MakeFloat2(0.0f, 0.0f);
	boss.RubyCollisionSize = MakeFloat2(0.0f, 0.0f);

	bossShakeOffset = MakeFloat2(0.0f, 0.0f);
	bossShakePower = 0.0f;
	bossShakeFrame = 0;

	// Boss攻撃リセット
	ResetThunder();
	ResetBossBullet();

	// 死亡爆発を初期化
	ResetBossDeadExplosion();

	// Boss本体を振動させる
	StartBossShake(BOSS_DEAD_SHAKE_POWER, BOSS_DEAD_SHAKE_FRAME);

	// 死亡開始時に最初の爆発を生成
	SetBossDeadExplosion();
}

// =========================================================
// Boss死亡更新
// =========================================================
void UpdateBossDead()
{
	UpdateBossShake();
	UpdateBossDeadExplosion();

	bossDeadExplosionTimer++;
	if (bossDeadExplosionTimer >= BOSS_DEAD_EXPLOSION_INTERVAL)
	{
		bossDeadExplosionTimer = 0;
		SetBossDeadExplosion();
	}

	bossDeadFrame++;
	if (bossDeadFrame >= BOSS_DEAD_FRAME)
	{
		boss.use = false;
		isBossDead = false;

		bossShakeOffset = MakeFloat2(0.0f, 0.0f);
		bossShakePower = 0.0f;
		bossShakeFrame = 0;

		ResetBossDeadExplosion();

		bossDeadFrame = 0;
		bossDeadExplosionTimer = 0;

		StartFade(SCENE_RESULT);
		return;
	}
}

// =========================================================
// Boss死亡描画
// =========================================================
void DrawBossDead()
{
	DrawSpriteQuad_Scroll(
		boss.pos.x + bossShakeOffset.x,
		boss.pos.y + bossShakeOffset.y,
		boss.size.x, boss.size.y,
		MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f),
		BossDeadTextureId,
		true
	);

	DrawBossDeadExplosion();
}

// =========================================================
// Boss死亡爆発生成
// =========================================================
void SetBossDeadExplosion()
{
	for (int i = 0; i < BOSS_DEAD_EXPLOSION_MAX; i++)
	{
		if (bossDeadExplosion[i].use) continue;

		float randomX = static_cast<float>(rand() % 2001 - 1000) / 1000.0f;
		float randomY = static_cast<float>(rand() % 2001 - 1000) / 1000.0f;

		float randomSizeRate = BOSS_DEAD_EXPLOSION_SIZE_MIN + static_cast<float>(rand() % 1001) / 1000.0f * (BOSS_DEAD_EXPLOSION_SIZE_MAX - BOSS_DEAD_EXPLOSION_SIZE_MIN);

		bossDeadExplosion[i].pos = MakeFloat2(
				boss.pos.x + randomX * BOSS_DEAD_EXPLOSION_RANGE_X,
				boss.pos.y + randomY * BOSS_DEAD_EXPLOSION_RANGE_Y + 50.0f
			);

		bossDeadExplosion[i].size = MakeFloat2(
			BOSS_DEAD_EXPLOSION_SIZE_X * randomSizeRate,
			BOSS_DEAD_EXPLOSION_SIZE_Y * randomSizeRate
		);

		bossDeadExplosion[i].rotation = Deg2Rad(static_cast<float>(rand() % 360));

		bossDeadExplosion[i].animeFrame = 0;
		bossDeadExplosion[i].use = true;

		switch (int randomExplosionId = rand() % 3)
		{
		case 0:
			PlaySE(SE_Boss_Explosion_1);
			break;
		case 1:
			PlaySE(SE_Boss_Explosion_2);
			break;
		case 2:
		default:
			PlaySE(SE_Boss_Explosion_2);
			break;
		}

		return;
	}
}

// =========================================================
// Boss死亡爆発更新
// =========================================================
void UpdateBossDeadExplosion()
{
	int animeSpeed = boss_dead_explosion.ANIME_SPEED;
	if (animeSpeed <= 0) animeSpeed = 1;

	int animationEndFrame = boss_dead_explosion.PATTERN_MAX * animeSpeed;
	for (int i = 0; i < BOSS_DEAD_EXPLOSION_MAX; i++)
	{
		if (!bossDeadExplosion[i].use) continue;

		bossDeadExplosion[i].animeFrame++;
		if (bossDeadExplosion[i].animeFrame >= animationEndFrame)
		{
			bossDeadExplosion[i] = BOSS_DEAD_EXPLOSION{};
		}
	}
}

// =========================================================
// Boss死亡爆発描画
// =========================================================
void DrawBossDeadExplosion()
{
	if (BossDeadExplosionTextureId == 0) return;
	int animeSpeed = boss_dead_explosion.ANIME_SPEED;
	if (animeSpeed <= 0) animeSpeed = 1;

	int patternMax = boss_dead_explosion.PATTERN_MAX;
	if (patternMax <= 0) patternMax = 1;

	int patternNumU = boss_dead_explosion.PATTERN_NUM_U;
	if (patternNumU <= 0) patternNumU = 1;

	for (int i = 0; i < BOSS_DEAD_EXPLOSION_MAX; i++)
	{
		if (!bossDeadExplosion[i].use) continue;

		int frame = bossDeadExplosion[i].animeFrame / animeSpeed;
		if (frame >= patternMax) frame = patternMax - 1;

		float tx = boss_dead_explosion.PATTERN_WIDTH * static_cast<float>(frame % patternNumU);
		float ty = boss_dead_explosion.PATTERN_HIGHT * static_cast<float>(frame / patternNumU);

		DrawSpriteAnimation_Scroll(
			bossDeadExplosion[i].pos.x, bossDeadExplosion[i].pos.y,
			bossDeadExplosion[i].size.x, bossDeadExplosion[i].size.y,
			MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f),
			bossDeadExplosion[i].rotation,
			tx, ty,
			boss_dead_explosion.PATTERN_WIDTH,
			boss_dead_explosion.PATTERN_HIGHT,
			BossDeadExplosionTextureId,
			true
		);
	}
}

// =========================================================
// Boss死亡爆発リセット
// =========================================================
void ResetBossDeadExplosion()
{
	for (int i = 0; i < BOSS_DEAD_EXPLOSION_MAX; i++)
	{
		bossDeadExplosion[i] =
			BOSS_DEAD_EXPLOSION{};
	}
}

// =========================================================
// Boss取得
// =========================================================
BOSS* GetBoss()
{
	return &boss;
}

// =========================================================
// 雷取得
// =========================================================
BOSS_THUNDER* GetBossThunder()
{
	return &bossThunder[0];
}