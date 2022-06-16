#include "global.h";


//std::vector<int> hangAboveCharacter = { 0xd1, 0xd4, 0xd5, 0xd6, 0xd7, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee };
//std::vector<int> hangBelowCharacter = { 0xd8, 0xd9, 0xda };
//std::vector<int> handRightAndTop = { 0xd3 };

std::vector<int> hangAboveCharacter = { 0xe31, 0xe34, 0xe35, 0xe36, 0xe37, 0xe47, 0xe48, 0xe49, 0xe4a, 0xe4b, 0xe4c, 0xe4d, 0xe4e };
std::vector<int> hangBelowCharacter = { 0xe38, 0xe39, 0xe3a };
std::vector<int> handRightAndTop = { 0xe33 };

std::vector<int> shiftCharacters = { };

DWORD hOrgCanvasBase = (DWORD)LoadLibraryA("Canvas.dll");
const wchar_t* currentDrawText = L"";
int currentDrawTextIndex = 0;
int textLength = 0;
WCHAR currentDrawFontCharacter = 0;


template <class N>
bool findInArray(int number, std::vector<N> vector) {
    // Finds if the number is in the array
    return std::find(std::begin(vector), std::end(vector), number) != std::end(vector);
}

#pragma region Calculate font position


bool hookCFontDrawTextA() {

    typedef void(__fastcall* CFont__DrawTextA_t)(void* ecx, void* edx, void* pCanvas, int x, int y, unsigned __int16* sText, int nAlpha, int nTabOrg);

    //75 12 6A 00 6A 20
    static auto CFont__DrawTextA = reinterpret_cast<CFont__DrawTextA_t>(hOrgCanvasBase + 0xED20); //v176
    //static auto CFont__DrawTextA = reinterpret_cast<CFont__DrawTextA_t>(hOrgCanvasBase + 0xC600); //v203.4

    CFont__DrawTextA_t Hook = [](void* ecx, void* edx, void* pCanvas, int x, int y, unsigned __int16* sText, int nAlpha, int nTabOrg) -> void {
       
        if (sText) {
            //LogW(L"---------------------------------------------------");
            LogW(L"[MapleLog] [CFont::DrawTextA] x : %u, y: %u, sText : %ls", x, y, sText);
            //Log("[MapleLog] [CFont::DrawTextA]");

            currentDrawText = (wchar_t*)sText;
            textLength = wcslen(currentDrawText);
            currentDrawTextIndex = 0;
        }

        CFont__DrawTextA(ecx, edx, pCanvas, x, y, sText, nAlpha, nTabOrg);

        if (sText) {
            currentDrawText = L""; //Reset
            textLength = 0;
            currentDrawTextIndex = 0;
        }

    };
    return SetHook(true, reinterpret_cast<void**>(&CFont__DrawTextA), Hook);
}


bool hookCFontGetFont() {

    typedef int(__fastcall* CFontGetFont_t)(void* ecx, void* edx, unsigned __int16 c, tagRECT* prc);

    //xref from CFont__Drawfont_t
    static auto CFont__GetFont = reinterpret_cast<CFontGetFont_t>(hOrgCanvasBase + 0xEC00); //v176
    //static auto CFont__GetFont = reinterpret_cast<CFontGetFont_t>(hOrgCanvasBase + 0xD840); //v203.4

    CFontGetFont_t Hook = [](void* ecx, void* edx, unsigned __int16 c, tagRECT* prc) -> int {
        //Log("[MapleLog] [CFont::GetFont]");

        auto ret = CFont__GetFont(ecx, edx, c, prc);

        if (currentDrawText && textLength > 0) { //check for drawble font only :/ (in v203 they get all font in background too)
            int nextPosition = currentDrawTextIndex + 1;
            wchar_t currentCharacter = currentDrawText[currentDrawTextIndex];
            wchar_t nextCharacter = currentDrawText[nextPosition];

            if (nextCharacter && findInArray(nextCharacter, shiftCharacters)) {
                ret = 0; //remove width for special characters
            }

            //Log("[MapleLog] [CFont::GetFont] [%u]currentCharacter = %c,  nextPosition :%u, nextCharacter = %c, nextCharacterCode : %u, textLength : %u, 0x%x, ret : %u", c, currentCharacter, nextPosition, nextCharacter, (int)nextCharacter, textLength, c, ret);
            //Log("[MapleLog] [CFont::GetFont] [%u]currentCharacter = %c,  nextPosition :%u, nextCharacter = %c, nextCharacterCode : %u, textLength : %u, 0x%x, ret : %u, currentText : %ls", c, currentCharacter, nextPosition, nextCharacter, (int)nextCharacter, textLength, c, ret, currentDrawText);

            currentDrawTextIndex = currentDrawTextIndex + 1;
        }

        return ret;

    };
    return SetHook(true, reinterpret_cast<void**>(&CFont__GetFont), Hook);
}

