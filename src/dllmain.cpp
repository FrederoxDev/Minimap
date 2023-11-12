#include <Windows.h>
#include "Log.h" 
#include "Hook.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    return TRUE;
}

struct ChunkBlockPos {
    uint8_t x;
    uint8_t z;
    int16_t y;
};

struct Block;
class LevelChunk;

class LevelChunk {
    uint8_t gap0[312];
    int64_t qword138;
    int64_t qword140;

public:
    Block* getBlock(ChunkBlockPos* chunkBlockPos) {
        uint8_t y = chunkBlockPos->y;
        uint64_t verticalChunkY = y / 16;
        uint64_t v5 = 96 * verticalChunkY + qword138 + 56;

        if (verticalChunkY < (this->qword140 - this->qword138) / 96 && v5 != 0) {
            typedef int64_t(__fastcall* FunctionPointer)(int64_t, uint64_t);
            FunctionPointer* vtable = *reinterpret_cast<FunctionPointer**>(v5);
            FunctionPointer func = vtable[3];

            return reinterpret_cast<Block*>(func(v5, (y & 0xF) + 16 * (chunkBlockPos->z + 16 * chunkBlockPos->x)));
        }
        
        return nullptr;
    }
};

struct BlockPos {
    int x;
    int y;
    int z;
};

typedef BlockPos* (__thiscall* _levelChunk_getTopRainBlockPos)(void* a1, void* a2, void* a3);
_levelChunk_getTopRainBlockPos levelChunk_getTopRainBlockPos;

static BlockPos* LevelChunk_getTopRainBlockPos(LevelChunk* a1, ChunkBlockPos* a2, void* a3) {
    BlockPos* pos = levelChunk_getTopRainBlockPos(a1, a2, a3);
    if (pos->x > 0 && pos->z > 0 && pos->x < 16 && pos->z < 16) {
        Log::Info("{} {} -> {}\n", pos->x, pos->z, pos->y);
        //bool is_null = a1->getBlock(a2) == nullptr;
        //Log::Info("{} {} -> {} {}\n", pos->x, pos->z, pos->y, is_null);
    }
    return pos;
}

extern "C" __declspec(dllexport) void ModInitializeHooks() {
    Log::Info("[Minimap] Loaded!\n");

    CreateHook(
        SlideAddress(0x3914AC0),
        LevelChunk_getTopRainBlockPos, levelChunk_getTopRainBlockPos
    );
}