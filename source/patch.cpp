#include "stdafx.h"

bool DisableLetterbox, LandmineLodFix, UnlockAllLanguages, UnlockLoyaltyItems;
uint8_t FrameInterval;

bool Borderless;

void Init()
{
    CIniReader iniReader("");

    FrameInterval = iniReader.ReadInteger("Main", "FrameInterval", 16);
    DisableLetterbox = iniReader.ReadInteger("Main", "DisableLetterbox", 0) == 1;
    LandmineLodFix = iniReader.ReadInteger("Main", "LandmineLodFix", 0) == 1;
    UnlockAllLanguages = iniReader.ReadInteger("Main", "UnlockAllLanguages", 0) == 1;
    UnlockLoyaltyItems = iniReader.ReadInteger("Main", "UnlockLoyaltyItems", 0) == 1;
    Borderless = iniReader.ReadInteger("Window", "Borderless", 0) == 1;

    // Main
    if (FrameInterval != 16) {
        injector::WriteMemory<BYTE>(0x4D919B, FrameInterval, true);
        injector::WriteMemory<BYTE>(0x4D919F, FrameInterval, true);
    }

    if (DisableLetterbox)
    {
        injector::MakeJMP(0x6F6E37, 0x6F6ED0, true);
    }

    if (LandmineLodFix)
    {
        injector::WriteMemory<BYTE>(0x57CC46, 0xEB, true);
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

    // Window
    if (Borderless)
    {
        injector::WriteMemory<BYTE>(0x795CAC, 0x8, true); // dwStyle
        injector::WriteMemory<BYTE>(0x795CBC, 0x8, true); // dwStyle
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
