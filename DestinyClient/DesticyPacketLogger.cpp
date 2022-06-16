#include "global.h"
#include <string>
#include <sstream>
#include <iomanip>

#include <stdio.h>
#include <intrin.h>

#pragma intrinsic(_ReturnAddress)

//template<typename T>
//typename std::enable_if<std::is_integral<T>::value, std::string>::type
//toHex(const T& value) {
//	std::stringstream convertingStream;
//	convertingStream << "0x" << std::setfill('00') << std::setw(sizeof(T) * 2) << std::hex << value;
//	return convertingStream.str();
//}

//std::string hexStr(std::vector<unsigned char> data)
//{
//	std::stringstream ss;
//	ss << std::hex;
//
//	for (auto& value : data) {
//		ss << std::setfill('0') << std::setw(2) << value;
//	}
//
//	BYTE array[100];
//	char hexstr[201];
//	int i;
//	for (auto& value : data) {
//		sprintf(hexstr + i * 2, "%02x", data);
//		i++;
//	}
//	hexstr[i * 2] = 0;
//	
//	return std::string(hexstr);
//}

std::string hexStr(const std::vector<uint8_t>& input)
{
	static const char characters[] = "0123456789ABCDEF";

	// Zeroes out the buffer unnecessarily, can't be avoided for std::string.
	std::string ret(input.size() * 2, 0);

	// Hack... Against the rules but avoids copying the whole buffer.
	char* buf = const_cast<char*>(ret.data());

	for (const auto& oneInputByte : input)
	{
		*buf++ = characters[oneInputByte >> 4];
		*buf++ = characters[oneInputByte & 0x0F];
	}
	return ret;
}


std::vector<unsigned char> longToBytes(long value)
{
	std::vector<unsigned char> result;
	result.push_back(value >> 56);
	result.push_back(value >> 48);
	result.push_back(value >> 40);
	result.push_back(value >> 32);
	result.push_back(value >> 24);
	result.push_back(value >> 16);
	result.push_back(value >> 8);
	result.push_back(value);
	return result;
}

std::vector<unsigned char> intToBytes(int value)
{
	std::vector<unsigned char> result;
	result.push_back(value >> 24);
	result.push_back(value >> 16);
	result.push_back(value >> 8);
	result.push_back(value);
	return result;
}

std::vector<unsigned char> shortToBytes(short value)
{
	std::vector<unsigned char> result;
	result.push_back(value >> 8);
	result.push_back(value);
	return result;
}

std::vector<unsigned char> charToBytes(char value)
{
	std::vector<unsigned char> result;
	result.push_back(value);
	return result;
}

const wchar_t* GetWC(const char* c)
{
	const size_t cSize = strlen(c) + 1;
	wchar_t* wc = new wchar_t[cSize];
	mbstowcs(wc, c, cSize);

	return wc;
}

std::string currentPacket;

bool Hook_COutPacket_COutPacket() {

	typedef void(__fastcall* COutPacket_COutPacket_t)(void* ecx, void* edx, unsigned short nType);
	static auto COutPacket_COutPacket = reinterpret_cast<COutPacket_COutPacket_t>(0x00CEF6E0);

	COutPacket_COutPacket_t Hook = [](void* ecx, void* edx, unsigned short nType) -> void
	{

		//Log("CoutPacket[OP 0x%x] was called from %p", nType, _ReturnAddress());
		currentPacket = "";
		currentPacket += "OP [";
		currentPacket += hexStr(shortToBytes(nType));
		currentPacket += "]";
		
		COutPacket_COutPacket(ecx, edx, nType);

	};

	return SetHook(true, reinterpret_cast<void**>(&COutPacket_COutPacket), Hook);
}

bool Hook_COutPacket_Encode1() {

	typedef void(__fastcall* COutPacket_Encode1_t)(void* ecx, void* edx, char n);
	static auto COutPacket_Encode1 = reinterpret_cast<COutPacket_Encode1_t>(0x004C6A20);

	COutPacket_Encode1_t Hook = [](void* ecx, void* edx, char n) -> void
	{
		currentPacket += "[";
		currentPacket += hexStr(charToBytes(n)); 
		currentPacket += "]";

		COutPacket_Encode1(ecx, edx, n);

		//currentPacket.append("[" + toHex(n) + "]");
		//Log("COutPacket_Encode1 : %d", n);

	};

	return SetHook(true, reinterpret_cast<void**>(&COutPacket_Encode1), Hook);
}

bool Hook_COutPacket_Encode2() {

	typedef void(__fastcall* COutPacket_Encode2_t)(void* ecx, void* edx, unsigned __int16 n);
	static auto COutPacket_Encode2 = reinterpret_cast<COutPacket_Encode2_t>(0x004C6A70);

	COutPacket_Encode2_t Hook = [](void* ecx, void* edx, unsigned __int16 n) -> void
	{
		currentPacket += "[";
		currentPacket += hexStr(shortToBytes(n));
		currentPacket += "]";
		COutPacket_Encode2(ecx, edx, n);

		//currentPacket.append("[" + toHex(n) + "]");
		//Log("COutPacket_Encode2 : %d", n);

	};

	return SetHook(true, reinterpret_cast<void**>(&COutPacket_Encode2), Hook);
}

