#include "main.h"
#include "texture.h"
#include "sprite.h"

unsigned int	g_SpriteVertexArrayObject;
unsigned int	g_SpriteVertexBuffer;
Float2 ScreenOffset;

//度＝＞ラジアン変換
float Deg2Rad(float deg)
{
	return deg * (3.1415926f / 180.0f);//度 * (π/180)
}

void InitSprite()
{
	// カリングをON
	glEnable(GL_CULL_FACE);

	glGenVertexArrays(1, &g_SpriteVertexArrayObject);
	glBindVertexArray(g_SpriteVertexArrayObject);

	glEnableVertexArrayAttrib(g_SpriteVertexArrayObject, 0);
	glEnableVertexArrayAttrib(g_SpriteVertexArrayObject, 1);
	glEnableVertexArrayAttrib(g_SpriteVertexArrayObject, 2);

	glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribFormat(1, 4, GL_FLOAT, GL_FALSE, sizeof(Float3));
	glVertexAttribFormat(2, 2, GL_FLOAT, GL_FALSE, sizeof(Float3) + sizeof(Float4));

	glVertexAttribBinding(0, 0);
	glVertexAttribBinding(1, 0);
	glVertexAttribBinding(2, 0);


	glGenBuffers(1, &g_SpriteVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, g_SpriteVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VERTEX_3D) * 4, 0, GL_DYNAMIC_DRAW);

	glBindVertexArray(0);

	InitializeOffset();
}

void UninitSprite()
{
	glDeleteVertexArrays(1, &g_SpriteVertexArrayObject);
	glDeleteBuffers(1, &g_SpriteVertexBuffer);
}

//オフセットの初期化
void InitializeOffset(void)
{
	ScreenOffset = MakeFloat2(0.0f, 0.0f);
}

//カメラ位置設定（新スクロール関数）
void SetCameraPosition(float x, float y)
{
	ScreenOffset.x = -x;
	ScreenOffset.y = -y;
}

//------------------------------------------------------------
//スクロールしない描画
//------------------------------------------------------------

//四角形の描画  
void DrawSpriteQuad(
	float x, float y, float w, float h,
	unsigned int texNo)
{
	VERTEX_3D vertexQuad[4];

	vertexQuad[0].Position = MakeFloat3(x + w * 0.5f, y - h * 0.5f, 0.0f);
	vertexQuad[1].Position = MakeFloat3(x - w * 0.5f, y - h * 0.5f, 0.0f);
	vertexQuad[2].Position = MakeFloat3(x + w * 0.5f, y + h * 0.5f, 0.0f);
	vertexQuad[3].Position = MakeFloat3(x - w * 0.5f, y + h * 0.5f, 0.0f);

	vertexQuad[0].Color = MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f);
	vertexQuad[1].Color = MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f);
	vertexQuad[2].Color = MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f);
	vertexQuad[3].Color = MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f);

	vertexQuad[0].TexCoord = MakeFloat2(1.0f, 0.0f);
	vertexQuad[1].TexCoord = MakeFloat2(0.0f, 0.0f);
	vertexQuad[2].TexCoord = MakeFloat2(1.0f, 1.0f);
	vertexQuad[3].TexCoord = MakeFloat2(0.0f, 1.0f);

	glBindBuffer(GL_ARRAY_BUFFER, g_SpriteVertexBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VERTEX_3D) * 4, vertexQuad);


	glBindVertexArray(g_SpriteVertexArrayObject);
	glBindVertexBuffer(0, g_SpriteVertexBuffer, 0, sizeof(VERTEX_3D));

	SetTexture(texNo);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
}

