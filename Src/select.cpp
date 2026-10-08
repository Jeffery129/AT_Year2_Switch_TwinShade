// =========================================================
// select.cpp タイトルからのステージ選択画面
//
// 制作者:        日付：
// =========================================================
#include "select.h"
#include "sound.h"
#include "controller.h"
#include "texture.h"
#include "sprite.h"
#include "fade.h"
#include "save_data.h"

//--------------------------------------------------------------
// マクロ定義 (*´▽｀*)
//--------------------------------------------------------------
#define PANEL_POS_X                         (320.0f)
#define PANEL_POS_Y                         (-20.0f)
#define PANEL_SIZE_W                        (1088.0f)
#define PANEL_SIZE_H                        (784.0f)
#define PANEL_ARROW_POS_X_LEFT              (PANEL_POS_X - PANEL_SIZE_W / 2 + 70.0f) // Local X Left
#define PANEL_ARROW_POS_X_RIGHT             (PANEL_POS_X + PANEL_SIZE_W / 2 - 70.0f) // Local X Right
#define PANEL_ARROW_SIZE_W                  (76.0f)
#define PANEL_ARROW_SIZE_H                  (100.0f)
#define PANEL_STAGE_ICON_POS_X              (PANEL_POS_X)
#define PANEL_STAGE_ICON_POS_Y              (PANEL_POS_Y)
#define PANEL_STAGE_ICON_SCALE              (2.0f)
#define PANEL_STAGE_ICON_SIZE               (313.0f * PANEL_STAGE_ICON_SCALE)

#define TAB_SIZE_W                          (448.0f)
#define TAB_SIZE_H                          (192.0f)
#define TAB_SIZE_SCALE                      (0.9f)
#define TAB_HOVERED_SCALE                   (1.1f)
#define TAB_HOVERED_DEGREE                  (2.0f)
#define TAB_POS_X                           (-570.0f)
#define TAB_STAGE_POS_Y                     (-280.0f)
#define TAB_Y_DISTANCE                      (TAB_SIZE_H / 2 + 160.0f)

// Setting PanelはStage Panelと同じ中心座標へ表示する
#define SETTING_PANEL_POS_X                 (PANEL_POS_X + 10.0f)
#define SETTING_PANEL_POS_Y                 (PANEL_POS_Y - 15.0f)
#define SETTING_SIZE_SCALE                  (1.7f)
#define SETTING_PANEL_SIZE_W                (640.0f * SETTING_SIZE_SCALE)
#define SETTING_PANEL_SIZE_H                (540.0f * SETTING_SIZE_SCALE)
#define SETTING_INTRO_POS_X                 (0.0f)
#define SETTING_INTRO_POS_Y                 (0.0f)
#define SETTING_INTRO_SIZE_W                (1920.0f)
#define SETTING_INTRO_SIZE_H                (1080.0f)

// Game内Optionと同じ調整範囲
#define SETTING_VOLUME_MIN                  (0.0f)
#define SETTING_VOLUME_MAX                  (1.0f)
#define SETTING_VOLUME_STEP                 (0.1f)
#define SETTING_SENS_MIN                    (0.05f)
#define SETTING_SENS_MAX                    (1.0f)
#define SETTING_SENS_STEP                   (0.05f)

// Game内OptionのBar座標をSetting Panel中心へ加算する
#define SETTING_BAR_LOCAL_POS_X             (158.0f)
#define SETTING_SOUND_BAR_LOCAL_POS_Y       (35.0f)
#define SETTING_SENS_BAR_LOCAL_POS_Y        (187.0f)
#define SETTING_BAR_POS_X                   (SETTING_PANEL_POS_X + SETTING_BAR_LOCAL_POS_X)
#define SETTING_SOUND_BAR_POS_Y             (SETTING_PANEL_POS_Y + SETTING_SOUND_BAR_LOCAL_POS_Y)
#define SETTING_SENS_BAR_POS_Y              (SETTING_PANEL_POS_Y + SETTING_SENS_BAR_LOCAL_POS_Y)
#define SETTING_BAR_SIZE_W                  (460.0f)
#define SETTING_BAR_SIZE_H                  (50.0f)

