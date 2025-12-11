//Moslty functional header only implementations

#include <windows.h>
#include <winbase.h>
#include <cstdint>

#include <type_traits>
#include <memory>

#include <include/util/types.h>

//Better async version may be needed, will be more implemented with threading system
//https://learn.microsoft.com/en-us/windows/win32/api/profileapi/nf-profileapi-queryperformancecounter
DOUBLE WINAPI setTimer(
	LARGE_INTEGER freq,
	LARGE_INTEGER time,
	LPVOID callback,
	HANDLE threadHandle
) {
	LARGE_INTEGER start;
	LARGE_INTEGER end;

	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&start);
	WaitForSingleObject(threadHandle, INFINITE);
	QueryPerformanceCounter(&end);
		
	return static_cast<DOUBLE>(end.QuadPart - start.QuadPart)
		/ static_cast<DOUBLE>(freq.QuadPart);
}

USHORT charToUshort(const char* character) {
	return reinterpret_cast<USHORT>(character);
}

inline VOID WINAPI dumpWString(LPWSTR wString, WCHAR wideChar[MAX_PATH]) noexcept {
	wcscat(wString, wideChar);
	deallocateDefMem(wideChar);
}

inline VOID WINAPI dumpCString(LPCSTR cString, TCHAR shortChar) noexcept {
	wscat
}
