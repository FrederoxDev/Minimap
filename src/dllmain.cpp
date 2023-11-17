#include <Windows.h>
#include "Log.h" 
#include "Hook.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    return TRUE;
}

//struct BlockPos {
//    int x;
//    int y;
//    int z;
//};
//
//class ChunkBlockPos {
//public:
//    uint8_t x;
//    uint8_t z;
//    int16_t y;
//
//    ChunkBlockPos(uint8_t x, uint8_t z, int16_t y) {
//        this->x = x;
//        this->z = z;
//        this->y = y;
//    }
//
//    ChunkBlockPos(const BlockPos& pos) {
//        this->x = pos.x % 16;
//        this->z = pos.z % 16;
//        this->y = pos.y;
//    }
//};
//
//class SubChunkBlockPos {
//public:
//    union {
//        struct {
//            uint8_t x;
//            uint8_t y;
//            uint8_t z;
//        };
//
//        uint32_t packed;
//    };
//
//    SubChunkBlockPos(uint8_t _x, uint8_t _y, uint8_t _z) :
//        x(_x),
//        y(_y),
//        z(_z)
//    {
//        packed &= 0xFFFFFF;
//    }
//
//    SubChunkBlockPos(const ChunkBlockPos& pos): 
//        x(pos.x), y(pos.y & 0xF), z(pos.z) 
//    {
//        packed &= 0xFFFFFF;
//    }
//
//    uint16_t index() const {
//        return (x << 8) + (z << 4) + y;
//    }
//};
//
//struct Block;
//class LevelChunk;
//
//class LevelChunk {
//public:
//    uint8_t gap0[312];
//    uint64_t qword138;
//    uint64_t qword140;
//
//    Block* getBlock(ChunkBlockPos& pos) {
//        uint64_t* qword138 = reinterpret_cast<uint64_t*>(this->qword138);
//        int64_t chunk_y = pos.y / 16;
//
//        // Gets the number of vertical chunks specific to the current dimension
//        uint64_t total_vertical_chunks = 0xAAAAAAAAAAAAAAABui64 * ((this->qword140 - this->qword138) >> 5);
//
//        qword138 = &qword138[12 * chunk_y + 7];
//        if (chunk_y < total_vertical_chunks && qword140 != 0) {
//            SubChunkBlockPos sub_chunk_pos(pos);
//
//            // This section causes the crash
//            using Function = Block*(__fastcall*)(uint64_t, uint16_t);
//            auto v16 = *reinterpret_cast<Function*>(*qword138 + 24);
//            return v16(reinterpret_cast<uint64_t>(qword138), sub_chunk_pos.index());
//        }
//        else {
//            return nullptr;
//        }
//    }
//};
//
//typedef BlockPos& (__thiscall* _levelChunk_getTopRainBlockPos)(void* a1, void* a2, void* a3);
//_levelChunk_getTopRainBlockPos levelChunk_getTopRainBlockPos;
//
//static BlockPos& LevelChunk_getTopRainBlockPos(LevelChunk* a1, ChunkBlockPos* a2, void* a3) {
//    BlockPos& pos = levelChunk_getTopRainBlockPos(a1, a2, a3);
//
//    if (pos.x > 0 && pos.z > 0 && pos.x < 16 && pos.z < 16) {
//        ChunkBlockPos chunk_pos(pos);
//        a1->getBlock(chunk_pos);
//    }
//
//    //Log::Info("{} {} {}\n", a1->qword138, a1->qword140, (a1->qword140 - a1->qword138) >> 5);
//    return pos;
//
//}

struct ChunkSource;

class BlockSource {
public:
    uintptr_t vftable;
};

typedef BlockSource* (__fastcall TBlockSource_Ctor)(BlockSource*, ChunkSource*, bool, bool);
TBlockSource_Ctor blockSource_Ctor;

static BlockSource* BlockSource_Ctor(BlockSource* self, ChunkSource* a1, bool a2, bool a3) {
    return blockSource_Ctor(self, a1, a2, a3);
}

//typedef void* (__thiscall* _chestBlockActor_tick)(void* self, BlockSource& blockSource);
//_chestBlockActor_tick chestBlockActor_tick;
//
//
//static void* ChestBlockActor_tick(void* self, BlockSource& blockSource) {
//    Log::Info("0x{:x}\n", blockSource.vftable - GetMinecraftBaseAddress() + 0x140000000);
//
//    return chestBlockActor_tick(self, blockSource);
//}

//static BlockSource* BlockSource_ctor(BlockSource* self, struct ChunkSource* a1, bool a2, bool a3) {
//    Log::Info("BlockSource::BlockSource\n");
//    return blockSource_ctor(self, a1, a2, a3);
//}

extern "C" __declspec(dllexport) void ModInitializeHooks() {
    Log::Info("[Minimap] Loaded!\n");

    /*CreateHook(
        SlideAddress(0x397CAD0),
        ChestBlockActor_tick, chestBlockActor_tick
    );*/

    CreateHook(
        SlideAddress(0x3146820),
        BlockSource_Ctor, blockSource_Ctor
    );
}