bool Hook_COutPacket_Encode4() {

	typedef void(__fastcall* COutPacket_Encode4_t)(void* ecx, void* edx, unsigned int n);
	static auto COutPacket_Encode4 = reinterpret_cast<COutPacket_Encode4_t>(0x004BEDD0);

	COutPacket_Encode4_t Hook = [](void* ecx, void* edx, unsigned int n) -> void
	{
		currentPacket += "[";
		currentPacket += hexStr(intToBytes(n));
		currentPacket += "]";

		COutPacket_Encode4(ecx, edx, n);

		//currentPacket.append("[" + toHex(n) + "]");
		//Log("COutPacket_Encode4 : %d", n);

	};

	return SetHook(true, reinterpret_cast<void**>(&COutPacket_Encode4), Hook);
}

bool Hook_COutPacket_Encode8() {

	typedef void(__fastcall* COutPacket_Encode8_t)(void* ecx, void* edx, __int64 n);
	static auto COutPacket_Encode8 = reinterpret_cast<COutPacket_Encode8_t>(0x0056FF80);

	COutPacket_Encode8_t Hook = [](void* ecx, void* edx, __int64 n) -> void
	{
		currentPacket += "[";
		currentPacket += hexStr(longToBytes(n));
		currentPacket += "]";

		COutPacket_Encode8(ecx, edx, n);

		//currentPacket.append("[" + toHex(n) + "]");
		//Log("COutPacket_Encode8 : %d", n);

	};

	return SetHook(true, reinterpret_cast<void**>(&COutPacket_Encode8), Hook);
}

bool Hook_COutPacket_EncodeBuffer() {

	typedef void(__fastcall* COutPacket_EncodeBuffer_t)(void* ecx, void* edx, const void *p, unsigned int uSize);
	static auto COutPacket_EncodeBuffer = reinterpret_cast<COutPacket_EncodeBuffer_t>(0x005B65F0);

	COutPacket_EncodeBuffer_t Hook = [](void* ecx, void* edx, const void* p, unsigned int uSize) -> void
	{
		COutPacket_EncodeBuffer(ecx, edx, p, uSize);

		Log("COutPacket_EncodeBuffer");

	};

	return SetHook(true, reinterpret_cast<void**>(&COutPacket_EncodeBuffer), Hook);
}

bool Hook_COutPacket_EncodeStr() {

	typedef void(__fastcall* CoutPacket_EndcodeStr_t)(void* ecx, void* edx, ZXString<char> s);
	static auto CoutPacket_EndcodeStr = reinterpret_cast<CoutPacket_EndcodeStr_t>(0x005713B0);

	CoutPacket_EndcodeStr_t Hook = [](void* ecx, void* edx, ZXString<char> s) -> void
	{
		//std::string str(s->m_pStr);
		std::wstring ws(GetWC(s.m_pStr));
		std::string str(ws.begin(), ws.end());

		currentPacket += "[";
		currentPacket += str;
		currentPacket += "]";
		//Log("COutPacket_EncodeStr : %s", s);

		CoutPacket_EndcodeStr(ecx, edx, s);

		
	};

	return SetHook(true, reinterpret_cast<void**>(&CoutPacket_EndcodeStr), Hook);
}


bool Hook_COutPacket_MakeBufferlist() {

	typedef void(__fastcall* COutPacket_MakeBufferList_t)(void* ecx, void* edx, void* l, unsigned __int16 uSeqbase, unsigned int* puSeqKey, int bEnc, unsigned int dwKey);
	static auto COutPacket_MakeBufferList = reinterpret_cast<COutPacket_MakeBufferList_t>(0x00CEF8E0);

	COutPacket_MakeBufferList_t Hook = [](void* ecx, void* edx, void* l, unsigned __int16 uSeqbase, unsigned int* puSeqKey, int bEnc, unsigned int dwKey) -> void
	{
		Log("%s", currentPacket);

		COutPacket_MakeBufferList(ecx, edx, l, uSeqbase, puSeqKey, bEnc, dwKey);
		
	};

	return SetHook(true, reinterpret_cast<void**>(&COutPacket_MakeBufferList), Hook);
}

bool Hook_CInPacket_CInPacket() {

	typedef void(__fastcall* CInPacket_CInPacket_t)(void* ecx, void* edx, unsigned short nType);
	static auto CInPacket_CInPacket = reinterpret_cast<CInPacket_CInPacket_t>(0x00CEF6E0);

	CInPacket_CInPacket_t Hook = [](void* ecx, void* edx, unsigned short nType) -> void
	{

		Log("CoutPacket[OP 0x%x] was called from %p", nType, _ReturnAddress());

		CInPacket_CInPacket(ecx, edx, nType);

	};

	return SetHook(true, reinterpret_cast<void**>(&CInPacket_CInPacket), Hook);
}



bool DestinyPacketLogger()
{
	bool bResult = true;

	/*bResult &= Hook_COutPacket_COutPacket();
	bResult &= Hook_COutPacket_Encode1();
	bResult &= Hook_COutPacket_Encode2();
	bResult &= Hook_COutPacket_Encode4();
	bResult &= Hook_COutPacket_Encode8();
	bResult &= Hook_COutPacket_EncodeBuffer();
	bResult &= Hook_COutPacket_EncodeStr();
	bResult &= Hook_COutPacket_MakeBufferlist();*/
	//bResult &= Hook_CInPacket_CInPacket();


	return bResult;
}