// Select Noise Background
#define SELECT_NOISE_PIC_NUM				(2)
#define SELECT_NOISE_SIZE					(256.0f)
#define SELECT_NOISE_01_SCROLL_SPEED		(0.0006f)
#define SELECT_NOISE_02_SCROLL_SPEED		(0.0003f)

// Tab上下浮動
#define TAB_FLOAT_HEIGHT                    (6.0f)
#define TAB_FLOAT_SPEED                     (0.03f)
#define TAB_FLOAT_PHASE_OFFSET              (0.5f)

// Tab影
#define TAB_SHADOW_OFFSET_X                 (8.0f)
#define TAB_SHADOW_OFFSET_Y                 (10.0f)
#define TAB_SHADOW_ALPHA                    (0.3f)

//--------------------------------------------------------------
// 列挙型定義
//--------------------------------------------------------------
// Stage選択時の左右Arrow
// LEFT・RIGHTは他の定義と衝突しやすいため専用名を使う
enum STAGE_PANEL_ARROW
{
    PANEL_ARROW_LEFT = 0,
    PANEL_ARROW_RIGHT,
    PANEL_ARROW_MAX
};

// Stage Panel画像
enum STAGE_PANEL_STATE
{
    STAGE_UNLOCK = 0,
    STAGE_LOCKED,
    PANEL_STATE_MAX
};

// Tab画像状態
enum SELECT_TAB_STATE
{
    NONE = 0,
    HOVERED,
    TAB_STATE_MAX
};

// Tab種類
enum SELECT_TAB_PIC
{
    TAB_STAGE = 0,
    TAB_OPTION,
    TAB_BACK,
    TAB_MAX
};

// Select操作状態
enum SELECT_MENU_STATE
{
    SELECT_MENU_TAB = 0,
    SELECT_MENU_STAGE,
    SELECT_MENU_OPTION,
    SELECT_MENU_STATE_MAX
};

// Setting画像
enum SELECT_SETTING_PIC
{
    SELECT_SETTING_PIC_FRAME = 0,
    SELECT_SETTING_PIC_SOUND,
    SELECT_SETTING_PIC_SENSITIVITY,
    SELECT_SETTING_PIC_MAX
};

// Setting項目
enum SELECT_SETTING_BUTTON
{
    SELECT_SETTING_BUTTON_SOUND = 0,
    SELECT_SETTING_BUTTON_SENSITIVITY,
    SELECT_SETTING_BUTTON_MAX
};

//--------------------------------------------------------------
// グローバル変数
//--------------------------------------------------------------
// Select Noise Background
const char* select_noise_file_name[SELECT_NOISE_PIC_NUM]
{
    "rom:/Select_Pattern_Low_Resolusion_01.tga",
    "rom:/Select_Pattern_Low_Resolusion_02.tga"
};
unsigned int select_noise_textureID[SELECT_NOISE_PIC_NUM]{};

float selectNoise01U{}; // Pattern 01 UV
float selectNoise01V{};

float selectNoise02U{}; // Pattern 02 UV
float selectNoise02V{};

Float4 selectNoise01Color{}; // Pattern Color
Float4 selectNoise02Color{};

// Select Menu全体のIntro
const char* select_menu_intro = "rom:/Select_Intro.tga";
unsigned int select_menu_textureID{};

// Stage選択時の左右Arrow
const char* select_panel_arrow_file_name[PANEL_ARROW_MAX]
{
    "rom:/Select_Arrow_Left.tga",
    "rom:/Select_Arrow_Right.tga"
};
unsigned int select_arrow_textureID[PANEL_ARROW_MAX]{};

// Stage選択Panel
const char* select_stage_panel_file_name[PANEL_STATE_MAX]
{
    "rom:/Select_Stage_Panel_Unlock.tga",
    "rom:/Select_Stage_Panel_Locked.tga"
};
unsigned int select_stage_panel_textureID[PANEL_STATE_MAX]{};

