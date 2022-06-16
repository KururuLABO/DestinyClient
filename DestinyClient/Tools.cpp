#include "global.h"

void Log(const char* format, ...) {

#ifdef  _DEBUG //Log only debugging mode!

	char buf[1024] = { 0 };

	va_list args;
	va_start(args, format);
	vsprintf_s(buf, format, args);

	OutputDebugStringA(buf);
	//OutputDebugStringA("\n");

	printf(buf);
	//printf("\n");

	va_end(args);
#endif
}

void LogW(const wchar_t* szFormat, ...)
{
#ifdef  _DEBUG //Log only debugging mode!
	wchar_t szMessage[1024] = { 0 };

	va_list args;
	va_start(args, szFormat);
	vswprintf_s(szMessage, szFormat, args);

	OutputDebugStringW(szMessage);
	//OutputDebugStringW(L"\n");

	va_end(args);
#endif
}



DWORD GetFunctionAddress(LPCSTR lpModule, LPCSTR lpFunc) {

	HMODULE library = LoadLibraryA(lpModule);

	if (!library) {
		return 0; //cannot load library
	}

	DWORD functionAddress = (DWORD)GetProcAddress(library, lpFunc);

	if (!functionAddress) {
		return 0; //not found function address!
	}

	Log("[MapleLog] [Tools:GetFunctionAddres] -> [0x%8X] %s:%s", functionAddress, lpModule, lpFunc);

	return functionAddress;

}

BOOL SetHook(BOOL bInstall, PVOID* ppvTarget, PVOID pvDetour)
{
	if (DetourTransactionBegin() != NO_ERROR)
	{
		return FALSE;
	}

	auto tid = GetCurrentThread();

	if (DetourUpdateThread(tid) == NO_ERROR)
	{
		auto func = bInstall ? DetourAttach : DetourDetach;

		if (func(ppvTarget, pvDetour) == NO_ERROR)
		{
			if (DetourTransactionCommit() == NO_ERROR)
			{
				return TRUE;
			}
		}
	}

	DetourTransactionAbort();
	return FALSE;
}

void PatchJmp(DWORD dwAddress, DWORD dwDest)
{

	*(BYTE*)dwAddress = JMP;
	*(DWORD*)(dwAddress + 1) = relative_address(dwAddress, dwDest);
}

void PatchNop(DWORD dwAddress, DWORD dwCount)
{
	for (int i = 0; i < dwCount; i++)
	{
		*(BYTE*)(dwAddress + i) = NOP;
	}
}

