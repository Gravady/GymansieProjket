#ifndef WIN_H
#define WIN_H

//READERS NOTE: This program call be summed up as a wrapper for WDKs Debugging and error
//handling mechanism integrated into the system as a whole were QT lib might not or have
//an unwished for version of the implementation

//This file in particular is also engineered to handle multiple things that may be flawed or 
//considered unwanted behavior, this is where things like better type and memory handling(RAII),
//asynchronus relations between different classes, this implementation is ISV based

//NOTE: Doxygen specifics should be moved to a speficic informative file, COC?
// |
// | 
// | 
// v
//READERS NOTE: Because this code has more ambigious variables not native to C++17 STL 
//doxygen commenting implementation will be used: https://www.doxygen.nl/
//DOXYGEN STYLE: Javadoc
//DOXYGEN COMMANDS: https://www.doxygen.nl/manual/commands.html

//NOTE: Might add MACROS to classes other then base class as to not include all classes to specific
//files that may only need one or two specfic classes, this will most likely reduce compilation time

#include <include/util/compile.h>

#if defined(_WIN64) || defined(_WIN32) || defined(_WIN32_WCE)
DISPLAYMESSAGE("Windows platform detected", WIN64)
#elif defined(__CYGWIN64__)
DISPLAYMESSAGE("Cygwin platform detected", CYGWIN64)
#elif defined(__CYGWIN__)
DISPLAYWARNING("Cygwin platform detected, compatibility not guaranteed")
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
DISPLAYMESSAGE("Windows platform detected, MSVC compiler detected")
#else
DISPLAYWARNING("Windows platform detected, non-MSVC compiler detected, compatibility not guaranteed")
#if defined(__GNUC__) || defined(__GNUG__)
DISPLAYWARNING("GCC/MinGW compiler detected, compatibility not guaranteed")
#define CUSTOM_COMPILER = 0x1
#elif defined(__clang__)
DISPLAYWARNING("Clang compiler detected, compatibility not guaranteed")
#define CUSTOM_COMPILER = 0x2
#else
DISPLAYERROR("Unknown compiler, compatibility not guaranteed")
#define CUSTOM_COMPILER = 0x0

#endif
#endif
#endif
#endif

#if defined(_WIN32_WINNT ) && (_WIN32_WINNT == 0x0A00)
DISPLAYMESSAGE("Windows 10 or later detected")
#else
DISPLAYWARNING("Windows 10 or later not detected, compatibility not guaranteed")
#endif

#include <basetsd.h>
#include <windows.h>
#include <dbghelp.h>
#include <winbase.h>
#include <memoryapi.h>
#include <processthreadsapi.h>
#include <fileapi.h>

#include <wow64apiset.h>

#include <string>
#include <optional>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <memory>

#include <include/util/checker.h>

#include <include/thread/worker.h>

static const struct
{
    unsigned int value;
    const char* name;
} IMAGE_FILE_MACHINE_MAP[] = {
    { 0x0000, "UNKNOWN" },
    { 0x0001, "TARGET_HOST" },
    { 0x014c, "I386" },
    { 0x0162, "R3000" },
    { 0x0166, "R4000" },
    { 0x0168, "R10000" },
    { 0x0169, "WCEMIPSV2" },
    { 0x0184, "ALPHA" },
    { 0x01a2, "SH3" },
    { 0x01a3, "SH3DSP" },
    { 0x01a4, "SH3E" },
    { 0x01a6, "SH4" },
    { 0x01a8, "SH5" },
    { 0x01c0, "ARM" },
    { 0x01c2, "THUMB" },
    { 0x01c4, "ARMNT" },
    { 0x01d3, "AM33" },
    { 0x01f0, "POWERPC" },
    { 0x01f1, "POWERPCFP" },
    { 0x0200, "IA64" },
    { 0x0266, "MIPS16" },
    { 0x0284, "ALPHA64" },
    { 0x0366, "MIPSFPU" },
    { 0x0466, "MIPSFPU16" },
    { 0x0520, "TRICORE" },
    { 0x0CEF, "CEF" },
    { 0x0EBC, "EBC" },
    { 0x8664, "AMD64" },
    { 0x9041, "M32R" },
    { 0xAA64, "ARM64" },
    { 0xC0EE, "CEE" },
    { 0, nullptr }
};