// Stage Icon
const char* select_stage_icon_file_name[GAME_STAGE_MAX]
{
    "rom:/Select_Icon_Stage_T_01.tga",
    "rom:/Select_Icon_Stage_T_02.tga",
    "rom:/Select_Icon_Stage_T_03.tga",
    "rom:/Select_Icon_Stage_S_01.tga",
    "rom:/Select_Icon_Stage_S_02.tga",
    "rom:/Select_Icon_Stage_Boss.tga"
};
unsigned int select_stage_icon_textureID[GAME_STAGE_MAX]{};

// Tab
const char* select_tab_file_name[TAB_MAX][TAB_STATE_MAX]
{
    {
        "rom:/Select_Tab_Stage_None.tga",
        "rom:/Select_Tab_Stage_Hovered.tga"
    },
    {
        "rom:/Select_Tab_Option_None.tga",
        "rom:/Select_Tab_Option_Hovered.tga"
    },
    {
        "rom:/Select_Tab_Back_None.tga",
        "rom:/Select_Tab_Back_Hovered.tga"
    }
};
unsigned int select_tab_textureID[TAB_MAX][TAB_STATE_MAX]{};

// Select内Setting
const char* select_setting_file_name[SELECT_SETTING_PIC_MAX]
{
    "rom:/Setting_Menu_Frame.tga",
    "rom:/Setting_Hover_Sound.tga",
    "rom:/Setting_Hover_Sensitivity.tga"
};
unsigned int select_setting_textureID[SELECT_SETTING_PIC_MAX]{};

// Menu状態
SELECT_MENU_STATE selectMenuState{ SELECT_MENU_TAB };
SELECT_TAB_PIC selectHoverTab{ TAB_STAGE };
GAME_STAGE selectStage{ GAME_STAGE_T_01 };

// Setting状態
int selectSettingHover{ SELECT_SETTING_BUTTON_SOUND };
float selectMasterVolume{ 1.0f };
float selectSensitivity{ 0.3f };

// Tab上下浮動Animation
unsigned int selectTabAnimeFrame{};

//--------------------------------------------------------------
// プロトタイプ宣言
//--------------------------------------------------------------
void UpdateSelectTab(void);
void UpdateSelectStage(void);
void UpdateSelectOption(void);
void DrawSelectTabs(void);
void DrawSelectStagePanel(void);
void DrawSelectOption(void);
void DrawSelectSettingBar(float posY, float value, float minValue, float maxValue);
float GetSelectTabPosY(int tab);
bool IsSelectStageUnlocked(GAME_STAGE stage);

// Noise In Select
void InitializeSelectNoise(void);
void UpdateSelectNoise(void);
void DrawSelectNoise(void);
void FinalizeSelectNoise(void);
void WrapSelectNoiseUV(float* u, float* v);

// =========================================================
// Select初期化
// =========================================================
void InitializeSelect(void)
{
    StopBGM();
    PlayBGM(BGM_Select);

    // Select Noise Background
    InitializeSelectNoise();

    select_menu_textureID = LoadTexture(select_menu_intro);

    for (int i = 0; i < PANEL_ARROW_MAX; i++)
    {
        select_arrow_textureID[i] = LoadTexture(select_panel_arrow_file_name[i]);
    }

    for (int i = 0; i < PANEL_STATE_MAX; i++)
    {
        select_stage_panel_textureID[i] = LoadTexture(select_stage_panel_file_name[i]);
    }

    for (int i = 0; i < GAME_STAGE_MAX; i++)
    {
        select_stage_icon_textureID[i] = LoadTexture(select_stage_icon_file_name[i]);
    }

    for (int i = 0; i < TAB_MAX; i++)
    {
        for (int j = 0; j < TAB_STATE_MAX; j++)
        {
            select_tab_textureID[i][j] = LoadTexture(select_tab_file_name[i][j]);
        }
    }

    for (int i = 0; i < SELECT_SETTING_PIC_MAX; i++)
    {
        select_setting_textureID[i] = LoadTexture(select_setting_file_name[i]);
    }

    // 最初は一番上のStage TabをHoverし、最新解放Stageを表示する
    selectMenuState = SELECT_MENU_TAB;
    selectHoverTab = TAB_STAGE;
    selectStage = GetSaveUnlockedStage();
    selectSettingHover = SELECT_SETTING_BUTTON_SOUND;
    selectMasterVolume = GetMasterVolume();
    selectSensitivity = GetRightStickSensitivity();

    // Tab上下浮動Animation
    selectTabAnimeFrame = 0;
}

