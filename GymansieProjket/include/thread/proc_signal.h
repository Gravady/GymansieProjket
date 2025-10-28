#ifndef PROC_SIGNAL_H
#define PROC_SIGNAL_H

//Worker and process communication medium

#include <atomic>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <utility>
#include <memory>
#include <optional>
#include <future>
#include <type_traits>
#include <random>

#include <QObject>
#include <QMutex>
#include <QMutexLocker>
#include <QMetaObject>
#include <QThread>

#include <include/thread/dump.h>

//Signal manager for assigning worker handlers and workers to specific threading task
//TODO: more info about this needs to be known
//TODO: More memory saftey needs to be implemented
//TODO: More error handling needs to be implemented

//class Signaler;
//class Controller;
//class SignalProvider;

//Invoke functionality from project
// TODO:Update and change this
//https://github.com/polygamma/Qt-Multithreading/blob/master/src/Magic.h
template<typename Callable, typename ...Args>
inline bool invokeInContext(
	QObject* const context,
	const Qt::ConnectionType connectionType,
	const Callable callable,
    Args &&... args) {

    if (connectionType == Qt::DirectConnection || connectionType == Qt::BlockingQueuedConnection ||
        (connectionType == Qt::AutoConnection && QThread::currentThread() == context->thread())) {

        return QMetaObject::invokeMethod(
            context, [&]() -> void { std::invoke(callable, std::forward<Args>(args)...); }, connectionType
        );
    }
    else {

        return QMetaObject::invokeMethod(
            context,
            [=, ...args = std::forward<Args>(args)]() mutable -> void {
                std::invoke(callable, std::forward<Args>(args)...);
            },
            connectionType
        );
    }
}

template <typename Ret, typename ... Args>
Ret(*getFunctionPointer(Ret(*func)(Args...)))(Args...) {
	return func;
}

template <typename Ret, typename ...Args>
inline Ret (*functionToPointer(Ret(*func)(Args...)))(Args...){
	return func;
}

template <typename Ret,typename Callable, typename ... Args>
inline Ret (*lambdaToFunctionPointer(Callable&& lambda))(Args...){
	return +lambda;
}

//HACK:CRTP 
//Functionality like stack dumping values of the signal, etc
//NOTE:Dosent need to be implemneted right now just in the future
//HACK:Shared mutex locker should be implemented
template <typename Derived, typename = std::enable_if_t<std::is_constructible_v<Derived>>
>
class SignalBaseFunc
{
public:
	Derived& returnDerived() {
		return *static_cast<Derived*>(this);
	}

	SignalBaseFunc() {
		returnDerived();
	}

	virtual ~SignalBaseFunc() = default;
private:

public:
};

//Purely virtual function
//HACK: Since its a remake this version of SignalProvider being purely virtual may change
class SignalProvider
{
public:
	explicit SignalProvider() = default;
	virtual ~SignalProvider() = default;
private:
	inline QMutex* localMutex() noexcept { return &local_mutex; }
	QMutex local_mutex;
public:
	//Invokepipeblock declared in CommunicationProvider
	virtual inline void invokePipeBlock(SlotProvider* provider, bool block) noexcept;
	virtual void disconnectLocalSignal(void* slot, SlotProvider* provider);
	virtual std::unordered_set<SlotProvider*> getConnectedSlotProviders() noexcept;
};

class ProviderCommuniction;

//HACK: SignalPromise is a singular instace of a promise and now not multiple promises coupled together
//NOTE: Since no signal should leave SignalProvider to SlotProvider without a promise any 
//checking on SlotProvider for a promise is not required
template <typename RetCallback = void*, typename ... Args>
class SignalPromise
{
	public:
		explicit SignalPromise(SignalProvider* provider = nullptr){
			owner_provider(provider);
		}
		virtual ~SignalPromise() {
			disconnectAllSlots();
		}
	private:
		std::promise<RetCallback> promise_callback;
		std::optional<std::future<RetCallback>> future_callback;
		Ret(*void)(Args...) task = nullptr;
		std::atomic_bool promise_set;
		QMutex promise_mutex;
		SingalPromise owner_provider;
public:
		template <typename RetCallback, typename ...Args, typename CallbackReciver>
		static inline void addTask(
			RetCallback(* const task)(Args...),
			Args&&...,
			std::promise<RetCallback> promise_object,
			CallbackReciver&& reciever
		);
		