//色指定付き四角形
void DrawSpriteQuad(
	float x, float y, float w, float h,
	Float4 color, //RGBA
	unsigned int texNo)
{
	VERTEX_3D vertexQuad[4];

	vertexQuad[0].Position = MakeFloat3(x + w * 0.5f, y - h * 0.5f, 0.0f);
	vertexQuad[1].Position = MakeFloat3(x - w * 0.5f, y - h * 0.5f, 0.0f);
	vertexQuad[2].Position = MakeFloat3(x + w * 0.5f, y + h * 0.5f, 0.0f);
	vertexQuad[3].Position = MakeFloat3(x - w * 0.5f, y + h * 0.5f, 0.0f);

	vertexQuad[0].Color = color;
	vertexQuad[1].Color = color;
	vertexQuad[2].Color = color;
	vertexQuad[3].Color = color;

	vertexQuad[0].TexCoord = MakeFloat2(1.0f, 0.0f);
	vertexQuad[1].TexCoord = MakeFloat2(0.0f, 0.0f);
	vertexQuad[2].TexCoord = MakeFloat2(1.0f, 1.0f);
	vertexQuad[3].TexCoord = MakeFloat2(0.0f, 1.0f);

	glBindBuffer(GL_ARRAY_BUFFER, g_SpriteVertexBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VERTEX_3D) * 4, vertexQuad);


	glBindVertexArray(g_SpriteVertexArrayObject);
	glBindVertexBuffer(0, g_SpriteVertexBuffer, 0, sizeof(VERTEX_3D));

	SetTexture(texNo);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
}

//回転付き色指定付き四角形
void DrawSpriteQuad(
	float x, float y, float w, float h,
	Float4 color, float rot,
	unsigned int texNo)
{
	VERTEX_3D vertexQuad[4];

	vertexQuad[0].Position = MakeFloat3(+w * 0.5f, -h * 0.5f, 0.0f);
	vertexQuad[1].Position = MakeFloat3(-w * 0.5f, -h * 0.5f, 0.0f);
	vertexQuad[2].Position = MakeFloat3(+w * 0.5f, +h * 0.5f, 0.0f);
	vertexQuad[3].Position = MakeFloat3(-w * 0.5f, +h * 0.5f, 0.0f);

	//回転処理
	for (int i = 0; i < 4; i++)
	{
		Float2 size;
		size.x = vertexQuad[i].Position.x;
		size.y = vertexQuad[i].Position.y;

		vertexQuad[i].Position.x = (size.x * cosf(rot) - size.y * sinf(rot)) + x;
		vertexQuad[i].Position.y = (size.x * sinf(rot) + size.y * cosf(rot)) + y;
	}

	vertexQuad[0].Color = color;
	vertexQuad[1].Color = color;
	vertexQuad[2].Color = color;
	vertexQuad[3].Color = color;

	vertexQuad[0].TexCoord = MakeFloat2(1.0f, 0.0f);
	vertexQuad[1].TexCoord = MakeFloat2(0.0f, 0.0f);
	vertexQuad[2].TexCoord = MakeFloat2(1.0f, 1.0f);
	vertexQuad[3].TexCoord = MakeFloat2(0.0f, 1.0f);

	glBindBuffer(GL_ARRAY_BUFFER, g_SpriteVertexBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VERTEX_3D) * 4, vertexQuad);


	glBindVertexArray(g_SpriteVertexArrayObject);
	glBindVertexBuffer(0, g_SpriteVertexBuffer, 0, sizeof(VERTEX_3D));

	SetTexture(texNo);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
}

