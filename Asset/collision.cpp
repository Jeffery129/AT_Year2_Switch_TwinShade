// ===================================================
// collision.cpp 当たり判定
// 
// 制作者：				日付：2026
// ===================================================
#include "main.h"
#include "collision.h"
#include <cmath>
#include <algorithm>

// ===================================================
// バウンディングボックスの当たり判定
// 
// 引数:
// 	矩形Ａの中心座標
// 	矩形Ｂの中心座標
// 	矩形Ａのサイズ
// 	矩形Ｂのサイズ
//
// 戻り値
//  true：当たっている
// 	false：当たっていない
// ===================================================
bool CheckBoxCollider(Float2 PosA, Float2 PosB, Float2 SizeA, Float2 SizeB)
{
	float ATop =	PosA.y - SizeA.y / 2;	// Aの上端
	float ABottom =	PosA.y + SizeA.y / 2;	// Aの下端
	float ARight =	PosA.x + SizeA.x / 2;	// Aの右端
	float ALeft =	PosA.x - SizeA.x / 2;	// Aの左端

	float BTop =	PosB.y - SizeB.y / 2;	// Bの上端
	float BBottom =	PosB.y + SizeB.y / 2;	// Bの下端
	float BRight =	PosB.x + SizeB.x / 2;	// Bの右端
	float BLeft =	PosB.x - SizeB.x / 2;	// Bの左端

	if ((ARight >= BLeft) &&		// Aの右端 > Bの左端
		(ALeft <= BRight) &&		// Aの左端 < Bの右端
		(ABottom >= BTop) &&		// Aの下端 > Bの上端
		(ATop <= BBottom))			// Aの上端 < Bの下端
	{
		// 当たっている
		return true;
	}

	// 当たっていない
	return false;
}

// ===================================================
// バウンディングサークルの当たり判定
// 
// 引数:
// 	円１の中心座標
// 	円２の中心座標
// 	円１の半径
// 	円２の半径
//
// 戻り値
//  true：当たっている
// 	false：当たっていない
// ===================================================
bool CheckCircleCollider(Float2 PosA, Float2 PosB, float rA, float rB)
{
	// (円Aの中心座標X - 円Bの中心座標X)の2乗 + (円Aの中心座標Y - 円Bの中心座標Y)の2乗
	float distance = (PosA.x - PosB.x) * (PosA.x - PosB.x) + (PosA.y - PosB.y) * (PosA.y - PosB.y);
	
	// (円1の半径+円2の半径)の2乗
	float rSum = (rA + rB) * (rA + rB);

	// 2つの円の距離が半径の合計を下回った時
	if (distance <= rSum)
	{
		// 当たっている
		return true;
	}

	// 当たっていない
	return false;
}

// ===================================================
// レーとボックスの当たり判定
// 
// 引数:
// 
//
// 戻り値
//  true：当たっている
// 	false：当たっていない
// ===================================================

bool CheckRaycastBox(Float2 rayOrigin, Float2 rayDir, Float2 boxCenter, Float2 boxSize, float& outDistance)
{
	float dirX = (rayDir.x == 0.0f) ? 0.00001f : rayDir.x;
	float dirY = (rayDir.y == 0.0f) ? 0.00001f : rayDir.y;

	float minX = boxCenter.x - boxSize.x / 2.0f;
	float maxX = boxCenter.x + boxSize.x / 2.0f;
	float minY = boxCenter.y - boxSize.y / 2.0f;
	float maxY = boxCenter.y + boxSize.y / 2.0f;

	float t1 = (minX - rayOrigin.x) / dirX;
	float t2 = (maxX - rayOrigin.x) / dirX;
	float t3 = (minY - rayOrigin.y) / dirY;
	float t4 = (maxY - rayOrigin.y) / dirY;

	float tMin = std::max(std::min(t1, t2), std::min(t3, t4));
	float tMax = std::min(std::max(t1, t2), std::max(t3, t4));

	if (tMax < 0.0f || tMin > tMax)
	{
		return false;
	}

	outDistance = (tMin < 0.0f) ? tMax : tMin;
	return true;
}
