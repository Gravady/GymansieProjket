#include <include/windows/win.h>
#include <include/windows/win_dbg.h>

#include <stringapiset.h> //MultiByteToWideChar

//Need to work on Comm to print errors;(

//Chatgpt solution, probably not good char conversion
LPWSTR WIN_Base::tcharToWString(const TCHAR* charString) noexcept {
#ifdef UNICODE
    size_t len = wcslen(charString) + 1;
    LPWSTR out = new wchar_t[len];
    wcscpy_s(out, len, charString);
    return out;
#else
    size_t len = MultiByteToWideChar(CP_UTF8, 0, charstring, -1, nullptr, 0);
    if (len <= 0)
        return nullptr;

    LPWSTR out = new wchar_t[len];
    MultiByteToWideChar(CP_UTF8, 0, charString, -1, out, len);
    return out;
#endif
}

LPWSTR WIN_File::getVolumePath(LPCWSTR fileName) noexcept {
    TCHAR buffVolPath[MAX_PATH];
	if (this->fileExists) {
        if ((GetVolumePathNameW(fileName, buffVolPath, MAX_PATH))) {
            return tcharToWString(buffVolPath);
        }
	}
}

VOID registerHandleCallback(HANDLE& handle, LPVOID* functionHandle) {
    WaitForSingleObject(functionHandle, INFINITE);

}

VOID setFileAttributes(
    LPVOID fileCallpackptr,
    DWORD& flProctect,
    DWORD dwMaximumSizeHigh,
    DWORD dwMaximumSizeLow,
    SECURITY_ATTRIBUTES& fileSecurityAttributes
){

}
