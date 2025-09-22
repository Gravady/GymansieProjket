#ifndef THREAD_GROUP_H
#define THREAD_GROUP_H

//Temporary include


//Docs/Sources
//https://doc.qt.io/qt-6/qtconcurrent-index.html
//https://doc.qt.io/qt-6/qtconcurrentrun.html

#include <QtConcurrent>
#include <QFuture>

//Non-permanent includes for linking
#include <include/util/util.h>
#include <include/gui/gui.h>

#include <type_traits> 
#include <atomic>

//Class to manage threading operations like QtConcurrent::run and the status generated from thereof
template <typename T, typename = std::enable_if_t<std::is_class<T>::value>>
class Thread
{
public:

	Thread() = default;
public:
	//runThread and startThread can probably be the same thing
	QFuture<void> runThread(
		std::optional<QThreadPool> &pool,
		func_ptr_t func);
	QFuture<auto> runThread(
		std::optional<QThreadPool> &pool,
		func_ptr_t<auto> func);

	uint8_t startThread(QThreadPool& pool); //QThreadPool::globalInstance->start(hello);

	//Maybe string literal instead of std::string?
	QFuture<QString> runThread(std::optional<QThreadPool>& pool, func_ptr_t<std::string> func);
	template <typename T>
	QFuture<T> runThread(std::optional<QThreadPool>& pool, func_ptr_t<T> func) noexcept;

	uint8_t threadWait(QThread& thread);
	template <typename T, typename = std::enable_if_t<std::is_class<T>::value>>
	//QObject::connect(thread, &QThread::finished, worker, &QObject::deleteLater); //worker = Worker class
	uint8_t threadLink(QObject& object,
		QThread& thread,
		std::optional<&QThread::finnished> is_finnished,
		std::optional<&QObject::deleteLater> deleteLater);
	
	uint8_t threadLink(Worker& worker,
		QObject& object,
		QFuture<void>& future,
		std::optional<&QFuture<void>::isFinished> is_finnished,
		std::optional<&QFuture<void>::cancel> cancel);
	uint8_t threadCancel(QFuture<void>& future);

	//Change return value to something similar to time_t
	uint8_t

	//Needs work if its going to function
	//uint8_t displayThreadAcitivity() noexcept;
	//uint8_t addThreadActivity();

	uint8_t setThreadQuality(QThread::QualityOfService& serivce);

private:
	static std::vector<QFuture<void>> future;
	//static std::vector<std::vector<unsigned char>> thread_activity_char;

	enum class THREAD_ACTIVITY
	{

	};

	std::vector<std::atomic<QThread>> thread_stack;
	std::atomic <QThread::QualityOfService> service;
	QThreadPool* pool;

};

#endif