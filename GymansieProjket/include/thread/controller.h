#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QQueue>

#include <memory>
#include <vector>
#include <mutex>
#include <atomic>

#include <include/thread/proc_signal>

//This will substitute the thread_model implementation as its better
//inspired by https://github.com/polygamma/Qt-Multithreading/tree/master
//thread_core will also be removed

//Use https://learn.microsoft.com/en-us/windows/win32/fileio/i-o-completion-ports?utm_source=chatgpt.com
//Completion ports for handling async threads

class Controller : public QObject
{
	Q_OBJECT
public:
	explicit Controller(std::unique_ptr<ProcSignal<T, R>> processor)
private:
public:
};

#endif