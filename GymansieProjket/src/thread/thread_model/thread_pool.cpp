#include <include/thread/thread_model/thread_pool.h>

[[nodiscard]] QFuture <qtask_t> THREAD_POOL<QObject>::threadRun(
	std::optional<QThreadPool>& pool,
	func_ptr_t<qtask_t> func
)
{
	
}