#pragma endregion

#pragma region Draw and replace pixel

bool hookCFontDrawFont() {

    typedef int(__fastcall* CFont__Drawfont_t)(void* ecx, void* edx, HBITMAP* hBitmap, tagBITMAP* bm, tagPOINT pt, WCHAR c, unsigned int* puPrintWidth, unsigned int* puRealWidth);

    //C1 E0 18 = shift left with 0x18(24)
    static auto CFont__Drawfont = reinterpret_cast<CFont__Drawfont_t>(hOrgCanvasBase + 0xC2B0); //v176
    //static auto CFont__Drawfont = reinterpret_cast<CFont__Drawfont_t>(hOrgCanvasBase + 0xD400); //203.4

    CFont__Drawfont_t Hook = [](void* ecx, void* edx, HBITMAP* hBitmap, tagBITMAP* bm, tagPOINT pt, WCHAR c, unsigned int* puPrintWidth, unsigned int* puRealWidth) -> int {
        //Log("[MapleLog] [CFont::DrawFont]");
        //Log("[MapleLog] [CFont::DrawFont] StripePosX: %u, StripePosY: %u,", pt.x, pt.y);

        currentDrawFontCharacter = c;
        auto ret = CFont__Drawfont(ecx, edx, hBitmap, bm, pt, c, puPrintWidth, puRealWidth);
        currentDrawFontCharacter = 0;

        return ret;

    };
    return SetHook(true, reinterpret_cast<void**>(&CFont__Drawfont), Hook);
}

bool hookTextOutA()
{
    static auto _CreateFontA = decltype(&TextOutA)(GetFunctionAddress("GDI32", "TextOutA"));

    decltype(&TextOutA) Hook = [](HDC hdc, int x, int y, LPCSTR lpString, int c) -> BOOL
    {
        //Log("[MapleLog] [GDI32::TextOutA] ");
        //LogW(L"[MapleLog] [GDI32::TextOutA] lpString : %c, currentDrawFontCharacter : 0x%x, x : %u, y : %u, c : %u", *lpString, currentDrawFontCharacter, x, y, c);

        auto ret = _CreateFontA(hdc, x, y, lpString, c);

        if (hdc && (findInArray(currentDrawFontCharacter, hangAboveCharacter) || findInArray(currentDrawFontCharacter, hangBelowCharacter))) {
            int startX = x + 0;
            int startY = y + 5;

            int endX = startX + 7;
            int endY = startY + 5;

            auto removeColor = RGB(255, 255, 255);

            for (int currentX = startX; currentX < endX; currentX++) {
                for (int currentY = startY; currentY < endY; currentY++) {
                    SetPixel(hdc, currentX, currentY, removeColor);
                }
            }
        }

        return ret;
    };

    return SetHook(true, reinterpret_cast<void**>(&_CreateFontA), Hook);
}

#pragma endregion

#pragma region Override charset

bool hookCFontGetLogFont() {

    typedef int(__fastcall* CFont__GetLogfont_t)(void* ecx, void* edx, tagLOGFONTA* lf);
    //A8 20
    static auto CFont__GetLogFont = reinterpret_cast<CFont__GetLogfont_t>(hOrgCanvasBase + 0xC8A0); //176
    //static auto CFont__GetLogFont = reinterpret_cast<CFont__GetLogfont_t>(hOrgCanvasBase + 0xD960); //203.4

    CFont__GetLogfont_t Hook = [](void* ecx, void* edx, tagLOGFONTA* lf) -> int {
        //Log("[MapleLog] [CFont::GetLogFont]");
        auto ret = CFont__GetLogFont(ecx, edx, lf);

        lf->lfCharSet = THAI_CHARSET;

        return ret;

    };
    return SetHook(true, reinterpret_cast<void**>(&CFont__GetLogFont), Hook);
}

#pragma endregion


bool DestinyHookCanvas() {
    shiftCharacters.insert(shiftCharacters.end(), hangAboveCharacter.begin(), hangAboveCharacter.end());
    shiftCharacters.insert(shiftCharacters.end(), hangBelowCharacter.begin(), hangBelowCharacter.end());
    shiftCharacters.insert(shiftCharacters.end(), handRightAndTop.begin(), handRightAndTop.end());

    bool bResult = true;
    
    bResult &= hookCFontDrawTextA();
    bResult &= hookCFontGetLogFont();
    bResult &= hookCFontGetFont();
    bResult &= hookCFontDrawFont();
    bResult &= hookTextOutA();

    return bResult;
}