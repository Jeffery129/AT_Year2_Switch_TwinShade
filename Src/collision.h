// ===================================================
// collision.h 当たり判定
// 
// 制作者：				日付：2026
// ===================================================
#ifndef _COLLISION_H_
#define _COLLISION_H_

// ===================================================
// プロトタイプ宣言
// ===================================================
bool CheckBoxCollider(Float2 PosA, Float2 PosB, Float2 SizeA, Float2 SizeB);	// バウンディングボックスの当たり判定

bool CheckCircleCollider(Float2 PosA, Float2 PosB, float rA, float rB);		// バウンディングサークルの当たり判定

bool CheckRaycastBox(Float2 rayOrigin, Float2 rayDir, Float2 boxCenter, Float2 boxSize, float& outDistance);

#endif