//アニメーションつき描画
void DrawSpriteAnimation(
	float x, float y, float w, float h,
	Float4 color, float rot,
	float tx, float ty, float tw, float th,
	unsigned int texNo)
{
	VERTEX_3D vertexQuad[4];

	vertexQuad[0].Position = MakeFloat3(+w * 0.5f, -h * 0.5f, 0.0f);
	vertexQuad[1].Position = MakeFloat3(-w * 0.5f, -h * 0.5f, 0.0f);
	vertexQuad[2].Position = MakeFloat3(+w * 0.5f, +h * 0.5f, 0.0f);
	vertexQuad[3].Position = MakeFloat3(-w * 0.5f, +h * 0.5f, 0.0f);

	//回転処理
	for (int i = 0; i < 4; i++)
	{
		Float2 size;
		size.x = vertexQuad[i].Position.x;
		size.y = vertexQuad[i].Position.y;

		vertexQuad[i].Position.x = (size.x * cosf(rot) - size.y * sinf(rot)) + x;
		vertexQuad[i].Position.y = (size.x * sinf(rot) + size.y * cosf(rot)) + y;
	}

	for (int i = 0; i < 4; i++)
	{
		vertexQuad[i].Color = color;
	}

	vertexQuad[0].TexCoord = MakeFloat2(tx + tw, ty     );
	vertexQuad[1].TexCoord = MakeFloat2(tx	   , ty     );
	vertexQuad[2].TexCoord = MakeFloat2(tx + tw, ty + th);
	vertexQuad[3].TexCoord = MakeFloat2(tx     , ty + th);

	glBindBuffer(GL_ARRAY_BUFFER, g_SpriteVertexBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VERTEX_3D) * 4, vertexQuad);


	glBindVertexArray(g_SpriteVertexArrayObject);
	glBindVertexBuffer(0, g_SpriteVertexBuffer, 0, sizeof(VERTEX_3D));

	SetTexture(texNo);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
}

//UV指定付き四角形
void DrawSpriteQuad_UV(
	float x, float y, float w, float h,
	float tx, float ty, float tw, float th,
	unsigned int texNo)
{
	VERTEX_3D vertexQuad[4];

	vertexQuad[0].Position = MakeFloat3(x + w * 0.5f, y - h * 0.5f, 0.0f);
	vertexQuad[1].Position = MakeFloat3(x - w * 0.5f, y - h * 0.5f, 0.0f);
	vertexQuad[2].Position = MakeFloat3(x + w * 0.5f, y + h * 0.5f, 0.0f);
	vertexQuad[3].Position = MakeFloat3(x - w * 0.5f, y + h * 0.5f, 0.0f);

	vertexQuad[0].Color = MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f);
	vertexQuad[1].Color = MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f);
	vertexQuad[2].Color = MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f);
	vertexQuad[3].Color = MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f);

	vertexQuad[0].TexCoord = MakeFloat2(tx + tw, ty);
	vertexQuad[1].TexCoord = MakeFloat2(tx, ty);
	vertexQuad[2].TexCoord = MakeFloat2(tx + tw, ty + th);
	vertexQuad[3].TexCoord = MakeFloat2(tx, ty + th);

	glBindBuffer(GL_ARRAY_BUFFER, g_SpriteVertexBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VERTEX_3D) * 4, vertexQuad);

	glBindVertexArray(g_SpriteVertexArrayObject);
	glBindVertexBuffer(0, g_SpriteVertexBuffer, 0, sizeof(VERTEX_3D));

	SetTexture(texNo);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
}

//------------------------------------------------------------
//スクロールする描画
//------------------------------------------------------------

Float2 GetOffset_Scroll(void)
{
	return ScreenOffset;
}

//四角形の描画
void DrawSpriteQuad_Scroll(
	float x, float y, float w, float h, 
	unsigned int texNo, bool flipX)
{
	VERTEX_3D vertexQuad[4];

	//スクロール処理
	x += ScreenOffset.x;
	y += ScreenOffset.y;

	vertexQuad[0].Position = MakeFloat3(x + w * 0.5f, y - h * 0.5f, 0.0f);
	vertexQuad[1].Position = MakeFloat3(x - w * 0.5f, y - h * 0.5f, 0.0f);
	vertexQuad[2].Position = MakeFloat3(x + w * 0.5f, y + h * 0.5f, 0.0f);
	vertexQuad[3].Position = MakeFloat3(x - w * 0.5f, y + h * 0.5f, 0.0f);

	vertexQuad[0].Color = MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f);
	vertexQuad[1].Color = MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f);
	vertexQuad[2].Color = MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f);
	vertexQuad[3].Color = MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f);

	float uLeft  = flipX ? 0.0f : 1.0f;
	float uRight = flipX ? 1.0f : 0.0f;

	vertexQuad[0].TexCoord = MakeFloat2(uRight, 0.0f);
	vertexQuad[1].TexCoord = MakeFloat2(uLeft , 0.0f);
	vertexQuad[2].TexCoord = MakeFloat2(uRight, 1.0f);
	vertexQuad[3].TexCoord = MakeFloat2(uLeft , 1.0f);

	glBindBuffer(GL_ARRAY_BUFFER, g_SpriteVertexBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VERTEX_3D) * 4, vertexQuad);

	glBindVertexArray(g_SpriteVertexArrayObject);
	glBindVertexBuffer(0, g_SpriteVertexBuffer, 0, sizeof(VERTEX_3D));

	SetTexture(texNo);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
}

