#include "global.h"

bool Hook_CreateWindowExA()
{
	static auto _CreateWindowExA = decltype(&CreateWindowExA)(GetFunctionAddress("USER32", "CreateWindowExA"));

	decltype(&CreateWindowExA) Hook = [](DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) -> HWND
	{
		auto lpLocalWndName = lpWindowName;

		if (!strcmp(lpClassName, "StartUpDlgClass"))
		{
			return NULL;
		}
		else if (!strcmp(lpClassName, "NexonADBallon"))
		{
			return NULL;
		}
		else if (!strcmp(lpClassName, "MapleStoryClass"))
		{
			DestinyHookMapleHack();
			lpLocalWndName = OPT_APPNAME;
		}
		else {
			Log("[MapleLog] [USER32::CreateWindowExA] lpClassName: %s", lpClassName);
		}

		return _CreateWindowExA(dwExStyle, lpClassName, lpLocalWndName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
	};

	return SetHook(true, reinterpret_cast<void**>(&_CreateWindowExA), Hook);
}

bool Hook_CreateMutexA()
{
	static auto _CreateMutexA = decltype(&CreateMutexA)(GetFunctionAddress("KERNEL32", "CreateMutexA"));

	decltype(&CreateMutexA) Hook = [](LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCSTR lpName) -> HANDLE
	{
		if (lpName && !strcmp(lpName, "WvsClientMtx"))
		{
			return (HANDLE)0xBADF00D;
		}

		return _CreateMutexA(lpMutexAttributes, bInitialOwner, lpName);
	};

	return SetHook(true, reinterpret_cast<void**>(&_CreateMutexA), Hook);
}

bool Hook_WideCharToMultiByte()
{
	static auto _WideCharToMultiByte = decltype(&WideCharToMultiByte)(GetFunctionAddress("KERNEL32", "WideCharToMultiByte"));

	decltype(&WideCharToMultiByte) Hook = [](UINT CodePage, DWORD dwFlags, LPCWCH lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, PCCH lpDefaultChar, LPBOOL lpUsedDefaultChar) -> int
	{
		CodePage = 0x36A; //replace with thai code pages

		return _WideCharToMultiByte(CodePage, dwFlags, lpWideCharStr, cchWideChar, lpMultiByteStr, cbMultiByte, lpDefaultChar, lpUsedDefaultChar);
	};

	return SetHook(true, reinterpret_cast<void**>(&_WideCharToMultiByte), Hook);
}

bool Hook_MultiByteToWideChar()
{
	static auto _MultiByteToWideChar = decltype(&MultiByteToWideChar)(GetFunctionAddress("KERNEL32", "MultiByteToWideChar"));

	decltype(&MultiByteToWideChar) Hook = [](UINT CodePage, DWORD dwFlags, LPCCH lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar) -> int
	{
		CodePage = 0x36A; //replace with thai code pages

		return _MultiByteToWideChar(CodePage, dwFlags, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar);
	};

	return SetHook(true, reinterpret_cast<void**>(&_MultiByteToWideChar), Hook);
}

bool DestinyHookWindows()
{
	bool bResult = true;

	bResult &= Hook_CreateWindowExA();
	bResult &= Hook_CreateMutexA();
	bResult &= Hook_WideCharToMultiByte();
	bResult &= Hook_MultiByteToWideChar();
	//bResult &= Hook_GetExitCodeProcess();
	//bResult &= Hook_RaiseException();

	return bResult;
}
