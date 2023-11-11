#include <Windows.h>
#include "Log.h" 
#include "Hook.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    return TRUE;
}

struct ChunkBlockPos {
    uint8_t x;
    uint8_t z;
    uint16_t y;
};

struct Block;
class LevelChunk;

//class LevelChunk {
//    uint8_t gap0[312];
//    int64_t qword138;
//    int64_t qword140;
//
//    Block* getBlock(ChunkBlockPos* chunkBlockPos) {
//        uint8_t y = chunkBlockPos->y;
//        uint64_t verticalChunkY = y / 16;
//        uint64_t v5 = 96 * verticalChunkY + qword138 + 56;
//
//        if (verticalChunkY < (this->qword140 - this->qword138) / 96 && v5 != 0) {
//            typedef int64_t(__fastcall* FunctionPointer)(int64_t, uint64_t);
//            FunctionPointer* vtable = *reinterpret_cast<FunctionPointer**>(v5);
//            FunctionPointer func = vtable[3];
//
//            return reinterpret_cast<Block*>(func(v5, (y & 0xF) + 16 * (chunkBlockPos->z + 16 * chunkBlockPos->x)));
//        }
//        
//        return nullptr;
//    }
//};

struct BlockPos {
    int x;
    int y;
    int z;
};

typedef const BlockPos (__thiscall* _levelChunk_getTopRainBlockPos)(LevelChunk* self, const ChunkBlockPos& pos);
_levelChunk_getTopRainBlockPos levelChunk_getTopRainBlockPos;

const BlockPos LevelChunk_getTopRainBlockPos(LevelChunk* self, const ChunkBlockPos& chunkPos) {
    const BlockPos pos = levelChunk_getTopRainBlockPos(self, chunkPos);
    Log::Info("{} {} {}\n", pos.x, pos.y, pos.z);
    return pos;
}

extern "C" __declspec(dllexport) void ModInitializeHooks() {
    CreateHook(
        SlideAddress(0x3914AC0),
        LevelChunk_getTopRainBlockPos, levelChunk_getTopRainBlockPos
    );
}