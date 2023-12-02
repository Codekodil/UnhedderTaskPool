
#define CPP2_INCLUDE_STD         Yes

//=== Cpp2 type declarations ====================================================


#include "cpp2util.h"

#line 1 "TaskPool.cpp2"

#line 13 "TaskPool.cpp2"
namespace AsyncTask {

 class PoolLink;
  

#line 24 "TaskPool.cpp2"
 class TaskPoolImpl;
  

#line 112 "TaskPool.cpp2"
}


//=== Cpp2 type definitions and function declarations ===========================

#line 1 "TaskPool.cpp2"

#define TASKPOOL_CPP2__
#include "TaskPool.h"

#define Throw(exc) throw exc

using namespace AsyncTask;
using namespace std;

thread_local shared_ptr<TaskPoolImpl> _owner_pool = nullptr;
thread_local vector<shared_ptr<PoolLink>> _pool_links{};

#line 13 "TaskPool.cpp2"
namespace AsyncTask {

 class PoolLink {
  private: shared_ptr<TaskPoolImpl> _pool; 

  public: explicit PoolLink(cpp2::in<shared_ptr<TaskPoolImpl>> pool);
#line 18 "TaskPool.cpp2"
  public: auto operator=(cpp2::in<shared_ptr<TaskPoolImpl>> pool) -> PoolLink& ;

  public: auto Unlink() & -> void;
  public: [[nodiscard]] auto GetPool() const& -> shared_ptr<TaskPoolImpl>;
  public: PoolLink(PoolLink const&) = delete; /* No 'that' constructor, suppress copy */
  public: auto operator=(PoolLink const&) -> void = delete;

#line 22 "TaskPool.cpp2"
 };

 class TaskPoolImpl {
  private: bool _shutdown {false}; 

  private: mutex _lock {}; 
  private: vector<function<void()>> _pending_actions {}; 
  private: vector<thread> _threads {}; 
  private: vector<binary_semaphore*> _waiting {}; 

  public: auto QueueAction(cpp2::in<function<void()>> action) & -> void;

#line 46 "TaskPool.cpp2"
  public: auto Quit() & -> void;

#line 56 "TaskPool.cpp2"
  private: [[nodiscard]] auto NextAction() & -> optional<function<void()>>;

#line 66 "TaskPool.cpp2"
  public: auto WorkerEntry() & -> void;

#line 85 "TaskPool.cpp2"
  public: ~TaskPoolImpl() noexcept;

#line 91 "TaskPool.cpp2"
  public: auto MoveThread(thread&& thread) & -> void;

#line 95 "TaskPool.cpp2"
  public: [[nodiscard]] static auto Local() -> shared_ptr<TaskPoolImpl>;
  public: TaskPoolImpl() = default;
  public: TaskPoolImpl(TaskPoolImpl const&) = delete; /* No 'that' constructor, suppress copy */
  public: auto operator=(TaskPoolImpl const&) -> void = delete;


#line 110 "TaskPool.cpp2"
 };

}

TaskPool::TaskPool(int workerThreads) {
	if(workerThreads <= 0)	
		throw invalid_argument("workerThreads must be at least 1");

	auto impl = _impl = make_shared<TaskPoolImpl>();

	for(int i = 0; i < workerThreads; i++) {
		thread worker([impl]() {
			auto implCopy = _owner_pool = impl;
			implCopy->WorkerEntry();
			});
		_impl->MoveThread(move(worker));
	}
}

TaskPool::~TaskPool() {
	_impl->Quit();
}

TaskPool::AwaitableTask::task_data::task_data() {
	_pool_impl = TaskPoolImpl::Local();
}

bool TaskPool::AwaitableTask::task_data::IsDone() {
	return _done;
}

void TaskPool::AwaitableTask::task_data::SetDone() {
	{
		const lock_guard<mutex> lock(_lock);
		for (auto& h : _handles)
			_pool_impl->QueueAction([h]() {h.resume(); });
		_handles.clear();
	}
	_done = true;
}

void TaskPool::AwaitableTask::task_data::AddSuspend(coroutine_handle<> h) {
	if (!_done) {
		const lock_guard<mutex> lock(_lock);
		_handles.push_back(h);
		return;
	}
	h.resume();
}

bool TaskPool::AwaitableTask::await_ready() {
	return _data->IsDone();
}

void TaskPool::AwaitableTask::await_suspend(coroutine_handle<> h) {
	_data->AddSuspend(h);
}