// =========================================================
// Select更新
// =========================================================
void UpdateSelect(void)
{
    selectTabAnimeFrame++;

    // Noise In Select
    UpdateSelectNoise();

    // Fade中は二重入力を防ぐ
    if (GetFade()->state != FADE_NONE) return;

    switch (selectMenuState)
    {
    case SELECT_MENU_TAB:
        UpdateSelectTab();
        break;

    case SELECT_MENU_STAGE:
        UpdateSelectStage();
        break;

    case SELECT_MENU_OPTION:
        UpdateSelectOption();
        break;

    default:
        break;
    }
}

// =========================================================
// Tab選択更新
// =========================================================
void UpdateSelectTab(void)
{
    if (GetControllerTrigger(NpadButton::Up::Index))
    {
        int tab = static_cast<int>(selectHoverTab) - 1;
        if (tab < 0) tab = TAB_MAX - 1;
        selectHoverTab = static_cast<SELECT_TAB_PIC>(tab);
        PlaySE(UI_Tab_Switch);
    }
    else if (GetControllerTrigger(NpadButton::Down::Index))
    {
        PlaySE(UI_Tab_Switch);
        int tab = static_cast<int>(selectHoverTab) + 1;
        if (tab >= TAB_MAX) tab = 0;
        selectHoverTab = static_cast<SELECT_TAB_PIC>(tab);
        PlaySE(UI_Tab_Switch);
    }

    if (!GetControllerTrigger(NpadButton::A::Index)) return;

    PlaySE(UI_Open_Menu);

    switch (selectHoverTab)
    {
    case TAB_STAGE:
        // Stage選択状態へ入る。現在表示中のStageはそのまま保持する
        selectMenuState = SELECT_MENU_STAGE;
        break;

    case TAB_OPTION:
        selectMenuState = SELECT_MENU_OPTION;
        selectSettingHover = SELECT_SETTING_BUTTON_SOUND;
        selectMasterVolume = GetMasterVolume();
        selectSensitivity = GetRightStickSensitivity();
        break;

    case TAB_BACK:
        StartFade(SCENE_TITLE);
        break;

    default:
        break;
    }
}

// =========================================================
// Stage選択更新
// =========================================================
void UpdateSelectStage(void)
{
    int stage = static_cast<int>(selectStage);

    if (GetControllerTrigger(NpadButton::Left::Index))
    {
        if (stage > static_cast<int>(GAME_STAGE_T_01))
        {
            selectStage = static_cast<GAME_STAGE>(stage - 1);
        }
        PlaySE(UI_Stage_Change);
    }
    else if (GetControllerTrigger(NpadButton::Right::Index))
    {
        if (stage < static_cast<int>(GAME_STAGE_MAX) - 1)
        {
            selectStage = static_cast<GAME_STAGE>(stage + 1);
        }
        PlaySE(UI_Stage_Change);
    }

    // Aで解放済みStageへ入る
    if (GetControllerTrigger(NpadButton::A::Index))
    {
        if (!IsSelectStageUnlocked(selectStage)) return;

        PlaySE(UI_Open_Menu);
        SetGameStage(selectStage);
        StartFade(SCENE_GAME);
        return;
    }

    // BでStage Tab操作へ戻る
    if (GetControllerTrigger(NpadButton::B::Index))
    {
        PlaySE(UI_Close_Menu);
        selectMenuState = SELECT_MENU_TAB;
        selectHoverTab = TAB_STAGE;
    }
}

