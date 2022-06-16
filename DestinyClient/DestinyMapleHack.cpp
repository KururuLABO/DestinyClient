#include "global.h"


bool RemoveNGS() {

	//v176
	*(BYTE*)(0x01960B00 + 0) = 0x33;
	*(BYTE*)(0x01960B00 + 1) = 0xC0;
	*(BYTE*)(0x01960B00 + 2) = 0xC3;

	////203.4
	////PatchRetZero(0x029EC300); //NGS Bypass
	//*(BYTE*)(0x029EC300 + 0) = 0x33;
	//*(BYTE*)(0x029EC300 + 1) = 0xC0;
	//*(BYTE*)(0x029EC300 + 2) = 0xC3;

	return true;
}

bool BypassMSCRC() {

	//176
	PatchJmp(0x019DD7AD, 0x019DD844);

	////203.4
	////PatchRetZero(0x029FA5D0); // CRC Bypass
	//*(BYTE*)(0x029FA5D0 + 0) = 0x33;
	//*(BYTE*)(0x029FA5D0 + 1) = 0xC0;
	//*(BYTE*)(0x029FA5D0 + 2) = 0xC3;

	return true;
}

bool RipOffLatinCheck() {

	#pragma region  v176
	*(BYTE*)(0x0193EDF8 + 0) = 0xFB; //CWndMan::TranslateMessage Enable Latin 0xFB like THMS
	*(BYTE*)(0x006F4325 + 1) = 0xFB; //CCtrlEdit::InsertString
	*(BYTE*)(0x00EC5787 + 1) = 0xFB; //CCurseProcess::IsValidCharacterName
	*(BYTE*)(0x00EC5A7D + 1) = 0xFB;
	*(BYTE*)(0x018C63FF + 1) = 0xFB;
	*(BYTE*)(0x01AD0C44 + 1) = 0xFB;
	*(BYTE*)(0x01AECA15 + 1) = 0xFB;

	//*(BYTE*)(0x00EC5F8A + 1) = 0xFB; //CCurseProcess::IsValidCharacterName
	#pragma endregion

	//#pragma region v203.4
	////83 ? 20 0F ? ? ? ? ? 83 ? 7F = CWndMan::TranslateMessage - Enable Latin 0xFB like THMS
	//*(BYTE*)(0x029AA0BF + 0) = 0xFB; //replace 0x7F -> 0xFB

	///*
	//	AOB : 3C E6 74 ? 3C F8  
	//	Description : Xref to below functions
	//	- CUser::OnChat,
	//	- CCurseProcess::InvalidCharacterName,
	//	- CField::OnGroupMessage
	//	- CField::OnGroupMessage_2
	//	- CUser::OnADBoard
	//*/

	////*(BYTE*)(0x00C4EC0D + 1) = 0xFB; //CCurseProcess::InvalidCharacterName
	//*(BYTE*)(0x0254C694 + 1) = 0xFB; //CUser::OnADBoard



	////3C 7A 7E - Other latin checks
	////*(BYTE*)(0x00770624 + 1) = 0xFB;
	////*(BYTE*)(0x0079E96A + 1) = 0xFB;
	////*(BYTE*)(0x0079EB40 + 1) = 0xFB;
	////*(BYTE*)(0x00C4EC0D + 1) = 0xFB;
	////*(BYTE*)(0x00C4EF08 + 1) = 0xFB;
	////*(BYTE*)(0x00C4F237 + 1) = 0xFB;
	////*(BYTE*)(0x00C51EA6 + 1) = 0xFB;
	////*(BYTE*)(0x0121B17A + 1) = 0xFB;
	////*(BYTE*)(0x0121B193 + 1) = 0xFB;
	////*(BYTE*)(0x0285F5C7 + 1) = 0xFB;
	////*(BYTE*)(0x02B6B916 + 1) = 0xFB;
	////*(BYTE*)(0x02B85D63 + 1) = 0xFB;




	////*(BYTE*)(0x00EC5F8A + 1) = 0xFB; //CCurseProcess::IsValidCharacterName
	//#pragma endregion

	return true;
}