		inline void setPromiseValue(const RetCallback& value);
		//SlotProvider can implement this virtual class
		virtual std::optional<std::future<RetCallback>> recieveFuture();

		std::future<RetCallback> getFuture();
		inline void resetPromise() noexcept;
		[[nodiscard]] bool hasPromise() noexcept;
		//SlotProvider or other will implement this
		template <typename ...CallArgs>
		virtual void executePromise(CallArgs&& ..callargs) noexcept;
};

//Static registry of slot providers
//Singleton class
class SlotProviderRegistry
{
	public:
		static SlotProviderRegistry& getInstance() {
			static SlotProviderRegistry instance;
			return instance;
		}

		SlotProviderRegistry(SlotProvider* slotprovider = nullptr) {
			registerSlotProvider(slotprovider);
		}

		SlotProviderRegistry(const SlotProviderRegistry&) = delete;
		SlotProviderRegistry& operator=(const SlotProviderRegistry&) = delete;
		SlotProviderRegistry(SlotProviderRegistry&&) = delete;
		SlotProviderRegistry& operator=(SlotProviderRegistry&&) = delete;

	private:
		std::unordered_map<const char*, SlotProvider*> slot_provider_id_map;
		std::unordered_map<void* const, SlotProvider*> slot_provider_reverse_map;
		static QMutex registry_mutex;
		friend class SlotProvider;
	public:
		static inline bool registerSlotProvider(SlotProvider* provider) noexcept;
		static inline bool unregisterSlotProvider(const char* provider_id) noexcept;
		static inline const char* generateSlotProviderId(SlotProvider* provider) noexcept;
		static inline SlotProvider* getSlotProviderById(const char* provider_id) noexcept; //hex id because why not
		static inline std::unordered_map<void* const, SlotProvider*> getSlotProviderMap() noexcept;
};

class ProviderCommunication;

//TODO: Needs more support for SignalPromise and ProviderCommunication but otherwise its good
class SlotProvider : public QObject
{
public:
	explicit SlotProvider(QObject* parent = nullptr) : QObject(parent) {
		SlotProviderRegistry(this);
	};
	virtual ~SlotProvider() noexcept override = default;
private:
	QMutex* mutex;
	std::unordered_map<void*, std::vector<SignalProvider>> provider_list;
	std::unordered_map<QThread*, std::vector<SignalProvider*>> signal_senders;
	SlotProviderRegistry* registry;
	bool pipe_block = false;
	friend class ProviderCommunication;
public:
	inline SignalProvider* signalSender() noexcept;
	bool checkSlotAvaliability(
		std::unordered_map<QThread*,
		std::vector<SignalProvider*>> slots_
	);

	template <typename Callable, typename ...Args>
	inline bool callSlot(
		SignalProvider* signal,
		Callable&& callable_object,
		Args&&... args,
		std::optional<std::string> callable_context,
		SignalPromise<void>* promise
	);

	inline void registerConnection(
		void* const slot,
		SignalProvider* const signalprovider
	);
};

//Handles the promise between the SignalProvider and SlotProvider
class ProviderCommunication : public QObject
{
	Q_OBJECT
public:
	explicit ProviderCommunication(
		const SlotProvider* slotProvider,
		const SignalProvider* signalProvider)
		: slot_provider(nullptr), signal_provider(nullptr) {}

	explicit ProviderCommunication(
		const SlotProvider* slotProvider,
		const SignalProvider* signalProvider,
		const SlotProviderRegistry* slotProviderRegistry)
		: slot_provider(nullptr), signal_provider(nullptr){}

private:
	QObject* object;
	QMutex pipe_mutex;
	static std::vector<ProviderCommunication*> provider_list;
	static std::unordered_map<SignalProvider, SlotProvider> communications_list;
	SlotProvider* slot_provider;
	SignalProvider* signal_provider;
public:
	template <typename Ret, typename ... Args, typename C>
	static inline bool disconnectCommunication(
		Ret(* const slot)(Args...),
		SignalProvider* signal_provider,
		SlotProvider* provider) noexcept;

