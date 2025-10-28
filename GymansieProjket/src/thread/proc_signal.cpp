#include <include/thread/proc_signal.h>
#include <include/util/dbg.h> //Debugging currently not implemeneted

//TODO:Fix this small issue later
template <typename RetCallback, typename ...Args,typename CallbackReciever>
inline void SignalPromise<RetCallback, Args...>::addTask(
	RetCallback(* const task)(Args...),
	Args&&... args, 
	std::promise<RetCallback> promise_object,
	CallbackReciever&& reciever
)
{
	QMutexLocker locker(&this->promise_mutex);
	auto future = promise_object.get_future();
	RetCallback result = task(std::forward<TaskArgs>(args)...);
	promise_object.set_value(result);
	reciever(std::move(future));
}

template <typename RetCallback, typename ...Args>
inline void SignalPromise<RetCallback, Args...>::setPromiseValue(const RetCallback& value) {
	QMutexLocker locker(&this->promise_mutex);
	this->promise_callback.set_value(value);
}

template <typename RetCallback, typename ...Args>
std::future<RetCallback> SignalPromise<RetCallback, Args...>::getFuture() {
	QMutexLocker locker(&this->promise_mutex);

	if (!this->future_callback.has_value()) {
		this->future_callback = this->promise_callback.get_future();
	}
	return std::move(*this->future_callback);
}

template <typename RetCallback, typename ...Args>
inline void SignalPromise<RetCallback, Args...>::resetPromise() {
	this->promise_callback = std::promise<void*>();
}

template <typename RetCallback, typename ...Args>
bool SignalPromise<RetCallback, Args...>::hasPromise() {
	return this->promise_callback.load() ? true : false;
}

QMutex SlotProviderRegistry::registry_mutex;

bool SlotProviderRegistry::registerSlotProvider(SlotProvider* provider) noexcept {
    if (!provider)
        return false;

    SlotProviderRegistry& reg = getInstance();
    QMutexLocker locker(&registry_mutex);

    const char* id = generateSlotProviderId(provider);
    reg.slot_provider_id_map[id] = provider;
    reg.slot_provider_reverse_map[static_cast<void*>(provider)] = provider;

    return true;
}

bool SlotProviderRegistry::unregisterSlotProvider(const char* provider_id) noexcept {
    if (!provider_id)
        return false;

    SlotProviderRegistry& reg = getInstance();
    QMutexLocker locker(&registry_mutex);

    auto it = reg.slot_provider_id_map.find(provider_id);
    if (it == reg.slot_provider_id_map.end())
        return false;

    reg.slot_provider_reverse_map.erase(static_cast<void*>(it->second));
    reg.slot_provider_id_map.erase(it);
    return true;
}

const char* SlotProviderRegistry::generateSlotProviderId(SlotProvider* provider) noexcept {
    SlotProviderRegistry& reg = getInstance();
    QMutexLocker locker(&registry_mutex);

    static std::random_device dev;
    static std::mt19937 rng(dev());
    static std::uniform_int_distribution<int> dist(0, 255);

    uint8_t number_instance;
    bool is_unique = false;

    do {
        number_instance = static_cast<uint8_t>(dist(rng));
        is_unique = true;

        for (const auto& [id_str, provider_ptr] : reg.slot_provider_id_map) {
            if (!id_str[0] && static_cast<uint8_t>(id_str[0]) == number_instance) {
                is_unique = false;
                break;
            }
        }
    } while (!is_unique);
}

SlotProvider* SlotProviderRegistry::getSlotProviderById(const char* provider_id) noexcept {
    if (!provider_id)
        return nullptr;

    SlotProviderRegistry& reg = getInstance();
    QMutexLocker locker(&registry_mutex);

    auto it = reg.slot_provider_id_map.find(provider_id);
    return (it != reg.slot_provider_id_map.end()) ? it->second : nullptr;
}

std::unordered_map<void* const, SlotProvider*> SlotProviderRegistry::getSlotProviderMap() noexcept {
    SlotProviderRegistry& reg = getInstance();
    QMutexLocker locker(&registry_mutex);
    return reg.slot_provider_reverse_map;
}

//NOTE: This logic may be worked upon in the future but right now its good enough
inline SignalProvider* SlotProvider::signalSender() noexcept {
    QMutexLocker locker(this->mutex);
    QThread* const executingThread = this->thread();

    if (this->signal_senders.count(executingThread)) {
        auto& queue = this->signal_senders[executingThread];
        if (!queue.empty()) {
            return queue.back();
        }
    }
    return nullptr;
}

bool SlotProvider::checkSlotAvaliability(
    std::unordered_map<QThread*,
    std::vector<SignalProvider*>> slots_
) {
    QMutexLocker locker(this->mutex);
    for (const auto [thread, providers] : slots_) {
        if (thread && providers.empty()) {
            return true;
        }
    }
    return false;
}

//NOTE: Might change this implementation later but right now im just going to steal it
template <typename Callable, typename ...Args>
inline bool SlotProvider::callSlot(
    SignalProvider* signal,
    Callable&& callable_object,
    Args&&... args,
    std::optional<std::string> callable_context,
    SignalPromise<void>* promise
) {
    QThread* const executingThread = this->thread();
    {
        QMutexLocker signalSendersLocker(&this->signalSendersMutex);

        if (this->signalSenders.count(executingThread)) {
            this->signalSenders[executingThread].emplace_back(signalSender);
        }
        else {
            this->signalSenders[executingThread] = std::deque{ signalSender };
        }
    }
    std::invoke(callable, std::forward<Args>(args)...);
    {
        QMutexLocker signalSendersLocker(&this->mutex);

        this->signalSenders[executingThread].pop_back();
    }
}

//TODO:Needs to be fixed but I cant right now
inline void SlotProvider::registerConnection(
    void* const slot,
    SignalProvider* const signalprovider
) {
    QMutexLocker signalSendersLocker(&this->mutex);
    if (this->provider_list.count(slot)) {
        this->provider_list[slot].emplace_back(signalprovider);
    }
}

//NOTE: Implemnetation for other part also needs to be done
//NOTE:Not tampering much with this implementation for now either, check if this is even right later
template <typename Ret, typename ... Args, typename C>
static inline bool ProviderCommunication::disconnectCommunication(
    Ret(* const slot)(Args...),
    SignalProvider* signal_provider,
    SlotProvider* provider
) noexcept{
    if (!slot && !signal_provider) {
        this->provider_list.clear();
    }
    else if (slot && !signal_provider) {
        this->provider_list.erase(slot);
    }
    else if (!slot) {
        for (auto& key_value : this->provider_list) {
            for (auto it = key_value.second.begin(); it != key_value.second.end(); ++it) {
                if (*it == signal_provider) {
                    key_value.second.erase(it);
                    break;
                }
            }
        }
    }
    else if (this->provider_list.count(slot)) {
        auto& signalProviders = this->provider_list[slot];
        for (auto it = signalProviders.begin(); it != signalProviders.end(); ++it) {
            if (*it == signalProviders) {
                signalProviders.erase(it);
                return;
            }
        }
    }
}

