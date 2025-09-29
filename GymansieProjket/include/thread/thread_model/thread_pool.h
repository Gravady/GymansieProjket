#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <QThreadPool>
#include <QRunnable>
#include <QFuture>
#include <QtConcurrent/>

#include <include/thread/thread_core.h>

template <typename T, typename = std::enable_if_t<std::is_base_of_v<QObject, T>>>
class THREAD_POOL : public THREAD_CORE
{
public:
	THREAD_POOL();
	virtual ~THREAD_POOL() = default;
public:
	[[nodiscard]] QFuture<qtask_t> threadRun(
		std::optional<QThreadPool>& pool,
		func_ptr_t<qtask_t> func
	) override;

	[[nodiscard]] THREAD_STATUS threadPause(
		QFuture<qtask_t>future
	) override;

	[[nodiscard]] virtual THREAD_STATUS threadResume(
		QFuture<qtask_t>& future
	) override;

	[[nodiscard]] virtual THREAD_STATUS threadWait(
		QFuture<qtask_t>& future,
		time_t time
	) override;
	[[nodiscard]] virtual THREAD_STATUS threadWait(
		QFuture<qtask_t>& future,
		QDeadlineTimer timer = QDeadlineTimer(QDeadlineTimer::Forever)
	) override;

	[[nodiscard]] virtual THREAD_STATUS threadStop(
		QFuture<qtask_t>& future,
		bool waitForFinished = true
	) override;

	static THREAD_STATUS clearPool();
	[[nodiscard]] static QThreadPool* getPool();

	uint8_t getStackSize() const;
	void setStackSize(uint8_t size);
private:
	inline static QThreadPool* pool = QThreadPool::globalInstance();
	uint8_t maxThreads;
	uint8_t threadCount;
};

#endif