// =========================================================
// color_change_block.cpp
// 
// 制作者:		日付：
// =========================================================
#include "color_change_block.h"
#include "texture.h"
#include "sprite.h"
#include "player.h"
#include "bullet.h"
#include "block.h"
#include "enemy_bullet.h"

// =========================================================
// マクロ定義
// =========================================================

// =========================================================
// 構造体定義
// =========================================================
struct Pic_Data_CCBlock
{
	const char FILE_NAME[256]{};
	Float2 PIC_SIZE{};
};

struct ENEMY_BLOCK_DATA
{
    GAME_STAGE myStage;
    Float2 size;

    MOVE_TYPE moveType;
    COLOR_CHANGE_TYPE changeType;
    BLOCK_COLOR blockColor;

    float speed; // どの方向往復してもこのスピード
    Float2 patrolStartPos;
    Float2 patrolEndPos;
    int changeCD; //frame
};

// =========================================================
// グローバル変数
// =========================================================
Float2 CENTER_OFFSET_CCBLOCK = MakeFloat2(MAP_BLOCK_WIDTH * 8.5f, MAP_BLOCK_HEIGHT * 5.0f);
Pic_Data_CCBlock ccblock_pic_data =
{
    "rom:/Color_Change_Block_Origin.tga",
    MakeFloat2(120.0f, 120.0f)
};
COLOR_CHANGE_BLOCK CCBlock[CCBLOCK_MAX]{};
unsigned int CCBlockTextureId{};

