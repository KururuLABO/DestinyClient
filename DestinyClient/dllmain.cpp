// dllmain.cpp : Defines the entry point for the DLL application.
#include "global.h"
#include <clocale>

DWORD WINAPI MainProccess(PVOID) {
#ifdef  _DEBUG
  /*  AllocConsole();
    freopen("conin$", "r", stdin);
    freopen("conout$", "w", stdout);
    freopen("conout$", "w", stderr);
    printf("Debugging Window:\n");*/
#endif

    auto locale = setlocale(LC_ALL, NULL);
    Log("[MapleLog] LC_ALL: %s", locale);

    if (!DestinyHookSocket()) {
        Log("[MapleLog] Failed Hook Socket");
    }

    if (!DestinyHookWindows()) {
        Log("[MapleLog] Failed Hook Windows");
    }

    if (!DestinyHookCanvas()) {
        Log("[MapleLog] Failed Hook Canvas");
    }

    if (!DestinyHookMapleApi()) {
        Log("[MapleLog] Failed Hook MapleAPI");
    }

    /*if (!DestinyPacketLogger()) {
        Log("[MapleLog] Failed Hook PacketLogger");
    }*/

    if (!DestinyHookNMCO()) {
        Log("[MapleLog] Failed Hook NMCO");
    }

    Log("[MapleLog] Hooked");



    return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
//    switch (ul_reason_for_call)
//    {
//        case DLL_PROCESS_ATTACH:
//            
//            break;
//        case DLL_THREAD_ATTACH:
//        case DLL_THREAD_DETACH:
//        case DLL_PROCESS_DETACH:
//            break;
//    }
    DisableThreadLibraryCalls(hinstDLL);

    if (fdwReason == DLL_PROCESS_ATTACH) {
        //setlocale(LC_ALL, "en_US.utf8");

        
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&MainProccess, NULL, NULL, NULL);
    }

    return TRUE;
}