// =========================================================
// Option更新
// =========================================================
void UpdateSelectOption(void)
{
    if (GetControllerTrigger(NpadButton::Up::Index))
    {
        selectSettingHover--;
        if (selectSettingHover < 0) selectSettingHover = SELECT_SETTING_BUTTON_MAX - 1;
        PlaySE(UI_Tab_Switch);
    }
    else if (GetControllerTrigger(NpadButton::Down::Index))
    {
        selectSettingHover++;
        if (selectSettingHover >= SELECT_SETTING_BUTTON_MAX) selectSettingHover = 0;
        PlaySE(UI_Tab_Switch);
    }

    if (GetControllerTrigger(NpadButton::Left::Index))
    {
        if (selectSettingHover == SELECT_SETTING_BUTTON_SOUND)
        {
            selectMasterVolume -= SETTING_VOLUME_STEP;
            if (selectMasterVolume < SETTING_VOLUME_MIN) selectMasterVolume = SETTING_VOLUME_MIN;
            SetMasterVolume(selectMasterVolume);
        }
        else
        {
            selectSensitivity -= SETTING_SENS_STEP;
            if (selectSensitivity < SETTING_SENS_MIN) selectSensitivity = SETTING_SENS_MIN;
            SetRightStickSensitivity(selectSensitivity);
            selectSensitivity = GetRightStickSensitivity();
            ResetRightStickInput();
        }
        PlaySE(UI_Stage_Option_Change);
    }
    else if (GetControllerTrigger(NpadButton::Right::Index))
    {
        if (selectSettingHover == SELECT_SETTING_BUTTON_SOUND)
        {
            selectMasterVolume += SETTING_VOLUME_STEP;
            if (selectMasterVolume > SETTING_VOLUME_MAX) selectMasterVolume = SETTING_VOLUME_MAX;
            SetMasterVolume(selectMasterVolume);
        }
        else
        {
            selectSensitivity += SETTING_SENS_STEP;
            if (selectSensitivity > SETTING_SENS_MAX) selectSensitivity = SETTING_SENS_MAX;
            SetRightStickSensitivity(selectSensitivity);
            selectSensitivity = GetRightStickSensitivity();
            ResetRightStickInput();
        }
        PlaySE(UI_Stage_Option_Change);
    }

    // BでOption Tab操作へ戻る
    if (GetControllerTrigger(NpadButton::B::Index))
    {
        PlaySE(UI_Close_Menu);
        SaveOptionData();
        selectMenuState = SELECT_MENU_TAB;
        selectHoverTab = TAB_OPTION;
        selectSettingHover = SELECT_SETTING_BUTTON_SOUND;
    }
}

// =========================================================
// Select描画
// =========================================================
void DrawSelect(void)
{
    // 白の背景
    DrawSpriteQuad(
        0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT,
        MakeFloat4(0.90f, 0.88f, 0.86f, 1.0f), 0.0f,
        0
    );
    // Noise
    DrawSelectNoise();

    // Select Intro
    DrawSpriteQuad(
        0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT,
        MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f,
        select_menu_textureID
    );

    // 左側TabはStage・Option操作中も表示する
    DrawSelectTabs();

    if (selectMenuState == SELECT_MENU_OPTION)
    {
        DrawSelectOption();
        return;
    }

    if (selectMenuState == SELECT_MENU_STAGE)
    {
        DrawSelectStagePanel();
    }
}

// =========================================================
// Tab描画
// =========================================================
void DrawSelectTabs(void)
{
    float animeFrame = static_cast<float>(selectTabAnimeFrame);

    for (int i = 0; i < TAB_MAX; i++)
    {
        bool isHovered = static_cast<int>(selectHoverTab) == i;
        int state = isHovered ? HOVERED : NONE;
        float scale = isHovered ? TAB_HOVERED_SCALE : 1.0f;
        float rotation = isHovered ? Deg2Rad(TAB_HOVERED_DEGREE) : 0.0f;

        float phase = static_cast<float>(i) * TAB_FLOAT_PHASE_OFFSET;
        float floatY = sinf(animeFrame * TAB_FLOAT_SPEED - phase) * TAB_FLOAT_HEIGHT;
        float drawPosY = GetSelectTabPosY(i) + floatY;
        float drawSizeW = TAB_SIZE_W * TAB_SIZE_SCALE * scale;
        float drawSizeH = TAB_SIZE_H * TAB_SIZE_SCALE * scale;

        // Tab影
        DrawSpriteQuad(
            TAB_POS_X + TAB_SHADOW_OFFSET_X,
            drawPosY + TAB_SHADOW_OFFSET_Y,
            drawSizeW, drawSizeH,
            MakeFloat4(0.0f, 0.0f, 0.0f, TAB_SHADOW_ALPHA), rotation,
            select_tab_textureID[i][state]
        );

        // Tab
        DrawSpriteQuad(
            TAB_POS_X, drawPosY,
            drawSizeW, drawSizeH,
            MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f), rotation,
            select_tab_textureID[i][state]
        );
    }
}

