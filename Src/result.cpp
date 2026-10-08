// =========================================================
// result.cpp リザルトシーン制御
//
// 制作者:        日付：
// =========================================================
#include "main.h"
#include "result.h"
#include "controller.h"
#include "texture.h"
#include "sprite.h"
#include "fade.h"
#include "sound.h"

// =========================================================
// マクロ定義
// =========================================================
#define RESULT_PATTERN_SIZE                 (512.0f)
#define RESULT_BLUE_SCROLL_SPEED            (0.001f)
#define RESULT_RED_SCROLL_SPEED             (0.0015f)

// =========================================================
// グローバル変数
// =========================================================
unsigned int resultTextureId{};
unsigned int resultPatternTextureId{};
float resultBlueU{};
float resultBlueV{};
float resultRedU{};
float resultRedV{};

// =========================================================
// プロトタイプ宣言
// =========================================================
void WrapResultPatternUV(float* u, float* v);

// =========================================================
// リザルトシーン初期化
// =========================================================
void InitializeResult(void)
{
    resultTextureId = LoadTexture("rom:/Result_Text.tga");
    resultPatternTextureId = LoadTexture("rom:/Result_Noise.tga");
    resultBlueU = 0.0f;
    resultBlueV = 0.0f;
    resultRedU = 0.5f;
    resultRedV = 0.5f;
    StopBGM();
    PlaySE(SE_Game_Clear);
}

// =========================================================
// リザルトシーン更新
// =========================================================
void UpdateResult(void)
{
    // 淡い青は右上へスクロール
    resultBlueU -= RESULT_BLUE_SCROLL_SPEED;
    resultBlueV += RESULT_BLUE_SCROLL_SPEED;
    // 淡い赤は左上へスクロール
    resultRedU += RESULT_RED_SCROLL_SPEED;
    resultRedV += RESULT_RED_SCROLL_SPEED;
    WrapResultPatternUV(&resultBlueU, &resultBlueV);
    WrapResultPatternUV(&resultRedU, &resultRedV);

    // いずれかのボタンでSelectへ戻る
    if (GetControllerTrigger(NpadButton::Plus::Index) ||
        GetControllerTrigger(NpadButton::Minus::Index) ||
        GetControllerTrigger(NpadButton::A::Index) ||
        GetControllerTrigger(NpadButton::B::Index) ||
        GetControllerTrigger(NpadButton::X::Index) ||
        GetControllerTrigger(NpadButton::Y::Index))
    {
        if (GetFade()->state == FADE_NONE)
        {
            PlaySE(UI_Title_Start);
            StartFade(SCENE_SELECT);
        }
    }
}
// =========================================================
// リザルトシーン描画
// =========================================================
void DrawResult(void)
{
    // 淡い白背景
    DrawSpriteQuad(
        0.0f, 0.0f,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        MakeFloat4(0.94f, 0.93f, 0.92f, 1.0f),
        0
    );

    float repeatU = SCREEN_WIDTH / (RESULT_PATTERN_SIZE / 2.0f);
    float repeatV = SCREEN_HEIGHT / (RESULT_PATTERN_SIZE / 2.0f);
    // 右上へ流れる淡い青Pattern
    DrawSpriteAnimation(
        0.0f, 0.0f,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        MakeFloat4(0.62f, 0.82f, 1.00f, 1.0f), 0.0f,
        resultBlueU, resultBlueV,
        repeatU, repeatV,
        resultPatternTextureId
    );

   repeatU = SCREEN_WIDTH / RESULT_PATTERN_SIZE;
   repeatV = SCREEN_HEIGHT / RESULT_PATTERN_SIZE;
    // 左上へ流れる淡い赤Pattern
    DrawSpriteAnimation(
        0.0f, 0.0f,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        MakeFloat4(1.00f, 0.68f, 0.76f, 1.0f), 0.0f,
        resultRedU, resultRedV,
        repeatU, repeatV,
        resultPatternTextureId
    );

    // Result画像
    DrawSpriteQuad(
        0.0f, 0.0f,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f,
        resultTextureId
    );
}
// =========================================================
// リザルトシーン終了処理
// =========================================================
void FinalizeResult(void)
{
    if (resultTextureId != 0)
    {
        UnloadTexture(resultTextureId);
        resultTextureId = 0;
    }
    if (resultPatternTextureId != 0)
    {
        UnloadTexture(resultPatternTextureId);
        resultPatternTextureId = 0;
    }
    resultBlueU = 0.0f;
    resultBlueV = 0.0f;
    resultRedU = 0.0f;
    resultRedV = 0.0f;
}

// =========================================================
// Result Pattern UV補正
// =========================================================
void WrapResultPatternUV(float* u, float* v)
{
    if (u == nullptr || v == nullptr) return;
    if (*u >= 1.0f) *u -= 1.0f;
    else if (*u < 0.0f) *u += 1.0f;
    if (*v >= 1.0f) *v -= 1.0f;
    else if (*v < 0.0f) *v += 1.0f;
}
