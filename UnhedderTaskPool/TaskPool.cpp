
#define CPP2_INCLUDE_STD         Yes

//=== Cpp2 type declarations ====================================================


#include "cpp2util.h"

#line 1 "TaskPool.cpp2"

#line 12 "TaskPool.cpp2"
namespace AsyncTask {

 class PoolLink;
  

#line 49 "TaskPool.cpp2"
 class TaskPoolImpl;
  

#line 129 "TaskPool.cpp2"
}


//=== Cpp2 type definitions and function declarations ===========================

#line 1 "TaskPool.cpp2"

#include "..\include\TaskPool.h"

#define Throw(exc) throw exc

using namespace AsyncTask;
using namespace std;

thread_local shared_ptr<TaskPoolImpl> _owner_pool = nullptr;
thread_local vector<shared_ptr<PoolLink>> _pool_links{};

#line 12 "TaskPool.cpp2"
namespace AsyncTask {

 class PoolLink {
  private: shared_ptr<TaskPoolImpl> _pool; 
  private: shared_ptr<TaskPool> _rootPool; 

  public: explicit PoolLink(cpp2::in<shared_ptr<TaskPoolImpl>> pool, cpp2::in<shared_ptr<TaskPool>> rootPool);

#line 23 "TaskPool.cpp2"
  public: auto Unlink() & -> void;

#line 27 "TaskPool.cpp2"
  public: [[nodiscard]] auto GetPool() const& -> shared_ptr<TaskPoolImpl>;
  public: [[nodiscard]] auto GetRootPool() const& -> shared_ptr<TaskPool>;

  public: [[nodiscard]] static auto TryGet() -> shared_ptr<PoolLink>;

#line 40 "TaskPool.cpp2"
  public: [[nodiscard]] static auto Get() -> shared_ptr<PoolLink>;
  public: PoolLink(PoolLink const&) = delete; /* No 'that' constructor, suppress copy */
  public: auto operator=(PoolLink const&) -> void = delete;


#line 47 "TaskPool.cpp2"
 };

 class TaskPoolImpl {
  private: bool _shutdown {false}; 

  private: mutex _lock {}; 
  private: vector<function<void()>> _pending_actions {}; 
  private: vector<jthread> _threads {}; 
  private: vector<binary_semaphore*> _waiting {}; 

  public: auto QueueAction(cpp2::in<function<void()>> action) & -> void;

#line 71 "TaskPool.cpp2"
  public: auto Quit() & -> void;

#line 84 "TaskPool.cpp2"
  private: [[nodiscard]] auto NextAction() & -> optional<function<void()>>;

#line 94 "TaskPool.cpp2"
  public: auto WorkerEntry() & -> void;

#line 114 "TaskPool.cpp2"
  public: auto MoveThread(jthread&& thread) & -> void;

#line 121 "TaskPool.cpp2"
  public: [[nodiscard]] static auto Local() -> shared_ptr<TaskPoolImpl>;
  public: TaskPoolImpl() = default;
  public: TaskPoolImpl(TaskPoolImpl const&) = delete; /* No 'that' constructor, suppress copy */
  public: auto operator=(TaskPoolImpl const&) -> void = delete;


#line 127 "TaskPool.cpp2"
 };

}

TaskPool::TaskPool(int workerThreads) {
	if(workerThreads <= 0)	
		throw invalid_argument("workerThreads must be at least 1");

	auto impl = _impl = make_shared<TaskPoolImpl>();

	for(int i = 0; i < workerThreads; i++) {
		jthread worker([impl]() {
			auto implCopy = _owner_pool = impl;
			implCopy->WorkerEntry();
			});
		_impl->MoveThread(move(worker));
	}
}

TaskPool::~TaskPool() {
	_impl->Quit();
}

bool TaskPool::QueuePoolTask::await_ready() { return false; }
void TaskPool::QueuePoolTask::await_suspend(coroutine_handle<> h) {
	auto pool_impl = TaskPoolImpl::Local();
	pool_impl->QueueAction([h]() {h.resume(); });
}
void TaskPool::QueuePoolTask::await_resume() {}

TaskPool::AwaitableTask::task_data::task_data() {
	_pool_impl = TaskPoolImpl::Local();
}

TaskPool::AwaitableTask::task_data::~task_data() {
	if(!IsDone())
	{
		try {
			throw UnfinishedTaskException();
		}
		catch (...) {
			SetException(std::current_exception());
		}
	}
}

bool TaskPool::AwaitableTask::task_data::IsDone() {
	return _done;
}