class WIN_Hardware;
class WIN_Arch;
class WIN_Memory;
class WIN_Comm;
class WIN_Crash;
class WIN_Proc;
class WIN_Base;
class WIN_Error;

//Base class for basic and logical broadly used windows function API tools aswell as general
//type conversion and information handling not appropirate in a include/util file
//NOTE: This file will be worked upon whilst the other classes are being created and may have minimal
//usecase in this implementation, CRTP will be used, RAII aswell

//NOTE: This should have a connection interface where the async task controller can easily connect 
//to the derived class to make the program run with concurrency
class WIN_Base
{
public:
    explicit WIN_Base() = default;
    ~WIN_Base() {
        //freeHandleList(handleList);
    }
private:
    HANDLE baseHandle;
    std::optional<SECURITY_ATTRIBUTES> secAttr;
    std::mutex handleMutex;
    std::unordered_map<HANDLE, WIN_Base*> handleList;
    
    //WIN_Error errorClass;
    friend class WIN_Error;
public:
    WIN_Base* returnDerived() noexcept;

    BOOL addHandle(HANDLE handle);
    VOID freeHandleList(std::unordered_map <HANDLE, WIN_Base*> handleList_);
    [[nodiscard]] LPWSTR tcharToWString(const TCHAR* charString) noexcept;

    VOID registerHandleCallback(HANDLE& handle, LPVOID* functionHandle);
};

//Windows error handler, same level of abstraction as WIN_Base needed to be derived
//Important parent class for WIN_Dbg to parse and handle errors
//NOTE: Needs to dynamically cast between hardware and OS classes to get specific information 
//that could be relevant to the errors
class WIN_Error : public WIN_Base
{
public:
    explicit WIN_Error() = default;
private:
    DWORD lastError;
public:
    VOID forwardToDebugging();
};

//Windows file manipulation class
//Wont be dependant on WIN_Base now
//Inspired by: https://github.com/microsoft/wil/blob/master/include/wil/filesystem.h
class WIN_File : public WIN_Base
{
public:
    explicit WIN_File() = default;

    //TODO:Work on this later, pretty much should be able to give handle for WIN_File implementations
    //through pointer acess to class in constructor
    //Derived inheritance
    template <typename T, typename C, typename ...Args>
    explicit WIN_File(
        T(C::*)(Args...),
        LPVOID callbackPtr,
        LPVOID& lpFileHolder //Pointer to hold the class 
    ) {
        //TODO:Unsure if this, could use dynamic casting once WIN_File is derived to be able
        //to use WIN_File implementations and functions without having to call the class
        &lpFileHolder(static_cast<T(C::*)(Args...)>(this));
    }

    explicit WIN_File(
        LPWSTR filePath_,
        DWORD dwDesiredAcess_,
        SECURITY_ATTRIBUTES fileSecurityAttributes_,
        DWORD flProtect_
    ) :
        filePath(filePath_),
        dwDesiredAcess(dwDesiredAcess_),
        fileSecurityAttributes(fileSecurityAttributes_),
        flProtect(flProtect_)
    {

    }

    struct FileActionInfo;

    typedef std::unordered_map<FileActionInfo, HANDLE> fileActionCache;

private:
    PCWSTR fileName;
    LPWSTR filePath;
    uint16_t pathLength;
    LPCSTR diskPath;
    uint16_t diskPathLength;
    LPVOID fileCallbackhandle;
    BOOL fileExists = false;
    HANDLE fileHandle;
    DWORD dwDesiredAcess;
    DWORD flProtect = PAGE_EXECUTE_READ | SEC_COMMIT; //https://learn.microsoft.com/en-us/windows/win32/api/WinBase/nf-winbase-createfilemappinga
    DWORD dwMaximumSizeHigh = 200;
    DWORD dwMaximumSizeLow = 50;
    char defaultRAWFileDataBuffer[256];
    SECURITY_ATTRIBUTES fileSecurityAttributes;
    using lpSharedFileState = VOID(*)(WIN_File* file);
    friend class WIN_Memory;
    std::shared_ptr<std::mutex> fileSharedMutex = nullptr; //Shared mutex for shared ownership of file
    //Section to signal if current process owning the file is writing or reading to it
    CRITICAL_SECTION readSection;
    CRITICAL_SECTION writeSection;
    OVERLAPPED overlapped;

