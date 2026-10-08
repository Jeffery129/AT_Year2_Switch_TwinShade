#pragma once


// プロトタイプ宣言
void InitSprite();
void UninitSprite();

//度＝＞ラジアン変換
float Deg2Rad(float deg);

//スクロールオフセット初期化
void InitializeOffset(void);

//カメラ位置設定
void SetCameraPosition(float x, float y);

//------------------------------------------------------------
//スクロールしない描画
//------------------------------------------------------------

//switchの中心点は(0, 0)
void DrawSpriteQuad(
	float x, float y, float w, float h,
	unsigned int texNo);

//色指定付き四角形
void DrawSpriteQuad(
	float x, float y, float w, float h,
	Float4 color, //RGBA
	unsigned int texNo);

//回転付き色指定付き四角形
void DrawSpriteQuad(
	float x, float y, float w, float h,
	Float4 color, float rot,
	unsigned int texNo);

//アニメーションつき描画
void DrawSpriteAnimation(
	float x, float y, float w, float h,
	Float4 color, float rot,
	float tx, float ty, float tw, float th,
	unsigned int texNo);

//UV指定付き四角形
void DrawSpriteQuad_UV(
	float x, float y, float w, float h,
	float tx, float ty, float tw, float th,
	unsigned int texNo);

//------------------------------------------------------------
//スクロールする描画
//------------------------------------------------------------
Float2 GetOffset_Scroll(void);

//switchの中心点は(0, 0)
void DrawSpriteQuad_Scroll(
	float x, float y, float w, float h,
	unsigned int texNo, bool flipX);

//色指定付き四角形
void DrawSpriteQuad_Scroll(
	float x, float y, float w, float h,
	Float4 color, //RGBA
	unsigned int texNo, bool flipX);

//回転付き色指定付き四角形
void DrawSpriteQuad_Scroll(
	float x, float y, float w, float h,
	Float4 color, float rot,
	unsigned int texNo, bool flipX);

//アニメーションつき描画
void DrawSpriteAnimation_Scroll(
	float x, float y, float w, float h,
	Float4 color, float rot,
	float tx, float ty, float tw, float th,
	unsigned int texNo, bool flipX);

// UV指定付きスクロール四角形
void DrawSpriteQuad_UV_Scroll(
	float x, float y, float w, float h,
	float tx, float ty, float tw, float th,
	Float4 color,
	unsigned int texNo
);