const ENEMY_BLOCK_DATA ccblock_data[] =
{
    #pragma region T_02
    {
        GAME_STAGE_T_02,
        MakeFloat2(120.0f, 480.0f),
        MOVE_TYPE::STATIC,
        COLOR_CHANGE_TYPE::NO_CHANGE,
        BLOCK_COLOR::BLOCK_ORIGIN,
        0.0f,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 44.0f  - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 17.5f - CENTER_OFFSET_CCBLOCK.y),
        MakeFloat2(
            MAP_BLOCK_WIDTH * 44.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 17.5f - CENTER_OFFSET_CCBLOCK.y),
        0
    },
    {
        GAME_STAGE_T_02,
        MakeFloat2(120.0f, 360.0f),
        MOVE_TYPE::STATIC,
        COLOR_CHANGE_TYPE::NO_CHANGE,
        BLOCK_COLOR::BLOCK_ORIGIN,
        0.0f,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 48.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 20.0f - CENTER_OFFSET_CCBLOCK.y),
        MakeFloat2(
            MAP_BLOCK_WIDTH * 48.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 20.0f - CENTER_OFFSET_CCBLOCK.y),
        0
    },
    {
        GAME_STAGE_T_02,
        MakeFloat2(120.0f, 480.0f),
        MOVE_TYPE::STATIC,
        COLOR_CHANGE_TYPE::NO_CHANGE,
        BLOCK_COLOR::BLOCK_BLUE,
        0.0f,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 51.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 11.5f - CENTER_OFFSET_CCBLOCK.y),
        MakeFloat2(
            MAP_BLOCK_WIDTH * 51.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 11.5f - CENTER_OFFSET_CCBLOCK.y),
        0
    },
    {
        GAME_STAGE_T_02,
        MakeFloat2(240.0f, 120.0f),
        MOVE_TYPE::MOVING,
        COLOR_CHANGE_TYPE::NO_CHANGE,
        BLOCK_COLOR::BLOCK_ORIGIN,
        5.0f,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 41.5f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 8.0f - CENTER_OFFSET_CCBLOCK.y),
        MakeFloat2(
            MAP_BLOCK_WIDTH * 34.5f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 8.0f - CENTER_OFFSET_CCBLOCK.y),
        0
    },
    {
        GAME_STAGE_T_02,
        MakeFloat2(360.0f, 120.0f),
        MOVE_TYPE::MOVING,
        COLOR_CHANGE_TYPE::NO_CHANGE,
        BLOCK_COLOR::BLOCK_RED,
        5.0f,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 19.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 8.0f - CENTER_OFFSET_CCBLOCK.y),
        MakeFloat2(
            MAP_BLOCK_WIDTH * 30.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 8.0f - CENTER_OFFSET_CCBLOCK.y),
        0
    },
    {
        GAME_STAGE_T_02,
        MakeFloat2(360.0f, 120.0f),
        MOVE_TYPE::MOVING,
        COLOR_CHANGE_TYPE::NO_CHANGE,
        BLOCK_COLOR::BLOCK_BLUE,
        5.0f,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 16.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 8.0f - CENTER_OFFSET_CCBLOCK.y),
        MakeFloat2(
            MAP_BLOCK_WIDTH * 5.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 8.0f - CENTER_OFFSET_CCBLOCK.y),
        0
    },
    #pragma endregion
    #pragma region T_03
    {
        GAME_STAGE_T_03,
        MakeFloat2(360.0f, 120.0f),
        MOVE_TYPE::STATIC,
        COLOR_CHANGE_TYPE::NO_CHANGE,
        BLOCK_COLOR::BLOCK_RED,
        5.0f,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 16.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 12.0f - CENTER_OFFSET_CCBLOCK.y),
        MakeFloat2(
            MAP_BLOCK_WIDTH * 16.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 8.0f - CENTER_OFFSET_CCBLOCK.y),
        0
    },
    {
        GAME_STAGE_T_03,
        MakeFloat2(360.0f, 120.0f),
        MOVE_TYPE::STATIC,
        COLOR_CHANGE_TYPE::NO_CHANGE,
        BLOCK_COLOR::BLOCK_BLUE,
        5.0f,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 25.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 12.0f - CENTER_OFFSET_CCBLOCK.y),
        MakeFloat2(
            MAP_BLOCK_WIDTH * 25.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 8.0f - CENTER_OFFSET_CCBLOCK.y),
        0
    },
    #pragma endregion
    #pragma region S_01
    //area1
    {
        GAME_STAGE_S_01,
        MakeFloat2(120.0f, 360.0f),
        MOVE_TYPE::STATIC,
        COLOR_CHANGE_TYPE::NO_CHANGE,
        BLOCK_COLOR::BLOCK_ORIGIN,
        0.0f,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 10.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 95.0f - CENTER_OFFSET_CCBLOCK.y),
        MakeFloat2(
            MAP_BLOCK_WIDTH * 10.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 95.0f - CENTER_OFFSET_CCBLOCK.y),
        0
    },
    {
        GAME_STAGE_S_01,
        MakeFloat2(120.0f, 360.0f),
        MOVE_TYPE::STATIC,
        COLOR_CHANGE_TYPE::NO_CHANGE,
        BLOCK_COLOR::BLOCK_ORIGIN,
        0.0f,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 17.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 92.0f - CENTER_OFFSET_CCBLOCK.y),
        MakeFloat2(
            MAP_BLOCK_WIDTH * 17.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 92.0f - CENTER_OFFSET_CCBLOCK.y),
        0
    },

    // area2
    {
        GAME_STAGE_S_01,
        MakeFloat2(240.0f, 120.0f),
        MOVE_TYPE::STATIC,
        COLOR_CHANGE_TYPE::NO_CHANGE,
        BLOCK_COLOR::BLOCK_ORIGIN,
        0.0f,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 35.5f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 81.0f - CENTER_OFFSET_CCBLOCK.y),
        MakeFloat2(
            MAP_BLOCK_WIDTH * 35.5f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 81.0f - CENTER_OFFSET_CCBLOCK.y),
        0
    },
    {
        GAME_STAGE_S_01,
        MakeFloat2(120.0f, 240.0f),
        MOVE_TYPE::STATIC,
        COLOR_CHANGE_TYPE::NO_CHANGE,
        BLOCK_COLOR::BLOCK_ORIGIN,
        0.0f,
        MakeFloat2(
            MAP_BLOCK_WIDTH *41.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 79.5f - CENTER_OFFSET_CCBLOCK.y),
        MakeFloat2(
            MAP_BLOCK_WIDTH * 41.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 79.5f - CENTER_OFFSET_CCBLOCK.y),
        0
    },
    {
        GAME_STAGE_S_01,
        MakeFloat2(360.0f, 120.0f),
        MOVE_TYPE::STATIC,
        COLOR_CHANGE_TYPE::NO_CHANGE,
        BLOCK_COLOR::BLOCK_ORIGIN,
        0.0f,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 49.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 81.0f - CENTER_OFFSET_CCBLOCK.y),
        MakeFloat2(
            MAP_BLOCK_WIDTH * 49.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 81.0f - CENTER_OFFSET_CCBLOCK.y),
        0
    },

    //area3
    {
        GAME_STAGE_S_01,
        MakeFloat2(120.0f, 480.0f),
        MOVE_TYPE::STATIC,
        COLOR_CHANGE_TYPE::NO_CHANGE,
        BLOCK_COLOR::BLOCK_RED,
        0.0f,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 59.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 60.5f - CENTER_OFFSET_CCBLOCK.y),
        MakeFloat2(
            MAP_BLOCK_WIDTH * 59.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 60.5f - CENTER_OFFSET_CCBLOCK.y),
        0
    },
    {
        GAME_STAGE_S_01,
        MakeFloat2(120.0f, 360.0f),
        MOVE_TYPE::STATIC,
        COLOR_CHANGE_TYPE::NO_CHANGE,
        BLOCK_COLOR::BLOCK_RED,
        0.0f,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 65.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 56.0f - CENTER_OFFSET_CCBLOCK.y),

        MakeFloat2(
            MAP_BLOCK_WIDTH * 65.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 56.0f - CENTER_OFFSET_CCBLOCK.y),
        0
    },
    {
        GAME_STAGE_S_01,
        MakeFloat2(360.0f, 120.0f),
        MOVE_TYPE::MOVING,
        COLOR_CHANGE_TYPE::NO_CHANGE,
        BLOCK_COLOR::BLOCK_BLUE,
        5.0f,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 74.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 63.0f - CENTER_OFFSET_CCBLOCK.y),

        MakeFloat2(
            MAP_BLOCK_WIDTH * 81.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 58.0f - CENTER_OFFSET_CCBLOCK.y),
        0
    },

    //area4
    {
        GAME_STAGE_S_01,
        MakeFloat2(360.0f, 120.0f),
        MOVE_TYPE::MOVING,
        COLOR_CHANGE_TYPE::NO_CHANGE,
        BLOCK_COLOR::BLOCK_RED,
        5.0f,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 73.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 44.0f - CENTER_OFFSET_CCBLOCK.y),
        MakeFloat2(
            MAP_BLOCK_WIDTH * 73.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 34.0f - CENTER_OFFSET_CCBLOCK.y),
        0
    },
    {
        GAME_STAGE_S_01,
        MakeFloat2(360.0f, 120.0f),
        MOVE_TYPE::STATIC,
        COLOR_CHANGE_TYPE::NO_CHANGE,
        BLOCK_COLOR::BLOCK_BLUE,
        0.0f,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 79.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 28.0f - CENTER_OFFSET_CCBLOCK.y),
        MakeFloat2(
            MAP_BLOCK_WIDTH * 79.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 28.0f - CENTER_OFFSET_CCBLOCK.y),
        0
    },
    {
        GAME_STAGE_S_01,
        MakeFloat2(360.0f, 120.0f),
        MOVE_TYPE::STATIC,
        COLOR_CHANGE_TYPE::NO_CHANGE,
        BLOCK_COLOR::BLOCK_RED,
        0.0f,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 74.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 25.0f - CENTER_OFFSET_CCBLOCK.y),
        MakeFloat2(
            MAP_BLOCK_WIDTH * 74.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 25.0f - CENTER_OFFSET_CCBLOCK.y),
        0
    },
    {
        GAME_STAGE_S_01,
        MakeFloat2(360.0f, 120.0f),
        MOVE_TYPE::STATIC,
        COLOR_CHANGE_TYPE::NO_CHANGE,
        BLOCK_COLOR::BLOCK_BLUE,
        0.0f,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 79.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 22.0f - CENTER_OFFSET_CCBLOCK.y),
        MakeFloat2(
            MAP_BLOCK_WIDTH * 74.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 22.0f - CENTER_OFFSET_CCBLOCK.y),
        0
    },
    {
        GAME_STAGE_S_01,
        MakeFloat2(360.0f, 120.0f),
        MOVE_TYPE::STATIC,
        COLOR_CHANGE_TYPE::NO_CHANGE,
        BLOCK_COLOR::BLOCK_RED,
        0.0f,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 73.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 19.0f - CENTER_OFFSET_CCBLOCK.y),
        MakeFloat2(
            MAP_BLOCK_WIDTH * 73.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 19.0f - CENTER_OFFSET_CCBLOCK.y),
        0
    },

    #pragma endregion
    #pragma region S_02
    //area1
    {
        GAME_STAGE_S_02,
        MakeFloat2(360.0f, 120.0f),
        MOVE_TYPE::MOVING,
        COLOR_CHANGE_TYPE::NO_CHANGE,
        BLOCK_COLOR::BLOCK_ORIGIN,
        5.0f,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 23.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 95.0f - CENTER_OFFSET_CCBLOCK.y),
        MakeFloat2(
            MAP_BLOCK_WIDTH * 13.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 95.0f - CENTER_OFFSET_CCBLOCK.y),
        0
    },
    {
        GAME_STAGE_S_02,
        MakeFloat2(360.0f, 120.0f),
        MOVE_TYPE::MOVING,
        COLOR_CHANGE_TYPE::NO_CHANGE,
        BLOCK_COLOR::BLOCK_BLUE,
        5.0f,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 32.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 96.0f - CENTER_OFFSET_CCBLOCK.y),
        MakeFloat2(
            MAP_BLOCK_WIDTH * 32.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 89.0f - CENTER_OFFSET_CCBLOCK.y),
        0
    },

    // area3
    {
        GAME_STAGE_S_02,
        MakeFloat2(120.0f, 360.0f),
        MOVE_TYPE::STATIC,
        COLOR_CHANGE_TYPE::NO_CHANGE,
        BLOCK_COLOR::BLOCK_RED,
        0.0f,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 72.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 68.0f - CENTER_OFFSET_CCBLOCK.y),
        MakeFloat2(
            MAP_BLOCK_WIDTH * 72.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 68.0f - CENTER_OFFSET_CCBLOCK.y),
        0
    },

    // area4
    {
        GAME_STAGE_S_02,
        MakeFloat2(240.0f, 120.0f),
        MOVE_TYPE::STATIC,
        COLOR_CHANGE_TYPE::NO_CHANGE,
        BLOCK_COLOR::BLOCK_BLUE,
        0.0f,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 64.5f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 53.0f - CENTER_OFFSET_CCBLOCK.y),
        MakeFloat2(
            MAP_BLOCK_WIDTH * 64.5f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 53.0f - CENTER_OFFSET_CCBLOCK.y),
        0
    },
    {
        GAME_STAGE_S_02,
        MakeFloat2(360.0f, 120.0f),
        MOVE_TYPE::MOVING,
        COLOR_CHANGE_TYPE::NO_CHANGE,
        BLOCK_COLOR::BLOCK_RED,
        4.0f,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 50.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 53.0f - CENTER_OFFSET_CCBLOCK.y),
        MakeFloat2(
            MAP_BLOCK_WIDTH * 48.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 47.0f - CENTER_OFFSET_CCBLOCK.y),
        0
    },

    // area 5
    {
        GAME_STAGE_S_02,
        MakeFloat2(360.0f, 120.0f),
        MOVE_TYPE::MOVING,
        COLOR_CHANGE_TYPE::NO_CHANGE,
        BLOCK_COLOR::BLOCK_ORIGIN,
        8.0f,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 34.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 52.0f - CENTER_OFFSET_CCBLOCK.y),
        MakeFloat2(
            MAP_BLOCK_WIDTH * 34.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 45.0f - CENTER_OFFSET_CCBLOCK.y),
        0
    },
    {
        GAME_STAGE_S_02,
        MakeFloat2(360.0f, 120.0f),
        MOVE_TYPE::MOVING,
        COLOR_CHANGE_TYPE::NO_CHANGE,
        BLOCK_COLOR::BLOCK_ORIGIN,
        8.0f,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 26.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 46.0f - CENTER_OFFSET_CCBLOCK.y),
        MakeFloat2(
            MAP_BLOCK_WIDTH * 26.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 53.0f - CENTER_OFFSET_CCBLOCK.y),
        0
    },

    // area vertical
    {
        GAME_STAGE_S_02,
        MakeFloat2(120.0f, 360.0f),
        MOVE_TYPE::STATIC,
        COLOR_CHANGE_TYPE::NO_CHANGE,
        BLOCK_COLOR::BLOCK_BLUE,
        0.0f,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 15.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 40.0f - CENTER_OFFSET_CCBLOCK.y),
        MakeFloat2(
            MAP_BLOCK_WIDTH * 15.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 40.0f - CENTER_OFFSET_CCBLOCK.y),
        0
    },
    {
        GAME_STAGE_S_02,
        MakeFloat2(120.0f, 360.0f),
        MOVE_TYPE::STATIC,
        COLOR_CHANGE_TYPE::NO_CHANGE,
        BLOCK_COLOR::BLOCK_BLUE,
        0.0f,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 15.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 28.0f - CENTER_OFFSET_CCBLOCK.y),
        MakeFloat2(
            MAP_BLOCK_WIDTH * 15.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 28.0f - CENTER_OFFSET_CCBLOCK.y),
        0
    },
    {
        GAME_STAGE_S_02,
        MakeFloat2(120.0f, 360.0f),
        MOVE_TYPE::STATIC,
        COLOR_CHANGE_TYPE::NO_CHANGE,
        BLOCK_COLOR::BLOCK_RED,
        0.0f,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 21.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 34.0f - CENTER_OFFSET_CCBLOCK.y),
        MakeFloat2(
            MAP_BLOCK_WIDTH * 21.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 34.0f - CENTER_OFFSET_CCBLOCK.y),
        0
    },
    {
        GAME_STAGE_S_02,
        MakeFloat2(120.0f, 360.0f),
        MOVE_TYPE::STATIC,
        COLOR_CHANGE_TYPE::NO_CHANGE,
        BLOCK_COLOR::BLOCK_RED,
        0.0f,
        MakeFloat2(
            MAP_BLOCK_WIDTH * 21.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 22.0f - CENTER_OFFSET_CCBLOCK.y),
        MakeFloat2(
            MAP_BLOCK_WIDTH * 21.0f - CENTER_OFFSET_CCBLOCK.x,
            MAP_BLOCK_HEIGHT * 22.0f - CENTER_OFFSET_CCBLOCK.y),
        0
    },

    #pragma endregion
};

