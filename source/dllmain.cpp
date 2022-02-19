#include "stdafx.h"

bool DisableCRC, UnlockAllLanguages, UnlockLoyaltyItems;
uint8_t FrameInterval;

void Init()
{
    CIniReader iniReader("");
    DisableCRC = iniReader.ReadInteger("Main", "DisableCRC", 0) == 1;
    FrameInterval = iniReader.ReadInteger("Main", "FrameInterval", 16);
    UnlockAllLanguages = iniReader.ReadInteger("Main", "UnlockAllLanguages", 0) == 1;
    UnlockLoyaltyItems = iniReader.ReadInteger("Main", "UnlockLoyaltyItems", 0) == 1;

    if (DisableCRC) {
        injector::WriteMemory<WORD>(0x635618, 0x00EB, true); // jmp 0x63561A
    }

    if (FrameInterval != 16) {
        injector::WriteMemory<BYTE>(0x4D919B, FrameInterval, true);
        injector::WriteMemory<BYTE>(0x4D919F, FrameInterval, true);
    }

    if (UnlockAllLanguages) {
        injector::WriteMemory<BYTE>(0x6239D5, 0xC, true); // cmp edi, 0xC
        injector::MakeNOP(0x6239DC, 27, true);
        injector::WriteMemory<WORD>(0x6239F7, 0x01B3, true); // mov bl, 0x1
    }

    if (UnlockLoyaltyItems) {
        injector::WriteMemory<DWORD>(0x622D9B, 0, true);
        injector::WriteMemory<DWORD>(0x622DB4, 0, true);
        injector::WriteMemory<DWORD>(0x622DCB, 0, true);
        injector::WriteMemory<DWORD>(0x622DE2, 0, true);
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        Init();
    }
    return TRUE;
}
