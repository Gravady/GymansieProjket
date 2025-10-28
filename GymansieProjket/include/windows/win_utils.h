#ifndef WIN_UTILS_H
#define WIN_UTILS_H

#include <windows.h>
#include <dbghelp.h>

//This needs to be changed
#include <string_view>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <queue>

#include <QThread>
#include <QMutex>
#include <QQueue>
#include <QWaitCondition>

//YAGNI change
//TODO: Check all of this mess tommorow, it needs to be interlock with thread header implementation
// instead of doing its own thing, needs to link with controller.h
// 
//Terminal for debugging purposes, changed it to this from a full fledged windows proc system
//because its not needed in this simple example
//NOTE: Not a flexiable class, only usecase is for there to be 1 displayable cmd window 
class WINTERMINAL : public QThread
{
	Q_OBJECT
public:
	WINTERMINAL(QObject* object) : QThread(object), hChildStd_IN_Wr(NULL), hChildStd_OUT_Rd(NULL){
		terminalCreate();
		start();
	}
	WINTERMINAL() {
		terminalCreate();
		start();
	}
	~WINTERMINAL() {
		wait();
		terminalEnd();
	}

	//Input
	WINTERMINAL& operator >> (const char* streamline_data) noexcept;
	WINTERMINAL& operator >> (QString streamline_data) noexcept;

	//Output
	WINTERMINAL& operator << (const char* streamline_data) noexcept;
	WINTERMINAL& operator << (QString streamline_data) noexcept;
private:
	HANDLE hChildStd_IN_Wr;
	HANDLE hChildStd_OUT_Rd;
	PROCESS_INFORMATION piProc;
	QThread terminal_thread;
	bool is_running = false;

	std::mutex output_data_streamline_lock;
	std::queue<QString> output_data_streamline;

	std::mutex input_data_streamline_lock;
	std::queue<const char*> input_data_streamline;
public:
	//Return values may be changed for debugging purposes
	static void terminalCreate();
	static void terminalRun();
	static void terminalEnd();
	void newOuput(const QString& data); //May be changed to something else
};

//Stack information for debugging purposes using winapi
#pragma comment(lib, "dbghelp.lib")
class WINSTACK
{

};
#endif