const Float4 CC_BLOCK_RED_COLOR = MakeFloat4(0.88f, 0.0f, 0.14f, 1.0f);
const Float4 CC_BLOCK_BLUE_COLOR = MakeFloat4(0.32f, 0.4f, 0.84f, 1.0f);
const Float4 CC_BLOCK_ORIGIN_COLOR = MakeFloat4(1.0f, 1.0f, 1.0f, 1.0f);

// =========================================================
// 関数宣言
// =========================================================
void HandleBlockMoving(COLOR_CHANGE_BLOCK* targetCCBlock);
Float4 GetCCBlockColor(COLOR_CHANGE_BLOCK* targetCCBlock);

// =========================================================
// 関数
// =========================================================
void InitializeColorChangeBlock()
{
    CCBlockTextureId = LoadTexture(ccblock_pic_data.FILE_NAME);
    ReloadColorChangeBlockStage();
}

// =========================================================
// 更新
// =========================================================
void UpdateColorChangeBlock()
{
    for (int i = 0; i < CCBLOCK_MAX; i++)
    {
        COLOR_CHANGE_BLOCK* targetCCBlock = &CCBlock[i];
        if (!targetCCBlock->use) continue;

        // Player color check
        PLAYER* player = GetPlayer();
        if (targetCCBlock->colorType == BLOCK_COLOR::BLOCK_RED)
        {
            targetCCBlock->isSameColorAsPlayer = player->COLORSTATE == COLOR_STATE::RED;
        }
        else if (targetCCBlock->colorType == BLOCK_COLOR::BLOCK_BLUE)
        {
            targetCCBlock->isSameColorAsPlayer = player->COLORSTATE == COLOR_STATE::BLUE;
        }
        else targetCCBlock->isSameColorAsPlayer = false;

        Float2 oldPos = targetCCBlock->pos;
        // MoveType
        switch (targetCCBlock->moveType)
        {
        case MOVE_TYPE::STATIC:
            targetCCBlock->vel = MakeFloat2(0.0f, 0.0f);
            break;
        case MOVE_TYPE::MOVING:
            HandleBlockMoving(targetCCBlock);
            break;
        }

        // ChangeType
        switch (targetCCBlock->colorChangeType)
        {
        case COLOR_CHANGE_TYPE::NO_CHANGE:
            break;
        case COLOR_CHANGE_TYPE::AUTO_CHANGE:
            break;
        }

        targetCCBlock->pos.x += targetCCBlock->vel.x;
        targetCCBlock->pos.y += targetCCBlock->vel.y;
        targetCCBlock->moveDelta = MakeFloat2(targetCCBlock->pos.x - oldPos.x, targetCCBlock->pos.y - oldPos.y);
        targetCCBlock->CollisionPos = targetCCBlock->pos;
        targetCCBlock->CollisionSize = targetCCBlock->size;
    }
}