    //Use something other then chrono to get timestamps and etc of file manipulation things
    //Uses ReadDirectoryChangesW and ReadFileChangesW to get DWORD status
    //TODO:Work on this
    struct FileActionInfo
    {
        const std::string_view fileActionRange = "5m:0s-5:0s";
        BOOL hasFileActionRange = true;
        std::vector<DWORD> fileChangeEvents;
        WIN32_FILE_ATTRIBUTE_DATA currentData;
        FILETIME fileTime;
        SYSTEMTIME realTime;

        VOID handleFileActionRange(FILETIME filetime, const std::string_view range);
        VOID disableFileActionRange();
        VOID appendData(fileActionCache& cache);
    };

    fileActionCache cache;

public:
    LPWSTR getVolumePath(LPCWSTR fileName) noexcept;
    LPCSTR getFullPathName(PCWSTR fileName) noexcept;

    //Used through GetFileAttributesExA
    FileActionInfo getFileManipulationData(LPCSTR fileName);

    VOID initilizeFile(
        HANDLE fileHandle,
        CRITICAL_SECTION& readSection,
        CRITICAL_SECTION& writeSection,
        DWORD dwCretionDisposition,
       DWORD dwFlagsAndAttributes
    ) noexcept;

    //https://learn.microsoft.com/en-us/windows/win32/memory/creating-named-shared-memory
   VOID initializeSharedFileState(
        LPVOID fileCallbackptr,
        HANDLE fileHandle_,
        SECURITY_ATTRIBUTES* security_attributes
     );

   //Defaults to common shared buffer size
   //File security needs to have read and write
   VOID initilizeDataHandling(
       OVERLAPPED& ov,
       const char readBuffer[],
       const char writeBuffer[],
       size_t readBufferSize,
       size_t writeBufferSize,
       const DWORD& flProtectFileSecurity
   ) noexcept;
     
   VOID setFileAttributes(
       LPVOID fileCallpackptr,
       DWORD& flProctect,
       DWORD dwMaximumSizeHigh,
       DWORD dwMaximumSizeLow,
       SECURITY_ATTRIBUTES& fileSecurityAttributes
   );

   //CreateFileX
   BOOL createFile(
       LPCSTR lpFileName,
       DWORD dwDesiredAcess,
       DWORD dwShareMode,
       LPSECURITY_ATTRIBUTES lpSecurityAttributes,
       DWORD dwCreationDisposition,
       DWORD dwFlagsAndAttributes,
       std::optional<HANDLE> hFileHandle
   );

   BOOL writeToFile(

   )
    
    

};

class WIN_Hardware
{
public:
private:
    DWORD MachineType = NULL; //One of the options from IMAGE_MACHINE_TYPE
    BOOL IsMachineSupported;
    std::string MachineTypeName;
    friend class WIN_Arch;
public:
    BOOL isMachineTypeSupported(DWORD MachineType); 
    DWORD getMachineType();
};


//Extracts things like system architecture for gpu and cpu components needed for 
//specific utilites such as stack tracing
class WIN_Arch : public WIN_Hardware
{
public:

private:

    struct CPU_ARCH
    {
        std::string CPU_ARCH_NAME;
        USHORT IMAGE_MACHINE_TYPE;
    };

    struct CPU_HARDWARE
    {

    };

    HANDLE hProcHandle;
    PVOID nakedcallbackPointer;
    PSYMBOLSERVERCALLBACKPROC debugcallbackPointer;
    LPSTACKFRAME64 stackFrame64;
public:
    inline void getProcessorMachineConstants();
    BOOL checkCurrentStackFrameInstance(
        LPSTACKFRAME64& stackFrame,
        PSYMBOLSERVERCALLBACKPROC debugcallbackPointer,
        HANDLE process_handle = nullptr) noexcept;
};


class WIN_Memory : public WIN_Arch
{
public:
private:
    //NOTE: Uses VirtualQuery to display memory info
    MEMORY_BASIC_INFORMATION mbi;
    std::wstring mbi_context;