bool DisablePacketStringCheck() {

	#pragma region v176

	PatchNop(0x01547C2C, 7); //CUIStatusBar::OnKey
	PatchNop(0x016FD78D, 9); //CUser::OnChat

	#pragma endregion
	//v203 = 74 0C 66 C7 06 20 20 B8 02 00 00 00 EB 1F 8A 06
	/*PatchNop(0x0135A5AF, 7);
	PatchNop(0x0135AB08, 7);
	PatchNop(0x0254B808, 7);
	PatchNop(0x0254C680, 7);*/

	return true;
}


bool Hook_IsValidCharacterName()
{
	typedef int(__fastcall* CCurseProcess__IsValidCharacterName_t)(void* a1, void* a2, ZXString<char>* sCharacterName, void* a4);
	static auto CCurseProcess__IsValidCharacterName = reinterpret_cast<CCurseProcess__IsValidCharacterName_t>(0x00EC5EF0); //v176
	//static auto CCurseProcess__IsValidCharacterName = reinterpret_cast<CCurseProcess__IsValidCharacterName_t>(0x00C4EB70); //v203.4

	CCurseProcess__IsValidCharacterName_t Hook = [](void* a1, void* a2, ZXString<char>* sCharacterName, void* a4) -> int
	{
		auto validName = CCurseProcess__IsValidCharacterName(a1, a2, sCharacterName, a4);
		if (!validName) {
			auto strLength = *(sCharacterName->m_pStr - 1);
			auto strIndex = 0;
			do {
				auto chr = sCharacterName->m_pStr[strIndex];

				if (chr < 0x61 || chr > 0xFB) {
					validName = true;
				}
				else {
					validName = false;
					break;
				}

				++strIndex;
			} while (strIndex < strLength);
		}

		return validName;
	};
	return SetHook(true, reinterpret_cast<void**>(&CCurseProcess__IsValidCharacterName), Hook);
}

typedef void(__fastcall* ZXString_char__Assign_t)(void* pThis, void* edx, const char* s, int n);
ZXString_char__Assign_t ZXString_char__Assign = reinterpret_cast<ZXString_char__Assign_t>(0x0047EDD0); //176
//ZXString_char__Assign_t ZXString_char__Assign = reinterpret_cast<ZXString_char__Assign_t>(0x00823160); //203.4

bool Hook_StringPool__GetString() {

	typedef ZXString<char>* (__fastcall* StringPool__GetString_t)(void* ecx, void* edx, ZXString<char>* result, unsigned int nIdx, char formal);
	static auto StringPool__GetString = reinterpret_cast<StringPool__GetString_t>(0x00EC3BF0); //176
	//static auto StringPool__GetString = reinterpret_cast<StringPool__GetString_t>(0x00C4C980); //203.4

	StringPool__GetString_t Hook = [](void* ecx, void* edx, ZXString<char>* result, unsigned int nIdx, char formal) -> ZXString<char>*
	{
		StringPool__GetString(ecx, edx, result, nIdx, formal);

		if (0 == strcmp(result->m_pStr, "Arial") || 0 == strcmp(result->m_pStr, "Arial Narrow"))
		{
			const char* szFontName = "Leelawadee";
#ifdef  _DEBUG
			Log("[MapleLog] [StringPool::GetString-%d] %s -> %s", nIdx, result->m_pStr, szFontName);
#endif

			result->m_pStr = 0;
			ZXString_char__Assign(result, edx, szFontName, -1);
		}

		return result;
	};
	return SetHook(true, reinterpret_cast<void**>(&StringPool__GetString), Hook);
}

bool DestinyHookMapleHack()
{
	bool bResult = true;

	bResult &= RemoveNGS();
	bResult &= BypassMSCRC();
	bResult &= RipOffLatinCheck();
	bResult &= DisablePacketStringCheck();
	

	bResult &= Hook_IsValidCharacterName();
	bResult &= Hook_StringPool__GetString();

	return bResult;
}