// =========================================================
// 描画
// =========================================================
void DrawColorChangeBlock()
{
    for (int i = 0; i < CCBLOCK_MAX; i++)
    {
        COLOR_CHANGE_BLOCK* targetCCBlock = &CCBlock[i];
        if (!targetCCBlock->use) continue;

        float repeatU = targetCCBlock->size.x / ccblock_pic_data.PIC_SIZE.x;
        float repeatV = targetCCBlock->size.y / ccblock_pic_data.PIC_SIZE.y;
        Float4 color = GetCCBlockColor(targetCCBlock);

        DrawSpriteQuad_UV_Scroll(
            targetCCBlock->pos.x, targetCCBlock->pos.y,
            targetCCBlock->size.x, targetCCBlock->size.y,
            0.0f, 0.0f,
            repeatU, repeatV,
            color,
            CCBlockTextureId
        );
    }
}

// =========================================================
// 終了処理
// =========================================================
void FinalizeColorChangeBlock()
{
    if (CCBlockTextureId != 0)
    {
        UnloadTexture(CCBlockTextureId);
        CCBlockTextureId = 0;
    }

    for (int i = 0; i < CCBLOCK_MAX; i++)
    {
        CCBlock[i] = COLOR_CHANGE_BLOCK{};
    }
}

// =========================================================
// 往復移動のブロックの処理
// =========================================================
void HandleBlockMoving(COLOR_CHANGE_BLOCK* targetCCBlock)
{
    if (targetCCBlock == nullptr) return;

    Float2 targetPos = targetCCBlock->isMovingToEnd ? targetCCBlock->patrolEndPos : targetCCBlock->patrolStartPos;
    Float2 toTarget = MakeFloat2(targetPos.x - targetCCBlock->pos.x, targetPos.y - targetCCBlock->pos.y);
    float distance = sqrtf(toTarget.x * toTarget.x + toTarget.y * toTarget.y);

    if (distance <= targetCCBlock->moveSpeed)
    {
        targetCCBlock->pos = targetPos;
        targetCCBlock->vel = MakeFloat2(0.0f, 0.0f);
        targetCCBlock->isMovingToEnd = !targetCCBlock->isMovingToEnd;
        return;
    }

    targetCCBlock->moveDir = MakeFloat2(toTarget.x / distance, toTarget.y / distance);
    targetCCBlock->vel = MakeFloat2(targetCCBlock->moveDir.x * targetCCBlock->moveSpeed, targetCCBlock->moveDir.y * targetCCBlock->moveSpeed);
}