	static inline bool disconnectCommunication(
		void* const slot,
		SignalProvider* signal_provider,
		SlotProvider provider
	) noexcept;

	inline void mutexLockPipe(QMutex* mutex) noexcept;
	inline void mutexUnlockPipe(QMutex* mutex) noexcept;

	static void dropCommunicationList();

	inline void registerSignalProvider(SignalProvider* signalProvider) noexcept;
	inline void unregisterSignalProvider(SignalProvider* signalProvider) noexcept;

	//Forwards promise to SlotProvider
	template <typename RetCallback>
	inline void recieveSignalPromise(SignalPromise<RetCallback>* promise) noexcept;

	template <typename RetCallback, typename ...Args>
	const char* registerPromiseForSlot(
		SlotProvider* slotProvider,
		RetCallback(* const task)(Args...),
		Qt::ConnectionType connectionType = Qt::AutoConnection
	) noexcept;
};

//TODO/NOTE:This hasnt been properly looked on and should be 
template <typename Ret, typename ...Args, typename C>
class Signal final : public ProviderCommunication, public QObject
{
	Q_OBJECT
public:
	explicit Signal<Args...>(Ret(C::* const)(Args...),
		const QObject* parent = nullptr),
		ProviderCommunication* communication,
		: proc(nullptr){};

	template <typename ...Args>
	explicit Signal<Args...>(QObject* parent = nullptr);

	//TODO:Deconstructor needs to be worked on
	explicit ~Signal() {
		disconnectAllSignalStreams();
	}

private:
	Signal* proc;
	bool proc_connected;
	static inline QMutex globalMutex = QMutex();
	std::unordered_map<void* std::vector<ProviderCommunication*>> communication_slots;

public:
	inline SignalProvider* getLocalSignalProvider() noexcept;
	inline SlotProvider* getLocalSlotProvider() noexcept;

	template <typename Ret, typename ... Args, typename C>
	static inline void disconnectCommunicationLine(
		Ret(C::* const slot)(Args...),
		ProviderCommunication* communication,
		std::unordered_map<void*, std::vector<ProviderCommunication*>> communication_slots;
	);

	template <typename Ret, typename ... Args, typename C>
	static inline void connectProvider(
		Ret(C::* const slot)(Args...),
		ProviderCommunication* communication,
		std::unordered_map<void*, std::vector<ProviderCommunication*>> communication_slots;

	virtual bool disconnectAllSignalStreams(std::unordered_map<slot_queue*, QObject> slot_map_ = nullptr);

	[[nodiscard]] bool sendSignal() noexcept;

	teamplate<typename Ret, typename ... Args, typename C>
	inline void registerSlotConnection(
		Ret(C::* const slot)(Args...),
		SignalProvider* signalProvider
		std::list<std::unordered_map<std::function<auto(C*)->void>>, uint8_t> slot_list);

	inline void killProviderCommunication(ProviderCommunication* signalProvider_);
	inline void mutexLockLocalSignal(QMutex* mutex,) noexcept;

	inline auto forwardToSlot();
	inline auto forwardToSignal();
	inline auto forwardPromise(QObject* parent = nullptr);
	
	std::unordered_map<void*, std::vector<ProviderCommunication*>> getCommunicationSlots() noexcept;
	std::unordered_set<SlotProvider*> getConnectedSlotProviders() noexcept;
	std::unordered_set<SignalProvider*> getConnectedSignalProviders() noexcept;

	inline bool connectionExists(
		SignalProvider* signalProvider,
		SlotProvider* slotProvider
	) noexcept;
	inline bool connectionExists(CommunicationProvider* communication) noexcept;

	//NOTE: Needs to be added to slotregistry aswell
	template <typename Ret, typename C, typename ...SlotArgs> 
	inline void registerSlot(
		C* const context, Ret(C::* const f)(SlotArgs...),
		const Qt::ConnectionType connectionType);
	
	template <typename Slot>
	inline void registerSlot(
		Slot& slot_type,
		const char* slot_provider_registry_id = nullptr);
};

#endif