    STACKFRAME64 frame;
    DWORD MachineType; 
    HANDLE hProcess;
    HANDLE hThread;
    LPSTACKFRAME64 StackFrame;
    PVOID contextRecord;

    LPVOID memory_invoke_callback;

    DWORD addr;
    LPCSTR hr_adress;
    std::shared_ptr<std::unordered_map<MEMORY_BASIC_INFORMATION, LPCSTR>> memory_cache_record;

    //NOTE: Uses StackWalk64 and SymFromAddr
    IMAGEHLP_SYMBOL64 memory_symbol_help;
    PSYMBOL_INFO memory_symbol_information;
    DWORD64 displacement;
    std::string symbol_context;
    std::string full_context = nullptr; //mbi_context + full_context 

    PREAD_PROCESS_MEMORY_ROUTINE64 ReadMemoryRoutine = nullptr;
    PFUNCTION_TABLE_ACCESS_ROUTINE64 FunctionTableAcessRoutine = nullptr;
    PGET_MODULE_BASE_ROUTINE64 ModuleBaseRoutine = nullptr;
    PTRANSLATE_ADDRESS_ROUTINE64 TranslateAdress = nullptr;

    std::wstring FullMemorySystemContext;
       
    enum class CONTEXT_PREFRENCE
    {
        UNKNOWN = 0x0,
        SYMBOL_ONLY = 0x1,
        MEMORY_BASIC_ONLY = 0x2,

    };

    enum class VIRTUAL_MEMORY_PREFRENCE
    {
        UNKNOWN = 0x0,
        VIRTUAL_QUERY = 0x1,
        HEAP_WALK = 0x2
    };
    friend class WIN_Comm;
    friend class WIN_Proc; //Needs to change process privledge for memory stuff to work properly

    WIN_File::lpSharedFileState fileState;

public:
    /*
    * @note Unofficial naming
    * @brief Extract process memory routine from current memory stack, useful for stack call
    *
    * @param hProcess handle processor to track stack trace
    * @param qwBaseAdress base adress of memory to be read
    * @param lpBuffer pointer to a buffer reciving memory to be read
    * @param nSize size of memory to be read, byte specific
    * @param lpNumberOfBytesRead pointer to the number of bytes read
    * @param callbackHandler Callback variable to get outputted callback after function execution
    *
    * @return Execution status
    *
    */
    BOOL PreadProcessMemoryRoutine64(
        HANDLE hProcess,
        DWORD64 qwBaseAddress,
        PVOID lpBuffer,
        DWORD nSize,
        LPDWORD lpNumberOfBytesRead,
        LPVOID& callbackHandler
    ) noexcept;

    
    BOOL PwriteProcessMemoryRoutine64(
        HANDLE hProcess,
        LPVOID lpBaseAdress,
        LPCVOID lpBuffer, 
        SIZE_T nSize,
        SIZE_T numberOfBytesWritten,
        LPVOID& callbackHanlder
    );

    /*
    * @brief Populates STACKWALK64 type with relevant information 
    * @
    */
    VOID getMemoryConfigurations() noexcept;

    VOID fetchMemoryHardware();

    /*
    * @brief
    *
    */
    VOID setContextPrefrence();

    /*
    *
    * 
    */
    VOID fetchFileConfiguration(const char* file);


    template <typename T>
    [[nodiscard]] const char* getMemHex(T& type) noexcept;

    //NOTE: Needs to be worked on
    DWORD getSymbolFromAdress();

    VOID retrieveSystemAndProcessMemoryStatus(
        LPMEMORYSTATUS lpBuffer,

     );

    //TODO: Needs to be worked on, uses GetPerformanceInfo with some other api tools to
    //retrieve global system performance metrics, whats mostly important here is the memory part,
    //because it retrieves other stuff it might be a part of the WIN_Core base class
    VOID retrievePerformanceInfo();

    VOID getFullMemoryAnalysis();

    BOOL compileFullMemoryContext(std::wstring& FullMemoryContextWstring);
};

