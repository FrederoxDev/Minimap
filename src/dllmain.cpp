#include <Windows.h>
#include "Log.h" 
#include "Hook.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    return TRUE;
}

struct BlockPos {
    int x;
    int y;
    int z;
};

class ChunkBlockPos {
public:
    uint8_t x;
    uint8_t z;
    int16_t y;

    ChunkBlockPos(uint8_t x, uint8_t z, int16_t y) {
        this->x = x;
        this->z = z;
        this->y = y;
    }

    ChunkBlockPos(const BlockPos& pos) {
        this->x = pos.x % 16;
        this->z = pos.z % 16;
        this->y = pos.y;
    }
};

struct Block;
class LevelChunk;

class LevelChunk {
public:
    uint8_t gap0[312];
    uint64_t qword138;
    uint64_t qword140;

    Block* getBlock(ChunkBlockPos* pos) {
        uint64_t* qword138 = reinterpret_cast<uint64_t*>(this->qword138);
        int64_t chunk_y = pos->y / 16;

        // Gets the number of vertical chunks specific to the current dimension
        uint64_t total_vertical_chunks = 0xAAAAAAAAAAAAAAABui64 * ((this->qword140 - this->qword138) >> 5);
        Log::Info("total_vertical_chunks: {}\n", total_vertical_chunks);

        qword138 = &qword138[12 * chunk_y + 7];
        if (chunk_y < total_vertical_chunks && qword140 != 0) {
            // TODO: x & z could easily be the wrong way round
            uint16_t index = (pos->y & 0xF) + 16 * (pos->z + 16 * pos->x);
            Log::Info("index: {}\n", index);

            // This section causes the crash
            using Function = Block*(__fastcall*)(uint64_t*, uint64_t);
            auto v16 = *reinterpret_cast<Function*>(*qword138 + 24);
            return v16(qword138, index);
        }
        else {
            return nullptr;
        }
    }
};

typedef BlockPos& (__thiscall* _levelChunk_getTopRainBlockPos)(void* a1, void* a2, void* a3);
_levelChunk_getTopRainBlockPos levelChunk_getTopRainBlockPos;

static BlockPos& LevelChunk_getTopRainBlockPos(LevelChunk* a1, ChunkBlockPos* a2, void* a3) {
    BlockPos& pos = levelChunk_getTopRainBlockPos(a1, a2, a3);

    if (pos.x > 0 && pos.z > 0 && pos.x < 16 && pos.z < 16) {
        ChunkBlockPos chunk_pos(pos);
        a1->getBlock(&chunk_pos);
    }

    //Log::Info("{} {} {}\n", a1->qword138, a1->qword140, (a1->qword140 - a1->qword138) >> 5);
    return pos;

}

extern "C" __declspec(dllexport) void ModInitializeHooks() {
    Log::Info("[Minimap] Loaded!\n");

    CreateHook(
        SlideAddress(0x3914AC0),
        LevelChunk_getTopRainBlockPos, levelChunk_getTopRainBlockPos
    );
}