// =========================================================
// Stage Panel描画
// =========================================================
void DrawSelectStagePanel(void)
{
    int stage = static_cast<int>(selectStage);
    if (stage < 0 || stage >= GAME_STAGE_MAX) return;

    bool isUnlocked = IsSelectStageUnlocked(selectStage);

    // Unlock Panelは常に一番下へ表示する
    DrawSpriteQuad(
        PANEL_POS_X, PANEL_POS_Y,
        PANEL_SIZE_W, PANEL_SIZE_H,
        MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f,
        select_stage_panel_textureID[STAGE_UNLOCK]
    );

    // 現在選択中のStage Icon
    DrawSpriteQuad(
        PANEL_STAGE_ICON_POS_X, PANEL_STAGE_ICON_POS_Y,
        PANEL_STAGE_ICON_SIZE, PANEL_STAGE_ICON_SIZE,
        MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f,
        select_stage_icon_textureID[stage]
    );

    // 最初のStageでは左Arrowを表示しない
    if (selectStage > GAME_STAGE_T_01)
    {
        DrawSpriteQuad(
            PANEL_ARROW_POS_X_LEFT, PANEL_POS_Y,
            PANEL_ARROW_SIZE_W, PANEL_ARROW_SIZE_H,
            MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f,
            select_arrow_textureID[PANEL_ARROW_LEFT]
        );
    }

    // 最後のStageでは右Arrowを表示しない
    if (selectStage < GAME_STAGE_MAX - 1)
    {
        DrawSpriteQuad(
            PANEL_ARROW_POS_X_RIGHT, PANEL_POS_Y,
            PANEL_ARROW_SIZE_W, PANEL_ARROW_SIZE_H,
            MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f,
            select_arrow_textureID[PANEL_ARROW_RIGHT]
        );
    }

    // 未解放Stageの場合だけLocked画像を重ねる
    if (!isUnlocked)
    {
        DrawSpriteQuad(
            PANEL_POS_X, PANEL_POS_Y,
            PANEL_SIZE_W, PANEL_SIZE_H,
            MakeFloat4(1.0f, 1.0f, 1.0f, 0.7f), 0.0f,
            select_stage_panel_textureID[STAGE_LOCKED]
        );
    }
}

// =========================================================
// Option描画
// =========================================================
void DrawSelectOption(void)
{
    // Setting FrameはStage Panelと同じ中心座標へ表示する
    DrawSpriteQuad(
        SETTING_PANEL_POS_X, SETTING_PANEL_POS_Y,
        SETTING_PANEL_SIZE_W, SETTING_PANEL_SIZE_H,
        MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f,
        select_setting_textureID[SELECT_SETTING_PIC_FRAME]
    );

    DrawSelectSettingBar(
        SETTING_SOUND_BAR_POS_Y,
        selectMasterVolume,
        SETTING_VOLUME_MIN,
        SETTING_VOLUME_MAX
    );

    DrawSelectSettingBar(
        SETTING_SENS_BAR_POS_Y,
        selectSensitivity,
        SETTING_SENS_MIN,
        SETTING_SENS_MAX
    );

    int hoverPic = selectSettingHover == SELECT_SETTING_BUTTON_SOUND
        ? SELECT_SETTING_PIC_SOUND
        : SELECT_SETTING_PIC_SENSITIVITY;

    // Hover画像もSetting Frameと同じ中心座標へ表示する
    DrawSpriteQuad(
        SETTING_PANEL_POS_X, SETTING_PANEL_POS_Y,
        SETTING_PANEL_SIZE_W, SETTING_PANEL_SIZE_H,
        MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f,
        select_setting_textureID[hoverPic]
    );
}

