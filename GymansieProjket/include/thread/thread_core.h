#ifndef THREAD_CORE_H
#define THREAD_CORE_H

#include <include/util/compile.h>
#include <include/util/types.h>

#include <cstdint>
#include <optional>

#include <QFuture>

class qtask_t
{
public:
public:
private:
};

class THREAD_CORE
{
public:
	//Only allow move semantics
	explicit THREAD_CORE(THREAD_CORE&& thread) = default;
	explicit THREAD_CORE(const THREAD_CORE& thread) = delete;
public:
	enum class THREAD_STATUS
	{
		THREAD_RUNNING = 0x0,
		THREAD_PAUSED = 0x1,
		THREAD_STOPPED = 0x2,
		THREAD_CANCELLED = 0x3,
		THREAD_WAITING = 0x4,
		THREAD_CRASH = 0x5,
	};

	virtual QFuture<qtask_t> threadRun(
		std::optional<QThreadPool>& pool,
		func_ptr_t<qtask_t> func
	) = 0;
	[[nodiscard]] virtual THREAD_STATUS threadRun(
		QThread& thread,
		std::optional<func_ptr_t<qtask_t>> func
	) = 0;

	[[nodiscard]] virtual THREAD_STATUS threadPause(
		QFuture<qtask_t>future
	) = 0;
	[[nodiscard]] virtual THREAD_STATUS threadPause(
		QThread& thread,
		QThread::Priority priority = QThread::NormalPriority
	) = 0;

	[[nodiscard]] virtual THREAD_STATUS threadResume(
		QFuture<qtask_t>& future
	) = 0;
	[[nodiscard]] virtual THREAD_STATUS threadResume(
		QThread& thread,
		QThread::Priority priority = QThread::InheritPriority
	) = 0;

	[[nodiscard]] virtual THREAD_STATUS threadWait(
		QFuture<qtask_t>& future,
		time_t time
	) = 0;
	[[nodiscard]] virtual THREAD_STATUS threadWait(
		QFuture<qtask_t>& future,
		QDeadlineTimer timer = QDeadlineTimer(QDeadlineTimer::Forever)
	) = 0;
	[[nodiscard]] virtual THREAD_STATUS threadWait(
		QThread& thread,
		time_t time
	) = 0;
	[[nodiscard]] virtual THREAD_STATUS threadWait(
		QThread& thread,
		QDeadlineTimer timer = QDeadlineTimer(QDeadlineTimer::Forever)
	) = 0;

	[[nodiscard]] virtual THREAD_STATUS threadStop(
		QFuture<qtask_t>& future,
		bool waitForFinished = true
	) = 0;
	[[nodiscard]] virtual THREAD_STATUS threadStop(
		QThread& thread,
		bool waitForFinished = true
	) = 0;

	uint8_t getCurrentThreadStatus();
	void setCurrentThreadStatus(THREAD_STATUS status);
	void displauyCurrentThreadStatus();
	THREAD_STATUS determineThreadStatus(QFuture<qtask_t>& future);
	THREAD_CORE* returnPtr();
private:
	static THREAD_STATUS current_thread_status;
};

#endif