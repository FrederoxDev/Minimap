#include <Windows.h>
#include <stdint.h>
#include "minecraft/common/world/level/BlockSource.h"
#include "minecraft/common/world/level/chunk/LevelChunk.h"
#include "minecraft/common/world/level/BlockPos.h"
#include "minecraft/common/world/level/ChunkBlockPos.h"
#include "minecraft/common/world/level/block/Block.h"
#include "core/math/Color.h"
#include <iostream>
#include <fstream>
#include "client/common/client/game/ClientInstance.h"
#include "minecraft/common/world/level/dimension/Dimension.h"

#include "amethyst/Log.h"
#include "amethyst/HookManager.h"

#define WIDTH 16

static ClientInstance* g_clientInstance = nullptr;
Color g_colormap[WIDTH][WIDTH];

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    return TRUE;
}

//typedef BlockPos& (__thiscall* _levelChunk_getTopRainBlockPos)(LevelChunk* a1, ChunkBlockPos* a2, void* a3);
//_levelChunk_getTopRainBlockPos levelChunk_getTopRainBlockPos;
//
//static BlockPos& LevelChunk_getTopRainBlockPos(LevelChunk* a1, ChunkBlockPos* a2, void* a3) {
//    BlockPos& pos = levelChunk_getTopRainBlockPos(a1, a2, a3);
//
//    if (pos.x >= 0 && pos.z >= 0 && pos.x < WIDTH && pos.z < WIDTH) {
//        ChunkBlockPos chunk_pos(pos.below());
//        const Block* block = a1->getBlock(chunk_pos);
//
//        if (block != nullptr && g_clientInstance != nullptr) {
//            Color color;
//            block->getMapColor(&color, g_clientInstance->getRegion(), &pos);
//            g_colormap[pos.x][pos.z] = color;
//        }
//    }
//
//    return pos;
//
//}

typedef Color* (__thiscall* _block_getMapColor)(void* self, Color* a2, BlockSource* a3, const BlockPos* a4);
_block_getMapColor block_getMapColor;

static Color* Block_getMapColor(void* self, Color* result, BlockSource* a3, const BlockPos* pos) {
    block_getMapColor(self, result, a3, pos);

    if (pos->x >= 0 && pos->z >= 0 && pos->x < WIDTH && pos->z < WIDTH) {
        g_colormap[pos->x][pos->z] = *result;
        LocalPlayer* player = g_clientInstance->getLocalPlayer();
        Dimension* dimension = player->getDimension();
        const BlockSource* bs = dimension->getBlockSourceFromMainChunkSource();
        Log::Info("0x{:x} 0x{:x} 0x{:x}\n", reinterpret_cast<uintptr_t>(a3), reinterpret_cast<uintptr_t>(bs), reinterpret_cast<uintptr_t>(g_clientInstance->getRegion()));
    }

    return result;
}

typedef int64_t(__thiscall* _clientInstance_onStartJoinGame)(ClientInstance*, int64_t, int64_t, uint64_t);
_clientInstance_onStartJoinGame clientInstance_onStartJoinGame;

static int64_t ClientInstance_onStartJoinGame(ClientInstance* self, int64_t a2, int64_t a3, uint64_t a4) {
    g_clientInstance = self;
    Log::Info("Client Instance: 0x{:x}\n", reinterpret_cast<uintptr_t>(g_clientInstance));
    return clientInstance_onStartJoinGame(self, a2, a3, a4);
}

HookManager hookManager;

extern "C" __declspec(dllexport) void Initialize() {
    hookManager.CreateHook(
        SlideAddress(0x037C3A0),
        &ClientInstance_onStartJoinGame, &clientInstance_onStartJoinGame
    );

    /*try {
        hooks.CreateHook(
            SlideAddress(0x037C3A0),
            ClientInstance_onStartJoinGame, clientInstance_onStartJoinGame
        );
    }
    catch (std::exception e) {
        Log::Info("Ex!! {}\n", e.what());
    }*/
    

    //CreateHook(
    //    SlideAddress(0x037C3A0),
    //    ClientInstance_onStartJoinGame, clientInstance_onStartJoinGame
    //);

    ///*CreateHook(
    //    SlideAddress(0x3914AC0),
    //    LevelChunk_getTopRainBlockPos, levelChunk_getTopRainBlockPos
    //);*/

    //CreateHook(
    //    SlideAddress(0x34B46D0),
    //    Block_getMapColor, block_getMapColor
    //);
}

void WriteMap() {
    const int maxColorValue = 255;

    std::ofstream outFile("C:/Users/blake/AppData/Local/Packages/Microsoft.MinecraftUWP_8wekyb3d8bbwe/LocalState/games/com.mojang/out.ppm");

    if (!outFile.is_open()) {
        std::cerr << "Error opening the file." << std::endl;
        return;
    }

    outFile << "P3\n" << WIDTH << " " << WIDTH << "\n" << maxColorValue << "\n";

    for (int y = 0; y < WIDTH; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int red = static_cast<unsigned char>(g_colormap[x][y].r * 255);
            int green = static_cast<unsigned char>(g_colormap[x][y].g * 255);
            int blue = static_cast<unsigned char>(g_colormap[x][y].b * 255);

            outFile << red << " " << green << " " << blue << " ";
        }
        outFile << "\n";
    }

    outFile.close();
}

extern "C" __declspec(dllexport) void OnTick() {
    /*if (GetAsyncKeyState(VK_NUMPAD1)) {
        WriteMap();
    }*/
}

extern "C" __declspec(dllexport) void Shutdown() {
    //hooks.DestroyAllHooks();
}

//__int64 __fastcall Actor::getDimensionBlockSource(Actor *__hidden this)
// called in ClientInstance getRegion func

// ClientInstance::init - 0x037AA40
// ClientInstance::getRegion - sub_140380AA0 

// public: class mce::Color Block::getMapColor(class BlockSource &, class BlockPos const &)const
// sub_1434B46D0