// =========================================================
// Setting Bar描画
// =========================================================
void DrawSelectSettingBar(float posY, float value, float minValue, float maxValue)
{
    float range = maxValue - minValue;
    if (range <= 0.0f) return;

    float rate = (value - minValue) / range;
    if (rate < 0.0f) rate = 0.0f;
    if (rate > 1.0f) rate = 1.0f;

    DrawSpriteQuad(
        SETTING_BAR_POS_X, posY,
        SETTING_BAR_SIZE_W, SETTING_BAR_SIZE_H,
        MakeFloat4(0.10f, 0.08f, 0.12f, 0.85f),
        0
    );

    float width = SETTING_BAR_SIZE_W * rate;
    if (width <= 0.0f) return;

    float startX = SETTING_BAR_POS_X - SETTING_BAR_SIZE_W / 2.0f;
    float centerX = startX + width / 2.0f;

    DrawSpriteQuad(
        centerX, posY,
        width, SETTING_BAR_SIZE_H,
        MakeFloat4(0.95f, 0.78f, 0.45f, 1.0f),
        0
    );
}

// =========================================================
// Tab Y座標取得
// =========================================================
float GetSelectTabPosY(int tab)
{
    return TAB_STAGE_POS_Y + TAB_Y_DISTANCE * static_cast<float>(tab);
}

// =========================================================
// Stage解放確認
// =========================================================
bool IsSelectStageUnlocked(GAME_STAGE stage)
{
    if (stage < GAME_STAGE_T_01 || stage >= GAME_STAGE_MAX) return false;
    return stage <= GetSaveUnlockedStage();
}

// =========================================================
// 最新解放Stage設定
// =========================================================
void SetLatestUnlockedStage(GAME_STAGE stage)
{
    if (stage < GAME_STAGE_T_01 || stage >= GAME_STAGE_MAX) return;
    SetSaveUnlockedStage(stage);
}

// =========================================================
// 最新解放Stage取得
// =========================================================
GAME_STAGE GetLatestUnlockedStage(void)
{
    return GetSaveUnlockedStage();
}

// =========================================================
// Select終了処理
// =========================================================
void FinalizeSelect(void)
{
    selectTabAnimeFrame = 0;

    // Select Noise Background
    FinalizeSelectNoise();

    if (select_menu_textureID != 0)
    {
        UnloadTexture(select_menu_textureID);
        select_menu_textureID = 0;
    }

    for (int i = 0; i < PANEL_ARROW_MAX; i++)
    {
        if (select_arrow_textureID[i] == 0) continue;
        UnloadTexture(select_arrow_textureID[i]);
        select_arrow_textureID[i] = 0;
    }

    for (int i = 0; i < PANEL_STATE_MAX; i++)
    {
        if (select_stage_panel_textureID[i] == 0) continue;
        UnloadTexture(select_stage_panel_textureID[i]);
        select_stage_panel_textureID[i] = 0;
    }

    for (int i = 0; i < GAME_STAGE_MAX; i++)
    {
        if (select_stage_icon_textureID[i] == 0) continue;
        UnloadTexture(select_stage_icon_textureID[i]);
        select_stage_icon_textureID[i] = 0;
    }

    for (int i = 0; i < TAB_MAX; i++)
    {
        for (int j = 0; j < TAB_STATE_MAX; j++)
        {
            if (select_tab_textureID[i][j] == 0) continue;
            UnloadTexture(select_tab_textureID[i][j]);
            select_tab_textureID[i][j] = 0;
        }
    }

    for (int i = 0; i < SELECT_SETTING_PIC_MAX; i++)
    {
        if (select_setting_textureID[i] == 0) continue;
        UnloadTexture(select_setting_textureID[i]);
        select_setting_textureID[i] = 0;
    }

    // 一時Menu状態だけ戻す。latestUnlockedStageは保持する
    selectMenuState = SELECT_MENU_TAB;
    selectHoverTab = TAB_STAGE;
    selectStage = GetSaveUnlockedStage();
    selectSettingHover = SELECT_SETTING_BUTTON_SOUND;
}

