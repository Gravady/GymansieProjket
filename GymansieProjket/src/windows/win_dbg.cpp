#include <include/windows/win_dbg.h>
#include <include/windows/win_utils.h>

#include <sysinfoapi.h>

VOID WIN_Dbg::enableConsoleColors() noexcept {
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleMode(hOut, &handleMode);
	if (!SetConsoleMode(hOut, handleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
		this->forwardPrimalDebugging("Unable to set color for more expressive debugging[Debugging issue]");
		this->exitDebugging();
	}
	CloseHandle(hOut); //Dont see a reason for why you shouldt close it here
}

//NOTE: Callback will be handled by threading to execute at a specific time
SYSTEMTIME WIN_Dbg::getDebuggingTimeStamp(LPVOID processCallback = nullptr) noexcept
{
	SYSTEMTIME lt;
	GetLocalTime(&lt);
	return lt;
}

LPWSTR WIN_Dbg::getDebuggingTimeStampW(LPVOID processCallback = nullptr) {
	SYSTEMTIME lt;
	GetLocalTime(&lt);
}

VOID forwardPrimalDebugging(const char* primalFormattedDebugging) {

}


VOID WIN_Dbg::formatDebuggingContext(
	SYSTEMTIME& time,
	DebugType type,
	LPCWSTR context,
	LPVOID memory_adress,
	LPCWSTR explanation,
	FILE code_file,
	size_t bufferSize
) {
	wchar_t* formatBuffer = (wchar_t*)malloc(bufferSize * sizeof(wchar_t));
	swprintf(
		formatBuffer,
		bufferSize,
		L"%04u-%02u-%02u %02u:%02u:%02u.%03u",
		time.wYear, time.wMonth, time.wDay,
		time.wHour, time.wMinute, time.wSecond,
		time.wMilliseconds);
	dumpWString(this->formattedDebugging, formatBuffer);
}

VOID formatDebuggingContextS(
	LPWSTR& time,
	LPCSTR type,
	LPCSTR context,
	LPWSTR memory_adress,
	LPCWSTR explanation,
	LPCWSTR code_file,
	LPCSTR bufferSize
) noexcept {

}

//Using fmt lib
VOID WIN_Dbg_Terminal::displayForwardedMessage(LPCSTR forwardedMessage) noexcept{

}