void TaskPool::TemplateTask<void>::Join() {
	if(_owner_pool)
		throw runtime_error("cannot join inside a thread owned by a task pool");

	binary_semaphore awaiter{0};

	[&awaiter, this]() -> TaskPool::TemplateTask<void>{
		co_await *this;
		awaiter.release();
	}();

	awaiter.acquire();
}

TaskPool::TemplateTask<void> TaskPool::TemplateTask<void>::Run(function<void()> action) {
	auto pool = TaskPoolImpl::Local();
	TemplateTask<void>::promise_type promise{};
	auto task = promise.get_return_object();
	pool->QueueAction([promise, action]() {
			auto promiseCopy = promise;
			action();
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
	auto desiredEnd = std::chrono::system_clock::now() + ms;
	TemplateTask<void>::promise_type promise{};
	auto task = promise.get_return_object();
	thread([promise, desiredEnd]() {
			auto promiseCopy = promise;
			auto toWait = chrono::duration_cast<chrono::milliseconds>(desiredEnd - std::chrono::system_clock::now());
			if(toWait > 0ms)
				this_thread::sleep_for(toWait);
			promiseCopy.return_void();
		}).detach();
	return move(task);
}


TaskPool::ThreadLinkedPool::ThreadLinkedPool(shared_ptr<TaskPool> pool) {
	if(!pool)
		throw invalid_argument("pool is nullptr");
	
	if(_owner_pool)
		throw runtime_error("thread is owned by a task pool");

	auto link = make_shared<PoolLink>(pool->_impl);

	_pool_links.push_back(link);

	_unlink = [link, pool](){link->Unlink();};
}

TaskPool::ThreadLinkedPool::~ThreadLinkedPool() {
	_unlink();
}


//=== Cpp2 function definitions =================================================

#line 1 "TaskPool.cpp2"

#line 13 "TaskPool.cpp2"
namespace AsyncTask {

#line 18 "TaskPool.cpp2"
  PoolLink::PoolLink(cpp2::in<shared_ptr<TaskPoolImpl>> pool)
                                                          : _pool{ pool } {  }
#line 18 "TaskPool.cpp2"
  auto PoolLink::operator=(cpp2::in<shared_ptr<TaskPoolImpl>> pool) -> PoolLink&  { 
                                                          _pool = pool;
                                                          return *this;  }

#line 20 "TaskPool.cpp2"
  auto PoolLink::Unlink() & -> void { _pool = nullptr;  }
  [[nodiscard]] auto PoolLink::GetPool() const& -> shared_ptr<TaskPoolImpl> { return _pool;  }

#line 32 "TaskPool.cpp2"
  auto TaskPoolImpl::QueueAction(cpp2::in<function<void()>> action) & -> void{
   lock_guard<mutex> auto_33_4 {_lock}; 
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
   lock_guard<mutex> auto_48_4 {_lock}; 
   for ( auto const& s : _waiting ) {
    CPP2_UFCS_0(release, (*cpp2::assert_not_null(s)));
   }
   CPP2_UFCS_0(clear, _waiting);
   CPP2_UFCS_0(clear, _pending_actions);
  }

  [[nodiscard]] auto TaskPoolImpl::NextAction() & -> optional<function<void()>>{
   lock_guard<mutex> auto_57_4 {_lock}; 
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
      lock_guard<mutex> auto_75_7 {_lock}; 
      if (_shutdown) {
       return ; 
      }
      CPP2_UFCS(push_back, _waiting, &waiting);
     }
     CPP2_UFCS_0(acquire, waiting);
    }
   }
  }
  TaskPoolImpl::~TaskPoolImpl() noexcept{
   for ( auto& t : _threads ) {
    CPP2_UFCS_0(detach, t);
   }
  }

  auto TaskPoolImpl::MoveThread(thread&& thread) & -> void{
   CPP2_UFCS(push_back, _threads, std::move(thread));
  }

  [[nodiscard]] auto TaskPoolImpl::Local() -> shared_ptr<TaskPoolImpl>{
   if (_owner_pool) {
    return _owner_pool; 
   }

   while( cpp2::cmp_greater(CPP2_UFCS_0(size, _pool_links),cpp2::as_<size_t, 0>()) ) {
    auto pool {CPP2_UFCS_0(GetPool, (*cpp2::assert_not_null(CPP2_UFCS_0(back, _pool_links))))}; 
    if (pool) {
     return pool; 
    }
    CPP2_UFCS_0(pop_back, _pool_links);
   }

   Throw(runtime_error("thread has no linked pool"));
  }

#line 112 "TaskPool.cpp2"
}