// =========================================================
// Noise in Select
// =========================================================
// Select Noise Background初期化
void InitializeSelectNoise(void)
{
    for (int i = 0; i < SELECT_NOISE_PIC_NUM; i++)
    {
        select_noise_textureID[i] = LoadTexture(select_noise_file_name[i]);
    }

    // 開始位置をずらす
    selectNoise01U = static_cast<float>(rand() % 100) / 100.0f;
    selectNoise01V = static_cast<float>(rand() % 100) / 100.0f;

    selectNoise02U = static_cast<float>(rand() % 100) / 100.0f;
    selectNoise02V = static_cast<float>(rand() % 100) / 100.0f;

    // 淡い赤・淡い青
    const Float4 paleRed =  MakeFloat4(1.0f, 0.68f, 0.76f, 1.0f);
    const Float4 paleBlue = MakeFloat4(0.62f, 0.82f, 1.0f, 1.0f);

    // 少し濃い色
    const Float4 deepRed = MakeFloat4(0.78f, 0.32f, 0.44f, 1.0f);
    const Float4 deepBlue = MakeFloat4(0.3f, 0.52f, 0.82f, 1.0f);

    // 01と02は同じ色
    bool useRed = rand() % 2 == 0;
    if (useRed)
    {
        selectNoise01Color = paleRed;
        selectNoise02Color = deepRed;
    }
    else
    {
        selectNoise01Color = paleBlue;
        selectNoise02Color = deepBlue;
    }
}

// Select Noise Background更新
void UpdateSelectNoise(void)
{
    // Pattern 01
    selectNoise01U += SELECT_NOISE_01_SCROLL_SPEED;
    selectNoise01V += SELECT_NOISE_01_SCROLL_SPEED;

    // Pattern 02
    selectNoise02U += SELECT_NOISE_02_SCROLL_SPEED;
    selectNoise02V += SELECT_NOISE_02_SCROLL_SPEED;

    // UVを0.0～1.0へ戻す
    WrapSelectNoiseUV(&selectNoise01U, &selectNoise01V);
    WrapSelectNoiseUV(&selectNoise02U, &selectNoise02V);
}

// Select Noise UV補正
void WrapSelectNoiseUV(float* u, float* v)
{
    if (u == nullptr || v == nullptr) return;

    if (*u >= 1.0f) *u -= 1.0f;
    else if (*u < 0.0f) *u += 1.0f;

    if (*v >= 1.0f) *v -= 1.0f;
    else if (*v < 0.0f) *v += 1.0f;
}

// Select Noise Background描画
void DrawSelectNoise(void)
{
    float repeatU = SCREEN_WIDTH / SELECT_NOISE_SIZE;
    float repeatV = SCREEN_HEIGHT / SELECT_NOISE_SIZE;

    // Pattern 02
    DrawSpriteAnimation(
        0.0f, 0.0f,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        selectNoise02Color, 0.0f,
        selectNoise02U, selectNoise02V,
        repeatU, repeatV,
        select_noise_textureID[1]
    );

    // Pattern 01
    DrawSpriteAnimation(
        0.0f, 0.0f,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        selectNoise01Color, 0.0f,
        selectNoise01U, selectNoise01V,
        repeatU, repeatV,
        select_noise_textureID[0]
    );
}

// =========================================================
// Select Noise Background終了処理
// =========================================================
void FinalizeSelectNoise(void)
{
    for (int i = 0; i < SELECT_NOISE_PIC_NUM; i++)
    {
        if (select_noise_textureID[i] == 0) continue;
        UnloadTexture(select_noise_textureID[i]);
        select_noise_textureID[i] = 0;
    }

    selectNoise01U = 0.0f;
    selectNoise01V = 0.0f;
    selectNoise02U = 0.0f;
    selectNoise02V = 0.0f;

    selectNoise01Color = MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f);
    selectNoise02Color = MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f);
}