// =========================================================
// カラータイプから色を決める
// =========================================================
Float4 GetCCBlockColor(COLOR_CHANGE_BLOCK* targetCCBlock)
{
    BLOCK_COLOR colorType = targetCCBlock->colorType;
    Float4 ret{};
    switch (colorType)
    {
    case BLOCK_COLOR::BLOCK_RED:
        ret = CC_BLOCK_RED_COLOR;
        break;
    case BLOCK_COLOR::BLOCK_BLUE:
        ret = CC_BLOCK_BLUE_COLOR;
        break;
    case BLOCK_COLOR::BLOCK_ORIGIN:
    default:
        ret = CC_BLOCK_ORIGIN_COLOR;
        break;
    }

    if (colorType != BLOCK_COLOR::BLOCK_ORIGIN && !targetCCBlock->isSameColorAsPlayer)
    {
        ret.w = 0.5f;
    }

    return ret;
}

// =========================================================
// カラーブロックゲッター
// =========================================================
COLOR_CHANGE_BLOCK* GetColorChangeBlock()
{
    return &CCBlock[0];
}

// =========================================================
// PlayerがColor Change BlockとCollisionするか
// =========================================================
bool CanPlayerCollideColorChangeBlock(const COLOR_CHANGE_BLOCK* targetCCBlock)
{
    if (targetCCBlock == nullptr) return false;
    if (!targetCCBlock->use) return false;
    if (targetCCBlock->colorType == BLOCK_COLOR::BLOCK_ORIGIN) return true;

    return targetCCBlock->isSameColorAsPlayer;
}

