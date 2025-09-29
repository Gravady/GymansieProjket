#include <include/thread/thread_model/thread_single.h>

[[nodiscard]] THREAD_CORE::THREAD_STATUS THREAD_SINGLE::threadRun(
	QThread& thread,
	std::optional<func_ptr_t<qtask_t>> func
){
	if(thread.isRunning()){
		return THREAD_STATUS::THREAD_RUNNING;
	}
	if(func.has_value()){
		QObject::connect(&thread, &QThread::started, [func]() {
			func.value()();
		});
	} else {
		return THREAD_STATUS::THREAD_CANCELLED;
	}

	thread.start();
	if (thread.isRunning()) {
		return THREAD_STATUS::THREAD_RUNNING;
	}
	return THREAD_STATUS::THREAD_STOPPED;
}

[[nodiscard]] THREAD_CORE::THREAD_STATUS THREAD_SINGLE::threadPause(
	QThread& thread,
	QThread::Priority priority
) {
	if(thread.isRunning()){
		thread.wait(QDeadlineTimer(QDeadlineTimer::Forever));
		return THREAD_STATUS::THREAD_PAUSED;
	}
	thread.setPriority(priority);
	return THREAD_STATUS::THREAD_PAUSED;
}

[[nodiscard]] THREAD_CORE::THREAD_STATUS THREAD_SINGLE::threadResume(
	QThread& thread,
	QThread::Priority priority
) {
	if(!thread.isRunning()){
		thread.start();
		return THREAD_STATUS::THREAD_RUNNING;
		if(!thread.isRunning()){
			return THREAD_STATUS::THREAD_CRASH;
		}
	}
	thread.setPriority(priority);
	return THREAD_STATUS::THREAD_RUNNING;
}