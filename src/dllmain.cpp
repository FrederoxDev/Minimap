#include <Windows.h>
#include "Log.h" 
#include "Hook.h"
#include <stdint.h>
#include "minecraft/common/world/level/BlockSource.h"
#include "minecraft/common/world/level/chunk/LevelChunk.h"
#include "minecraft/common/world/level/BlockPos.h"
#include "minecraft/common/world/level/ChunkBlockPos.h"
#include "minecraft/common/world/level/block/Block.h"
#include "core/math/Color.h"

BlockSource* g_blockSource = NULL;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    return TRUE;
}

typedef BlockPos& (__thiscall* _levelChunk_getTopRainBlockPos)(LevelChunk* a1, ChunkBlockPos* a2, void* a3);
_levelChunk_getTopRainBlockPos levelChunk_getTopRainBlockPos;

static BlockPos& LevelChunk_getTopRainBlockPos(LevelChunk* a1, ChunkBlockPos* a2, void* a3) {
    BlockPos& pos = levelChunk_getTopRainBlockPos(a1, a2, a3);

    if (pos.x > 0 && pos.z > 0 && pos.x < 16 && pos.z < 16) {
        ChunkBlockPos chunk_pos(pos);
        const Block* block = a1->getBlock(chunk_pos);

        if (block != nullptr && g_blockSource != nullptr) {
            Color color;
            Color* result = const_cast<Block*>(block)->getMapColor(&color, g_blockSource, &pos);
            Log::Info("{} {} {}\n", result->r, result->g, result->b);
        }
    }

    return pos;

}

typedef BlockSource* (__thiscall* _clientInstance_getRegion)(uintptr_t self);
_clientInstance_getRegion clientInstance_getRegion;

static BlockSource* ClientInstance_getRegion(uintptr_t self) {
    g_blockSource = clientInstance_getRegion(self);
    return g_blockSource;
}

//typedef Color* (__thiscall* _block_getMapColor)(void* self, Color* a2, BlockSource* a3, const BlockPos* a4);
//_block_getMapColor block_getMapColor;
//
//static Color* Block_getMapColor(void* self, Color* a2, BlockSource* a3, const BlockPos* a4) {
//    Color* col = block_getMapColor(self, a2, a3, a4);
//    Log::Info("{} {} {} {}\n", col->r * 255, col->g * 255, col->b * 255, col->a * 255);
//    return col;
//}

extern "C" __declspec(dllexport) void ModInitializeHooks() {
    CreateHook(
        SlideAddress(0x3914AC0),
        LevelChunk_getTopRainBlockPos, levelChunk_getTopRainBlockPos
    );

    CreateHook(
        SlideAddress(0x0380AA0),
        ClientInstance_getRegion, clientInstance_getRegion
    );
}

//__int64 __fastcall Actor::getDimensionBlockSource(Actor *__hidden this)
// called in ClientInstance getRegion func

// ClientInstance::init - 0x037AA40
// ClientInstance::getRegion - sub_140380AA0 

// public: class mce::Color Block::getMapColor(class BlockSource &, class BlockPos const &)const
// sub_1434B46D0