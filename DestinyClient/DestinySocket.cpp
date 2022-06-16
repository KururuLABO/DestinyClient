#include "global.h"

WSPPROC_TABLE g_ProcTable;

int WINAPI WSPGetPeerName_Hook(SOCKET s, struct sockaddr* name, LPINT namelen, LPINT lpErrno)
{
	int nRet = g_ProcTable.lpWSPGetPeerName(s, name, namelen, lpErrno);

	if (nRet == SOCKET_ERROR)
	{
#ifdef  _DEBUG
		Log("[MapleLog] [WSPGetPeerName] ErrorCode: %d", *lpErrno);
#endif
	}
	else
	{
		char szAddr[50];
		DWORD dwLen = 50;
		WSAAddressToStringA((sockaddr*)name, *namelen, NULL, szAddr, &dwLen);

		sockaddr_in* service = (sockaddr_in*)name;

		auto nPort = ntohs(service->sin_port);
		if (nPort >= OPT_PORT_LOW && nPort <= OPT_PORT_HIGH)
		{
			service->sin_addr.S_un.S_addr = inet_addr(OPT_ADDR_NEXON);
#ifdef  _DEBUG
			Log("[MapleLog] [WSPGetPeerName] Replaced: %s", OPT_ADDR_NEXON);
#endif
		}
		else
		{
#ifdef  _DEBUG
			Log("[MapleLog] [WSPGetPeerName] Original: %s", szAddr);
#endif
		}
	}

	return  nRet;
}

int WINAPI WSPConnect_Hook(SOCKET s, const struct sockaddr* name, int namelen, LPWSABUF lpCallerData, LPWSABUF lpCalleeData, LPQOS lpSQOS, LPQOS lpGQOS, LPINT lpErrno)
{
	char szAddr[50];
	DWORD dwLen = 50;
	WSAAddressToStringA((sockaddr*)name, namelen, NULL, szAddr, &dwLen);

	sockaddr_in* service = (sockaddr_in*)name;
	auto nPort = ntohs(service->sin_port);

	Log("[MapleLog] [WSPConnect] Maplestory connect to : %s", szAddr);


	if (strstr(szAddr, OPT_ADDR_SEARCH))
	//if (nPort >= OPT_PORT_LOW && nPort <= OPT_PORT_HIGH) //search by port
	{
		service->sin_addr.S_un.S_addr = inet_addr(OPT_ADDR_HOSTNAME);
#ifdef  _DEBUG
		Log("[MapleLog] [WSPConnect] Replaced: %s", OPT_ADDR_HOSTNAME);
#endif
	}
	else
	{
#ifdef  _DEBUG
		Log("[MapleLog] [WSPConnect] Original: %s", szAddr);
#endif
	}

	return g_ProcTable.lpWSPConnect(s, name, namelen, lpCallerData, lpCalleeData, lpSQOS, lpGQOS, lpErrno);
}

bool Hook_WSPStartup()
{
	static auto _WSPStartup = decltype(&WSPStartup)(GetFunctionAddress("MSWSOCK", "WSPStartup"));

	decltype(&WSPStartup) Hook = [](WORD wVersionRequested, LPWSPDATA lpWSPData, LPWSAPROTOCOL_INFOW lpProtocolInfo, WSPUPCALLTABLE UpcallTable, LPWSPPROC_TABLE lpProcTable) -> int
	{
		int ret = _WSPStartup(wVersionRequested, lpWSPData, lpProtocolInfo, UpcallTable, lpProcTable);
		g_ProcTable = *lpProcTable;

		lpProcTable->lpWSPConnect = WSPConnect_Hook;
		lpProcTable->lpWSPGetPeerName = WSPGetPeerName_Hook;

		return ret;
	};

	auto a = SetHook(true, reinterpret_cast<void**>(&_WSPStartup), Hook);
	return a;
}

bool DestinyHookSocket()
{
	bool bResult = true;

	bResult &= Hook_WSPStartup();

	return bResult;
}

