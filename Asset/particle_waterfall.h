// =========================================================
// particle_waterfall.h
// Particle管理
// =========================================================
#ifndef _PARTICLE_WATERFALL_H_
#define _PARTICLE_WATERFALL_H_

// =========================================================
// Particle画像
// =========================================================
enum PARTICLE_WATERFALL_PIC
{
	PARTICLE_WATERFALL_RED = 0,
	PARTICLE_WATERFALL_BLUE,
	PARTICLE_WATERFALL_PIC_MAX
};

// =========================================================
// プロトタイプ宣言
// =========================================================
void InitializeParticleWaterfall(void);
void UpdateParticleWaterfall(void);
void DrawParticleWaterfall(
	float startPosX, float startPosY,
	int posXRange, float directionRot,
	int speedRange, float startSpeedY,
	float startScale, int numPerShoot,
	int framePerShoot, int picNum
);
void ResetParticleWaterfall(void);
void FinalizeParticleWaterfall(void);

#endif