void TaskPool::AwaitableTask::task_data::SetDone() {
	_done = true;
	{
		const lock_guard<mutex> lock(_lock);
		for (auto& h : _handles)
			_pool_impl->QueueAction([h]() {h.resume(); });
		_handles.clear();
	}
}

void TaskPool::AwaitableTask::task_data::SetException(exception_ptr exc) {
	if(exc) {
		_exc = exc;
		SetDone();
	}
}

void TaskPool::AwaitableTask::task_data::Rethrow() {
	auto exc = _exc;
	if(exc)
		rethrow_exception(exc);
}


void TaskPool::AwaitableTask::task_data::AddSuspend(coroutine_handle<> h) {
	if (!_done) {
		const lock_guard<mutex> lock(_lock);
		if (!_done) {
			_handles.push_back(h);
			return;
		}
	}
	h.resume();
}

bool TaskPool::AwaitableTask::await_ready() {
	if(_data->IsDone()) {
		_data->Rethrow();
		return true;
	}
	return false;
}

void TaskPool::AwaitableTask::await_suspend(coroutine_handle<> h) {
	_data->AddSuspend(h);
}

void TaskPool::TemplateTask<void>::await_resume() {
	_data->Rethrow();
}

void TaskPool::TemplateTask<void>::Join() {
	if(_owner_pool)
		throw runtime_error("cannot join inside a thread owned by a task pool");

	binary_semaphore awaiter{0};
	exception_ptr exc{};

	[&awaiter, &exc, this]() -> TaskPool::TemplateTask<void>{
		auto& scopeAwaiter = awaiter;
		auto& scopeExc = exc;
		try {
			co_await *this;
		}
		catch(...) {
			scopeExc = std::current_exception();
		}
		scopeAwaiter.release();
	}();

	awaiter.acquire();
	if(exc)
		rethrow_exception(exc);
}

TaskPool::TemplateTask<void> TaskPool::TemplateTask<void>::Run(function<void()> action) {
	auto pool = TaskPoolImpl::Local();
	TemplateTask<void>::promise_type promise{};
	auto task = promise.get_return_object();
	pool->QueueAction([promise, action]() {
			auto promiseCopy = promise;
			try {
				action();
			}
			catch(...) {
				promiseCopy.unhandled_exception();
			}
			promiseCopy.return_void();
		});
	return move(task);
}

TaskPool::TemplateTask<void> TaskPool::TemplateTask<void>::CompletedTask() {
	TemplateTask<void>::promise_type promise{};
	auto task = promise.get_return_object();
	promise.return_void();
	return move(task);
}

TaskPool::TemplateTask<void> TaskPool::TemplateTask<void>::Delay(chrono::milliseconds ms) {
	auto desiredEnd = chrono::system_clock::now() + ms;
	TemplateTask<void>::promise_type promise{};
	auto task = promise.get_return_object();
	thread([promise, desiredEnd]() {
			auto promiseCopy = promise;
			auto toWait = chrono::duration_cast<chrono::milliseconds>(desiredEnd - chrono::system_clock::now());
			if(toWait > 0ms)
				this_thread::sleep_for(toWait);
			promiseCopy.return_void();
		}).detach();
	return move(task);
}

TaskPool::QueuePoolTask TaskPool::TemplateTask<void>::ContinueInPool() { return TaskPool::QueuePoolTask(); }

TaskPool::ThreadLinkedPool::ThreadLinkedPool(ThreadLinkedPool&& that) {
	_unlink = exchange(that._unlink, [](){});
}

TaskPool::ThreadLinkedPool& TaskPool::ThreadLinkedPool::operator=(ThreadLinkedPool&& that) {
	exchange(_unlink, exchange(that._unlink, [](){}))();
	return *this;
}

TaskPool::ThreadLinkedPool::ThreadLinkedPool(shared_ptr<TaskPool> pool) {
	if(!pool)
		throw invalid_argument("pool is nullptr");
	
	if(_owner_pool)
		throw runtime_error("thread is owned by a task pool");

	auto link = make_shared<PoolLink>(pool->_impl, pool);

	_pool_links.push_back(link);

	_unlink = [link](){link->Unlink();};
}

TaskPool::ThreadLinkedPool::~ThreadLinkedPool() {
	_unlink();
}

function<void()> TaskPool::ThreadLinkedPool::LinkedThreadAction(function<void()> action) {
	auto pool = PoolLink::Get()->GetRootPool();
	return [pool, action](){
		const ThreadLink link(pool);
		action();
	};
}


//=== Cpp2 function definitions =================================================