//色指定付き四角形
void DrawSpriteQuad_Scroll(
	float x, float y, float w, float h, 
	Float4 color, 
	unsigned int texNo,
	bool flipX)
{
	VERTEX_3D vertexQuad[4];

	//スクロール処理
	x += ScreenOffset.x;
	y += ScreenOffset.y;

	vertexQuad[0].Position = MakeFloat3(x + w * 0.5f, y - h * 0.5f, 0.0f);
	vertexQuad[1].Position = MakeFloat3(x - w * 0.5f, y - h * 0.5f, 0.0f);
	vertexQuad[2].Position = MakeFloat3(x + w * 0.5f, y + h * 0.5f, 0.0f);
	vertexQuad[3].Position = MakeFloat3(x - w * 0.5f, y + h * 0.5f, 0.0f);

	for (int i = 0; i < 4; i++) 
	{
		vertexQuad[i].Color = color;
	}

	float uLeft  = flipX ? 0.0f : 1.0f;
	float uRight = flipX ? 1.0f : 0.0f;

	vertexQuad[0].TexCoord = MakeFloat2(uRight, 0.0f);
	vertexQuad[1].TexCoord = MakeFloat2(uLeft , 0.0f);
	vertexQuad[2].TexCoord = MakeFloat2(uRight, 1.0f);
	vertexQuad[3].TexCoord = MakeFloat2(uLeft , 1.0f);

	glBindBuffer(GL_ARRAY_BUFFER, g_SpriteVertexBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VERTEX_3D) * 4, vertexQuad);


	glBindVertexArray(g_SpriteVertexArrayObject);
	glBindVertexBuffer(0, g_SpriteVertexBuffer, 0, sizeof(VERTEX_3D));

	SetTexture(texNo);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
}

//回転付き色指定付き四角形
void DrawSpriteQuad_Scroll(
	float x, float y, float w, float h, 
	Float4 color, float rot, 
	unsigned int texNo,
	bool flipX)
{
	VERTEX_3D vertexQuad[4];

	//スクロール処理
	x += ScreenOffset.x;
	y += ScreenOffset.y;

	vertexQuad[0].Position = MakeFloat3(+w * 0.5f, -h * 0.5f, 0.0f);
	vertexQuad[1].Position = MakeFloat3(-w * 0.5f, -h * 0.5f, 0.0f);
	vertexQuad[2].Position = MakeFloat3(+w * 0.5f, +h * 0.5f, 0.0f);
	vertexQuad[3].Position = MakeFloat3(-w * 0.5f, +h * 0.5f, 0.0f);

	//回転/色処理
	for (int i = 0; i < 4; i++)
	{
		Float2 size;
		size.x = vertexQuad[i].Position.x;
		size.y = vertexQuad[i].Position.y;

		vertexQuad[i].Position.x = (size.x * cosf(rot) - size.y * sinf(rot)) + x;
		vertexQuad[i].Position.y = (size.x * sinf(rot) + size.y * cosf(rot)) + y;

		vertexQuad[i].Color = color;
	}

	float uLeft  = flipX ? 0.0f : 1.0f;
	float uRight = flipX ? 1.0f : 0.0f;

	vertexQuad[0].TexCoord = MakeFloat2(uRight, 0.0f);
	vertexQuad[1].TexCoord = MakeFloat2(uLeft , 0.0f);
	vertexQuad[2].TexCoord = MakeFloat2(uRight, 1.0f);
	vertexQuad[3].TexCoord = MakeFloat2(uLeft , 1.0f);

	glBindBuffer(GL_ARRAY_BUFFER, g_SpriteVertexBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VERTEX_3D) * 4, vertexQuad);


	glBindVertexArray(g_SpriteVertexArrayObject);
	glBindVertexBuffer(0, g_SpriteVertexBuffer, 0, sizeof(VERTEX_3D));

	SetTexture(texNo);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
}