//Default as by: https://learn.microsoft.com/en-us/windows/win32/ipc/named-pipe-server-using-overlapped-i-o
#define DEFAULT_BUFSIZE 4096
#define DEFAULT_PIPE_INSTANCE 4
#define DEFAULT_PIPE_TIMEOUT 5000

//TODO: Needs more memory safe apporach
class WIN_Comm
{
public:
    explicit WIN_Comm();
private:
    DWORD bytesreadIn;
    DWORD bytesreadOut;
    std::ostream* streamline;

    HANDLE hReadPipe;
    HANDLE hWritePipe;

    typedef struct 
    { 
       OVERLAPPED oOverlap; 
       HANDLE hPipeInst; 
       TCHAR chRequest[DEFAULT_BUFSIZE]; 
       DWORD cbRead;
       TCHAR chReply[DEFAULT_BUFSIZE];
       DWORD cbToWrite; 
       DWORD dwState; 
       BOOL fPendingIO; 
    } PIPEINST, *LPPIPEINST; 

    //TODO: Check if changing this to a vector is better for flexability
    PIPEINST pipe[DEFAULT_PIPE_INSTANCE];
    HANDLE hEvents[DEFAULT_PIPE_INSTANCE];

    std::unordered_map<LPPIPEINST, DWORD> instanced_pipes;

    BOOL fSucess = false;
    BOOL pipeOpen = false;
    std::mutex pipe_mutex;
    LPCSTR named_pipe;
    LPCSTR pipe_file;

    struct PipeConfigure
    {
        DWORD openMode = PIPE_ACCESS_DUPLEX;
        DWORD pipeMode = PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT;
    };

    DWORD errorInstance;

    enum class INFORMATION_PRESUMED_CONTEXT
    {
        MEM_ADRESS, //0x00000
        WIN_ERROR_CODE, //ERROR_FILE_NOT_FOUND
        MODULE_PATH, //C:/Program/Files
        WIN_SRC_FILE, //C:\src/device
        GUID, //4A1F2C10-DF1A-44FA-A8E3-0807A4FF1AC7
        WIN64STACKFRAME, //kernel32.dll!BaseThreadInitThunk+0x14
        WINREGISTERDUMP, //rax=0000000000000000 rbx=0000000000012340 rcx=7FFDE1234000
        HEXDUMP, //48 89 E5 48 83 EC 20 89 7D FC
        UNKNOWN 
    };

    friend class WIN_Dbg; //Dis good solution?
    
public:
    VOID registerPipeObject(LPCSTR pipe_name);
    VOID addNamedPipe();
    VOID connectToObject(DWORD pipeInstance);
    VOID getAnswerFromRequest(LPPIPEINST pipeInst);
    VOID configurePipeInstance(PipeConfigure& configure) noexcept;
    VOID waitForConnectionInstance();
    LPCSTR getNamedPipeInstance();
    BOOL runPipeInstance();
    VOID processCleanup();
    
    VOID WINAPI CompletedWriteRoutine(DWORD dwErr, DWORD cbWritten, LPOVERLAPPED oOverlap); 
    VOID WINAPI CompletedReadRoutine(DWORD dwErr, DWORD cbRead, LPOVERLAPPED oOverlap); 

    VOID addFormatLabel(
        std::wstring context,
        std::optional<INFORMATION_PRESUMED_CONTEXT> persumed_context);
};

//Windows crash handler API wrapper and for ease of use integrated with the rest of the system
class WIN_Crash
{
public:
private:
public:
};

