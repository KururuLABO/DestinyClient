#include "global.h"

bool Hook_WriteStageLogA()
{
	typedef int(__cdecl* pWriteStageLogA)(int nIdx, ZXString<char> szMessage);
	static auto _WriteStageLogA = (pWriteStageLogA)(GetFunctionAddress("nxgsm", "WriteStageLogA"));

	pWriteStageLogA Hook = [](int nIdx, ZXString<char> szMessage) -> int
	{
		Log("[MapleLog] [nxgsm::WriteStageLogA] %s", szMessage);
		return 0;
	};

	return SetHook(true, reinterpret_cast<void**>(&_WriteStageLogA), Hook);
}

bool Hook_WriteErrorLogA()
{
	typedef int(__cdecl* pWriteErrorLogA)(int nIdx, ZXString<char> szMessage);
	static auto _WriteErrorLogA = (pWriteErrorLogA)(GetFunctionAddress("nxgsm", "WriteErrorLogA"));

	pWriteErrorLogA Hook = [](int nIdx, ZXString<char> szMessage) -> int
	{
		Log("[MapleLog] [nxgsm::WriteErrorLogA] %s", szMessage);
		return 0;
	};

	return SetHook(true, reinterpret_cast<void**>(&_WriteErrorLogA), Hook);
}

bool DestinyHookMapleApi()
{
	bool bResult = true;

	//bResult &= Hook_WriteStageLogA();
	//bResult &= Hook_WriteErrorLogA();

	return bResult;
}
