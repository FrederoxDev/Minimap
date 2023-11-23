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
#include "MinHook.h"

static ClientInstance* g_clientInstance = nullptr;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    return TRUE;
}

typedef Color* (__thiscall* _block_getMapColor)(void* self, Color* a2, BlockSource* a3, const BlockPos* a4);
_block_getMapColor block_getMapColor;

static Color* Block_getMapColor(void* self, Color* a2, BlockSource* a3, const BlockPos* a4) {
    Color* col = block_getMapColor(self, a2, a3, a4);
    Log::Info("Sup {} {} {} {}\n", col->r * 255, col->g * 255, col->b * 255, col->a * 255);
    return col;
}

HookManager hookManager;

extern "C" __declspec(dllexport) void Initialize() {
    MH_Initialize();

    hookManager.CreateHook(SlideAddress(0x34B46D0), &Block_getMapColor, reinterpret_cast<void**>(&block_getMapColor));
}

extern "C" __declspec(dllexport) void Shutdown() {
    hookManager.Shutdown();
}