//Process creator, attacher and handler, parses the threads but does not handle them directly
//NOTE: Child process created from Parent process(this program) should inherit system variables, be
//able to acess file variables, have pipeline communication to send information both ways
class WIN_Proc
{
public:
    /*
    * @brief Constructor for initilizing child process, primary usecase to display relevant debugging
    * and system variable information in a easy and readable way with I/O piping enabling command 
    * parsing to change parent process runtime, process memory, etc
    * 
    * @param name Name of the process or file path, needs to be exact binary name
    * @param creation_flag Creation flag attributes to go with process creation
    * @param proc_attr Specific process attributes, should have specified parent and child inheritance
    * for proper process communication and piping
    * @param thread_attr Thread attribute for thread created by child process
    * @param process_information Process information about the process, may be unset
    * @param unicode_ Specifies if process uses unicode decoding or Windows default utf8
    * @param directory_file_state Master directory where shared files where logging information may 
    * be dumped, this garantuees saftey of information that may need to persist when the process is 
    * terminated by the user
    * @param communication Windows communciations file handling I/O, byte buffer, communication state,
    * communication mutexes and more
    */
    explicit WIN_Proc(
        LPCSTR name,
        DWORD creation_flag = 0,
        LPSECURITY_ATTRIBUTES proc_attr,
        LPSECURITY_ATTRIBUTES thread_attr,
        LPPROCESS_INFORMATION process_information = 0,
        BOOL unicode_ = false,
        LPCSTR directory_file_state,
        WIN_Comm communication_) :
        lpApplicationName_(name),
        dwCreationFlags_(creation_flag),
        lpProcessAttributes_(proc_attr),
        lpThreadAttributes_(thread_attr),
        lp_pi(process_information){ }

    explicit WIN_Proc();

    //NOTE: May need process cleanup
    ~WIN_Proc() = default;

private:
    BOOL unicode = false; //Specfify CREATE_UNICODE_ENVIRONMENT in dwCreationFlags

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    LPPROCESS_INFORMATION lp_pi;
    WIN_Comm communication;

    LPCSTR lpApplicationName_ = "cmd.exe";
    LPSTR lpCommandLine_ = 0;
    LPSECURITY_ATTRIBUTES lpProcessAttributes_ = 0;
    LPSECURITY_ATTRIBUTES lpThreadAttributes_ = 0;
    BOOL bInheritHandles_ = false;
    DWORD dwCreationFlags_ = 0; //https://learn.microsoft.com/en-us/windows/win32/procthread/process-creation-flags
    LPVOID lpEnvironment_ = 0;

    enum class ProcessType
    {
        UNKNOWN = 0x0,
        TERMINAL_PROCESS = 0x1,
        DERIVEABLE_COPY_PROCESS = 0x2,
        OTHER_PROCESS = 0x3
    };
    
    ProcessType processType = ProcessType::UNKNOWN;

    /*typedef struct _STARTUPINFOA
    {
        DWORD  cb;
        LPSTR  lpReserved;
        LPSTR  lpDesktop;
        LPSTR  lpTitle;
        DWORD  dwX;
        DWORD  dwY;
        DWORD  dwXSize;
        DWORD  dwYSize;
        DWORD  dwXCountChars;
        DWORD  dwYCountChars;
        DWORD  dwFillAttribute;
        DWORD  dwFlags;
        WORD   wShowWindow;
        WORD   cbReserved2;
        LPBYTE lpReserved2;
        HANDLE hStdInput;
        HANDLE hStdOutput;
        HANDLE hStdError;
    } STARTUPINFOA, * LPSTARTUPINFOA;
    */

public:
    /*
    *
    * @brief Almost mandatory startup function for each process determining if
    * things like enovirement variables, process inheritance structure, process handle,
    * decoder state, etc for a healthy CreateProcess* startup
    * 
    * @param si Startupinfo var that needs to be populate by the user, hardcoded to be set
    * but a configuration file can be implemented in the future although with defualt fallback
    * @param pi Output variable only real usecase in this scenario is for callback promises 
    * garantueeing that the parent process gets information about the process, optional param
    * @param lpEnvoirement Envoiremenet variables
    * @param lpProcessAttributes Attributes towards the process itself
    * @param ThreadAttributes Attributes for the thread that the process runs on, can be changed but
    * should mostly follow the same as the thread controller
    * 
    */
    VOID procStartupRun(
        STARTUPINFO& si,
        std::optional<PROCESS_INFORMATION>& pi,
        LPVOID lpEnvoirement,
        SECURITY_ATTRIBUTES& lpProcessAttributes,
        SECURITY_ATTRIBUTES& ThreadAttributes);

    BOOL getStartupInfoVars();

    BOOL isUnicode();

    inline static BOOL openProcess(
        DWORD dwDesiredAcess,
        BOOL bInheritHandle,
        DWORD dwProcessId,
        WIN_Comm& communications_handle
    );

    static inline DWORD getProcessId();
};
