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

class Controller : public QObject
{
	Q_OBJECT
public:
	explicit Controller(std::unique_ptr<ProcSignal<T, R>> processor)
private:
public:
};

#endif