// =========================================================
// Blockのデータをロード
// =========================================================
void ReloadColorChangeBlockStage()
{
    for (int i = 0; i < CCBLOCK_MAX; i++)
    {
        CCBlock[i] = COLOR_CHANGE_BLOCK{};
    }

    GAME_STAGE currentStage = GetCurrentGameStage();
    const int dataCount = sizeof(ccblock_data) / sizeof(ccblock_data[0]);

    int blockCnt = 0;
    for (int i = 0; i < dataCount; i++)
    {
        const ENEMY_BLOCK_DATA& data = ccblock_data[i];

        if (data.myStage != currentStage) continue;
        if (blockCnt >= CCBLOCK_MAX) break;

        COLOR_CHANGE_BLOCK* targetCCBlock = &CCBlock[blockCnt];

        targetCCBlock->pos = data.patrolStartPos;
        targetCCBlock->size = data.size;
        targetCCBlock->vel = MakeFloat2(0.0f, 0.0f);
        targetCCBlock->moveDelta = MakeFloat2(0.0f, 0.0f);

        targetCCBlock->CollisionPos = targetCCBlock->pos;
        targetCCBlock->CollisionSize = targetCCBlock->size;
        targetCCBlock->patrolStartPos = data.patrolStartPos;
        targetCCBlock->patrolEndPos = data.patrolEndPos;

        Float2 vectorToEnd = MakeFloat2(
            data.patrolEndPos.x - data.patrolStartPos.x,
            data.patrolEndPos.y - data.patrolStartPos.y
        );

        float length = sqrtf(
            vectorToEnd.x * vectorToEnd.x +
            vectorToEnd.y * vectorToEnd.y
        );

        if (length > 0.000001f)
        {
            targetCCBlock->moveDir = MakeFloat2(
                vectorToEnd.x / length,
                vectorToEnd.y / length
            );
        }
        else
        {
            targetCCBlock->moveDir = MakeFloat2(0.0f, 0.0f);
        }

        targetCCBlock->moveSpeed = data.speed;
        targetCCBlock->isMovingToEnd = true;
        targetCCBlock->moveType = data.moveType;
        targetCCBlock->colorChangeType = data.changeType;
        targetCCBlock->colorType = data.blockColor;
        targetCCBlock->isSameColorAsPlayer = false;
        targetCCBlock->use = true;

        blockCnt++;
    }
}

// =========================================================
// Tutorial Stage 03敵を撃破したあとStaticのブロックをMovingにする
// =========================================================
void SetTutorialColorBlockMoving()
{
    if (GetCurrentGameStage() != GAME_STAGE_T_03) return;

    for (int i = 0; i < CCBLOCK_MAX; i++)
    {
        COLOR_CHANGE_BLOCK* targetCCBlock = &CCBlock[i];

        if (!targetCCBlock->use) continue;
        if (targetCCBlock->moveType != MOVE_TYPE::STATIC) continue;
        if (targetCCBlock->moveSpeed <= 0.0f) continue;

        targetCCBlock->vel = MakeFloat2(0.0f, 0.0f);
        targetCCBlock->moveDelta = MakeFloat2(0.0f, 0.0f);
        targetCCBlock->isMovingToEnd = true;
        targetCCBlock->moveType = MOVE_TYPE::MOVING;
    }
}