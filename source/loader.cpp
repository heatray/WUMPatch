#include "stdafx.h"

const char* DataPath = "Data2";

DWORD XString__XString = 0x638101;
DWORD XString__RemoveInstance = 0x6380B8;
DWORD XString__FreeRep = 0x637DB4;
DWORD XomGetApp = 0x692DA7;

void __declspec(naked) InitOptionalPathsCodeCave()
{
    __asm {
        push    DataPath
        lea     ecx, dword ptr ss : [esp + 0x8]
        call    XString__XString
        mov     dword ptr ss : [esp + 0x10], 0x1
        call    XomGetApp
        mov     ecx, dword ptr ds : [eax]
        lea     edx, dword ptr ss : [esp + 0x4]
        push    edx
        push    eax
        mov     eax, dword ptr ds : [ecx + 0xC]
        call    eax
        mov     dword ptr ss : [esp + 0x10], esi
        lea     ecx, dword ptr ss : [esp + 0x4]
        call    XString__RemoveInstance
        mov     ecx, dword ptr ss : [esp + 0x4]
        add     ecx, 0xFFFFFFFA
        mov     edx, 0xFFFF
        add     word ptr ds : [ecx], dx
        movzx   eax, word ptr ds : [ecx]
        test    eax, eax
        jne     InitPathsEnd
        call    XString__FreeRep
    InitPathsEnd:
        mov     ecx, dword ptr ss : [esp + 0x8]
        pop     ecx
        mov     dword ptr fs : [0x0], ecx
        add     esp, 0x10
        ret
    }
}

void Init()
{
    // CRC
    injector::WriteMemory<WORD>(0x635618, 0x00EB, true); // jmp 0x63561A

    // Data Search Path
    injector::MakeJMP(0x4D75DF, InitOptionalPathsCodeCave, true);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        Init();
    }
    return TRUE;
}