//アニメーションつき描画
void DrawSpriteAnimation_Scroll(
	float x, float y, float w, float h,
	Float4 color, float rot, 
	float tx, float ty, float tw, float th, 
	unsigned int texNo,
	bool flipX)
{
	VERTEX_3D vertexQuad[4];

	//スクロール処理
	x += ScreenOffset.x;
	y += ScreenOffset.y;

	vertexQuad[0].Position = MakeFloat3(+w * 0.5f, -h * 0.5f, 0.0f);
	vertexQuad[1].Position = MakeFloat3(-w * 0.5f, -h * 0.5f, 0.0f);
	vertexQuad[2].Position = MakeFloat3(+w * 0.5f, +h * 0.5f, 0.0f);
	vertexQuad[3].Position = MakeFloat3(-w * 0.5f, +h * 0.5f, 0.0f);

	//回転/色処理
	for (int i = 0; i < 4; i++)
	{
		Float2 size;
		size.x = vertexQuad[i].Position.x;
		size.y = vertexQuad[i].Position.y;

		vertexQuad[i].Position.x = (size.x * cosf(rot) - size.y * sinf(rot)) + x;
		vertexQuad[i].Position.y = (size.x * sinf(rot) + size.y * cosf(rot)) + y;

		vertexQuad[i].Color = color;
	}

	float uLeft  = flipX ? tx      : tx + tw;
	float uRight = flipX ? tx + tw : tx     ;

	vertexQuad[0].TexCoord = MakeFloat2(uRight, ty     );
	vertexQuad[1].TexCoord = MakeFloat2(uLeft , ty     );
	vertexQuad[2].TexCoord = MakeFloat2(uRight, ty + th);
	vertexQuad[3].TexCoord = MakeFloat2(uLeft , ty + th);

	glBindBuffer(GL_ARRAY_BUFFER, g_SpriteVertexBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VERTEX_3D) * 4, vertexQuad);

	glBindVertexArray(g_SpriteVertexArrayObject);
	glBindVertexBuffer(0, g_SpriteVertexBuffer, 0, sizeof(VERTEX_3D));

	SetTexture(texNo);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
}

// UV指定付きスクロール四角形
void DrawSpriteQuad_UV_Scroll(
	float x, float y, float w, float h,
	float tx, float ty, float tw, float th,
	Float4 color,
	unsigned int texNo)
{
	VERTEX_3D vertexQuad[4];

	// スクロール処理
	x += ScreenOffset.x;
	y += ScreenOffset.y;

	vertexQuad[0].Position = MakeFloat3(x + w * 0.5f, y - h * 0.5f, 0.0f);
	vertexQuad[1].Position = MakeFloat3(x - w * 0.5f, y - h * 0.5f, 0.0f);
	vertexQuad[2].Position = MakeFloat3(x + w * 0.5f, y + h * 0.5f, 0.0f);
	vertexQuad[3].Position = MakeFloat3(x - w * 0.5f, y + h * 0.5f, 0.0f);

	for (int i = 0; i < 4; i++)
	{
		vertexQuad[i].Color = color;
	}

	vertexQuad[0].TexCoord = MakeFloat2(tx + tw, ty);
	vertexQuad[1].TexCoord = MakeFloat2(tx, ty);
	vertexQuad[2].TexCoord = MakeFloat2(tx + tw, ty + th);
	vertexQuad[3].TexCoord = MakeFloat2(tx, ty + th);

	glBindBuffer(GL_ARRAY_BUFFER, g_SpriteVertexBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VERTEX_3D) * 4, vertexQuad);
	glBindVertexArray(g_SpriteVertexArrayObject);
	glBindVertexBuffer(0, g_SpriteVertexBuffer, 0, sizeof(VERTEX_3D));

	SetTexture(texNo);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}