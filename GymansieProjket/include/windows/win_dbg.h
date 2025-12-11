#ifndef DBG_H
#define DBG_H

#include <QDebug>

#include <memory>
#include <string>
#include <mutex>
#include <atomic>

#include <include/windows/win.h> //WIN_Proc

//NOTE:Dbg moved and changed to win_dbg.h this is because its recommended to use Windows native
//API functionality for debugging on a windows system, other operating systems can go and fuck themselfs
//because that is too much work

/*
* @brief Underlying debug class
* 
*/
//NOTE: Use QueryPerformanceFrequency for timing function calls
class WIN_Dbg
{
public:
	
private:
	enum class DebugType
	{
		UNCATEOGIRZED = 0x0,
		DEBUG_NOTE = 0x1,
		DEBUG_GREENLIT = 0x2,
		DEBUG_WARNING = 0x3,
		DEBUG_ERROR = 0x4
	};
	WIN_Error descriptive_error;
	LPWSTR formattedDebugging;
	BOOL usePrimalDebugging = false;
	size_t debuggingBufferSize = 256;
	TCHAR debuggingBuffer[256];
	BOOL consoleColors = false;
	DWORD handleMode = 0;

	//Octal color representation, has to be manually added
	enum class COLORLIST
	{
		RESET = 0x000,
		RED = 0x031,
		GREEN = 0x032,
		YELLOW = 0x033,
		CYAN = 0x036, 
		DEFAULT = 0x039,
	};

public:
	//SetConsoleMode(ENABLE_VIRTUAL_TERMINAL_PROCESSING
	VOID enableConsoleColors() noexcept;

	VOID exitDebugging() noexcept;

	VOID assignDebuggingColor(const TCHAR* debbugingType) noexcept;
	//NOTE: Firstly needs to be formatted like:
	//| TimeDate | [ERROR/WARNING/NOTE/GREENLIT][Context]
	//<Memory Adress #AH213> : Explanation of debugging context (Code file)
	VOID forwardDebugging(LPCSTR& formattedDebugging);

	//Primal means without any formatting or dependency used for when things break that cant be formatted
	//like if the debugging logic itself breaks
	VOID forwardPrimalDebugging(const char* primalFormattedDebugging);

	//NOTE: Display speciifc memory information from 
	VOID forwardMemoryContext(
		LPVOID memoryAdress,
		MEMORY_BASIC_INFORMATION information,
		DWORD64 memoryOffset
	);

	DebugType diagnoseDebugType(DWORD herrorHandle = GetLastError());

	VOID formatDebuggingContext(
		SYSTEMTIME& time,
		DebugType type,
		LPCWSTR context,
		LPVOID memory_adress,
		LPCWSTR explanation,
		FILE code_file,
		size_t bufferSize
	) noexcept;

	//NOTE: Used only for string literals that can be aquired by conversion methods
	VOID formatDebuggingContextS(
		LPWSTR& time,
		LPCSTR type,
		LPCSTR context,
		LPWSTR memory_adress,
		LPCWSTR explanation,
		LPCWSTR code_file,
		LPCSTR bufferSize
	) noexcept;

	inline [[nodiscard]] SYSTEMTIME getDebuggingTimeStamp(LPVOID processCallback = nullptr) noexcept;
	inline LPWSTR getDebuggingTimeStampW(LPVOID processCallback = nullptr) noexcept;
};

/*
* @brief Point of this class is to display and take commands for relevant debug information regarding
* the software, this class may seem confusing from WIN_Dbg_Terminal, the main difference is that 
* this class specifically 
* 
*/ 
class WIN_Dbg_Terminal : WIN_Dbg
{
public:

private:

public:
	//TODO:Work on displaying the message 
	VOID displayForwardedMessage(LPCSTR forwardedMessage) noexcept;
};

#endif