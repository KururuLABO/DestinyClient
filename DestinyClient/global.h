#pragma once
#include <WinSock2.h>
#include <WS2spi.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <stdio.h>
#include "detours.h"
#include <wtypes.h>
#include <vector>
#include <string>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "detours.lib")
#pragma warning( disable : 4996)

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files

#define OPT_APPNAME			"Destiny Story"
#define OPT_ADDR_HOSTNAME	"127.0.0.1"
#define OPT_ADDR_SEARCH		"8.31.99."
#define OPT_ADDR_NEXON		"8.31.99.141"
#define OPT_PORT_LOW		8484
#define OPT_PORT_HIGH		8989


#define relative_address(frm, to) (int)(((int)to - (int)frm) - 5)
#define JMP		0xE9
#define NOP		0x90
#define RET		0xC3

//Type
template <typename T>
struct ZXString
{
	T* m_pStr;
};

//Tools
void Log(const char* format, ...);
void LogW(const wchar_t* format, ...);
DWORD GetFunctionAddress(LPCSTR lpModule, LPCSTR lpFunc);
BOOL SetHook(BOOL bInstall, PVOID* ppvTarget, PVOID pvDetour);
void PatchJmp(DWORD dwAddress, DWORD dwDest);
void PatchNop(DWORD dwAddress, DWORD dwCount);



//DestinySocket
bool DestinyHookSocket();

//DestinyCanvas
bool DestinyHookCanvas();

//DestinyWindows
bool DestinyHookWindows();

//DestinyMapleAPI
bool DestinyHookMapleApi();


bool DestinyHookMapleHack();

bool DestinyPacketLogger();

bool DestinyHookNMCO();