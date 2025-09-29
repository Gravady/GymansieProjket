#ifndef THREAD_SINGLE_H
#define THREAD_SINGLE_H

#include <QThread>

#include <include/thread/thread_core.h>

class THREAD_SINGLE : public THREAD_CORE
{
public:
	THREAD_SINGLE() = default;
	THREAD_SINGLE(const THREAD_SINGLE& thread) = delete;

	~THREAD_SINGLE() {
		this->threadStop(*this->thread_object->thread(), true);
	}
public:

	[[nodiscard]] virtual THREAD_STATUS threadRun(
		QThread& thread,
		std::optional<func_ptr_t<qtask_t>> func
	);

	[[nodiscard]] virtual THREAD_STATUS threadPause(
		QThread& thread,
		QThread::Priority priority = QThread::NormalPriority
	) override;

	[[nodiscard]] virtual THREAD_STATUS threadResume(
		QThread& thread,
		QThread::Priority priority = QThread::InheritPriority
	) override;

	[[nodiscard]] virtual THREAD_STATUS threadWait(
		QThread& thread,
		time_t time
	) override;
	[[nodiscard]] virtual THREAD_STATUS threadWait(
		QThread& thread,
		QDeadlineTimer timer = QDeadlineTimer(QDeadlineTimer::Forever)
	) override;

	[[nodiscard]] virtual THREAD_STATUS threadStop(
		QThread& thread,
		bool waitForFinished = true
	) override;

	static bool isThreadRunning();
	static uint8_t moveToThread(QThread& thread, QObject* object);
	static uint8_t threadInit(QObject* object);
	static uint8_t connectThread(
		QObject* sender,
		const char* signal,
		std::optional<QObject*> receiver, //this elsewhere
		const char* slot,
		Qt::ConnectionType type = Qt::AutoConnection
	);
	static uint8_t disconnectThread(
		QObject* sender,
		const char* signal,
		std::optional<QObject*> receiver, //this elsewhere
		const char* slot
	);

	void assignDanglingObjects(std::vector<QObject*> thread_list);

private:
	QThread::Priority thread_priority = QThread::InheritPriority;
	QObject* thread_object = nullptr;
	std::vector<QObject*> thread_objects;
};

#endif