#line 1 "TaskPool.cpp2"

#line 12 "TaskPool.cpp2"
namespace AsyncTask {

#line 18 "TaskPool.cpp2"
  PoolLink::PoolLink(cpp2::in<shared_ptr<TaskPoolImpl>> pool, cpp2::in<shared_ptr<TaskPool>> rootPool)
   : _pool{ pool }
   , _rootPool{ rootPool }{

#line 21 "TaskPool.cpp2"
  }

  auto PoolLink::Unlink() & -> void{
   _pool = nullptr;
   _rootPool = nullptr;
  }
  [[nodiscard]] auto PoolLink::GetPool() const& -> shared_ptr<TaskPoolImpl> { return _pool;  }
  [[nodiscard]] auto PoolLink::GetRootPool() const& -> shared_ptr<TaskPool> { return _rootPool;  }

  [[nodiscard]] auto PoolLink::TryGet() -> shared_ptr<PoolLink>{
   while( cpp2::cmp_greater(CPP2_UFCS_0(size, _pool_links),cpp2::as_<size_t, 0>()) ) {
    auto link {CPP2_UFCS_0(back, _pool_links)}; 
    if (CPP2_UFCS_0(GetPool, (*cpp2::assert_not_null(link)))) {
     return link; 
    }
    CPP2_UFCS_0(pop_back, _pool_links);
   }
   return nullptr; 
  }
  [[nodiscard]] auto PoolLink::Get() -> shared_ptr<PoolLink>{
   auto link {TryGet()}; 
   if (link) {
    return link; 
   }
   Throw(runtime_error("thread has no linked pool"));
  }

#line 57 "TaskPool.cpp2"
  auto TaskPoolImpl::QueueAction(cpp2::in<function<void()>> action) & -> void{
   lock_guard<mutex> auto_58_4 {_lock}; 
   if (_shutdown) {
    return ; 
   }

   CPP2_UFCS(push_back, _pending_actions, action);
   if (CPP2_UFCS_0(size, _waiting) == cpp2::as_<size_t, 0>()) {
    return ; 
   }
   CPP2_UFCS_0(release, (*cpp2::assert_not_null(CPP2_UFCS_0(front, _waiting))));
   CPP2_UFCS(erase, _waiting, CPP2_UFCS_0(begin, _waiting));
  }

  auto TaskPoolImpl::Quit() & -> void{
   _shutdown = true;
   {
    lock_guard<mutex> auto_74_5 {_lock}; 
    for ( auto const& s : _waiting ) {
     CPP2_UFCS_0(release, (*cpp2::assert_not_null(s)));
    }
    CPP2_UFCS_0(clear, _waiting);
    CPP2_UFCS_0(clear, _pending_actions);
   }
   CPP2_UFCS_0(clear, _threads);//_threads will not be accessed after shutdown and a finished mutex lock
  }

  [[nodiscard]] auto TaskPoolImpl::NextAction() & -> optional<function<void()>>{
   lock_guard<mutex> auto_85_4 {_lock}; 
   if (CPP2_UFCS_0(size, _pending_actions) == cpp2::as_<size_t, 0>()) {
    return {  }; 
   }
   auto result {CPP2_ASSERT_IN_BOUNDS(_pending_actions, 0)}; 
   CPP2_UFCS(erase, _pending_actions, CPP2_UFCS_0(begin, _pending_actions));
   return result; 
  }

  auto TaskPoolImpl::WorkerEntry() & -> void{
   while( !(_shutdown) ) {
    auto next {NextAction()}; 
    if (next) {
     CPP2_UFCS_0(value, next)();
    }
    else {
     binary_semaphore waiting {0}; 
     {
      lock_guard<mutex> auto_103_7 {_lock}; 
      if (_shutdown) {
       return ; 
      }
      CPP2_UFCS(push_back, _waiting, &waiting);
     }
     CPP2_UFCS_0(acquire, waiting);
    }
   }
  }

  auto TaskPoolImpl::MoveThread(jthread&& thread) & -> void{
   lock_guard<mutex> auto_115_4 {_lock}; 
   if (!(_shutdown)) {
    CPP2_UFCS(push_back, _threads, std::move(thread));
   }
  }

  [[nodiscard]] auto TaskPoolImpl::Local() -> shared_ptr<TaskPoolImpl>{
   if (_owner_pool) {
    return _owner_pool; 
   }
   return CPP2_UFCS_0(GetPool, (*cpp2::assert_not_null(PoolLink::Get()))); 
  }

#line 129 "TaskPool.cpp2"
}

