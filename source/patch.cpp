#include "stdafx.h"

uint8_t FrameInterval;

bool HudAspectRatioFix, DisableLetterbox, XSceneCameraFix, LandmineLodFix, \
    UnlockAllLanguages, UnlockLoyaltyItems;
int Width = 800;
int Height = 600;
float AspectRatioX = 4.0f / 3.0f;
float AspectRatioY = 3.0f / 4.0f;
float ApertureValue = 12.7f;
float WXFE_ZBuffer_Scale_XY = 2000.0f;

bool Borderless;

DWORD* WormsXApp__c_pInstance = (DWORD*)0x9795A4;
DWORD SetFromSceneCameraCodeCaveExit = 0x6E1F83;

void __declspec(naked) AspectRatioCodeCave()
{
    __asm {
        mov     ecx, dword ptr ds:[0x9795A4]
        mov     ecx, [ecx+0x28]
        mov     Width, ecx
        mov     ecx, dword ptr ds:[0x9795A4]
        mov     ecx, [ecx+0x2C]
        mov     Height, ecx
        fild    dword ptr ds:[Height]
        fild    dword ptr ds:[Width]
        fdivp   st(1), st(0)
        fld     dword ptr ds:[AspectRatioY]
        fcomip  st(0), st(1)
        fstp    st(0)
        jne     Hor
        jmp     Exit

    Hor:
        jb      Vert
        fild    dword ptr ds:[Width]
        fild    dword ptr ds:[Height]
        fdivp   st(1), st(0)
        fstp    dword ptr ds:[AspectRatioX]
        jmp     Exit

    Vert:
        fild    dword ptr ds:[Height]
        fild    dword ptr ds:[Width]
        fdivp   st(1), st(0)
        fstp    dword ptr ds:[AspectRatioY]

    Exit:
        retn    8
    }
}

void __declspec(naked) SetFromSceneCameraCodeCave()
{
    // Frustum.X = 25.4/2 * Aperture.Y * W/H
    // Frustum.Y = 25.4/2 * Aperture.X * H/W
    __asm {
        fld     dword ptr ds:[ApertureValue]
        fld     dword ptr ds:[AspectRatioX]
        fmul    dword ptr ds:[eax+0x30]
        fmul    st(0), st(1)
        fstp    dword ptr ss:[ebp+0xC]
        fld     dword ptr ds:[AspectRatioY]
        fmul    dword ptr ds:[eax+0x2C]
        fmul    st(0), st(1)
        fstp    dword ptr ss:[ebp-4]
        fstp    st(0)
        push    edi
        mov     ecx, esi
        mov     byte ptr ds:[esi+0x11C], 0
        jmp     SetFromSceneCameraCodeCaveExit
    }
}

void Init()
{
    CIniReader iniReader("");

    FrameInterval = iniReader.ReadInteger("Main", "FrameInterval", 16);
    HudAspectRatioFix = iniReader.ReadInteger("Main", "HudAspectRatioFix", 0) == 1;
    DisableLetterbox = iniReader.ReadInteger("Main", "DisableLetterbox", 0) == 1;
    XSceneCameraFix = iniReader.ReadInteger("Main", "XSceneCameraFix", 0) == 1;
    LandmineLodFix = iniReader.ReadInteger("Main", "LandmineLodFix", 0) == 1;
    UnlockAllLanguages = iniReader.ReadInteger("Main", "UnlockAllLanguages", 0) == 1;
    UnlockLoyaltyItems = iniReader.ReadInteger("Main", "UnlockLoyaltyItems", 0) == 1;
    Borderless = iniReader.ReadInteger("Window", "Borderless", 0) == 1;

    // Main

    if (FrameInterval != 16) {
        injector::WriteMemory<BYTE>(0x4D919B, FrameInterval, true);
        injector::WriteMemory<BYTE>(0x4D919F, FrameInterval, true);
    }

    if (HudAspectRatioFix)
    {
        injector::WriteMemory<BYTE>(0x4D4D22, 0xEB, true); // jmp
    }

    if (DisableLetterbox)
    {
        // XOpenGLRenderManager
        injector::MakeJMP(0x6F6E37, 0x6F6ED0);

        // FrontEndService
        injector::WriteMemory<FLOAT*>(0x7298B1, &WXFE_ZBuffer_Scale_XY, true);
    }

    if (XSceneCameraFix)
    {
        // WormsXApp::ParseInitCommands
        injector::MakeJMP(0x510FA7, AspectRatioCodeCave);

        // XCamera::SetFromSceneCamera
        injector::MakeJMP(0x6E1F59, SetFromSceneCameraCodeCave);
    }

    if (LandmineLodFix)
    {
        injector::WriteMemory<BYTE>(0x57CC46, 0xEB, true);
    }

    if (UnlockAllLanguages) {
        injector::WriteMemory<BYTE>(0x6239D5, 0xC, true); // cmp edi, 0xC
        injector::MakeNOP(0x6239DC, 27);
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
