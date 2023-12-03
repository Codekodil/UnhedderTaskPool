#ifndef TASKPOOL_H__
#define TASKPOOL_H__

#include <coroutine>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <vector>

namespace AsyncTask {

	class TaskPoolImpl;

	class TaskPool {

		std::shared_ptr<TaskPoolImpl> _impl;

	public:

		TaskPool(int workerThreads);
		~TaskPool();

		class AwaitableTask {

		protected:

			class task_data {
				bool _done = false;
				std::exception_ptr _exc = {};
				std::mutex _lock = {};
				std::vector<std::coroutine_handle<>> _handles = {};

				std::shared_ptr<TaskPoolImpl> _pool_impl;

			public:

				task_data();

				bool IsDone();
				void SetDone();
				void SetException(std::exception_ptr exc);
				void Rethrow();
				void AddSuspend(std::coroutine_handle<> h);
			};
			std::shared_ptr<task_data> _data;

		public:

			

			template <typename TTask, typename TData>
			class base_promise_type {

			protected:

				std::shared_ptr<TData> _data;

			public:
				TTask get_return_object() {
					_data = std::make_shared<TData>();
					TTask task{};
					task._data = _data;
					return std::move(task);
				}
				std::suspend_never initial_suspend() noexcept { return {}; }
				std::suspend_never final_suspend() noexcept { return {}; }
				void unhandled_exception() { _data->SetException(std::current_exception()); }
			};

			bool await_ready();
			void await_suspend(std::coroutine_handle<> h);

		};

		template <typename T>
		class TemplateTask : public AwaitableTask {

			struct value_task_data : public AwaitableTask::task_data {
				std::optional<T> _value = {};
			};

		public:

			class promise_type : public AwaitableTask::base_promise_type<TemplateTask<T>, value_task_data> {

			public:
				void return_value(T&& value) {
					AwaitableTask::base_promise_type<TemplateTask<T>, value_task_data>::_data->_value = std::move(value);
					AwaitableTask::base_promise_type<TemplateTask<T>, value_task_data>::_data->SetDone();
				}
			};

			T&& await_resume() { _data->Rethrow(); return std::move(static_cast<value_task_data*>(_data.get())->_value.value()); }

			T&& Join() {
				[this]() -> TemplateTask<void>{
					co_await *this;
				}().Join();
				return await_resume();
			}
			static TemplateTask<T> Run(std::function<T()> action) {
				auto result = std::make_shared<std::optional<T>>();
				co_await TemplateTask<void>::Run([result, action]() {
						*result = std::move(action());
					});
				co_return std::move(result->value());
			}
			static TemplateTask<T> FromResult(T value) {
				TemplateTask<T>::promise_type promise{};
				auto task = promise.get_return_object();
				promise.return_value(std::move(value));
				return std::move(task);
			}

		};

		template <>
		class TemplateTask<void> : public AwaitableTask {

		public:

			class promise_type : public AwaitableTask::base_promise_type<TemplateTask<void>, task_data> {

			public:
				void return_void() {
					_data->SetDone();
				}
			};

			void await_resume();

			void Join();
			static TemplateTask<void> Run(std::function<void()> action);
			static TemplateTask<void> CompletedTask();
			static TemplateTask<void> Delay(std::chrono::milliseconds ms);
		};

		class ThreadLinkedPool {
			std::function<void()> _unlink = [](){};
		public:
			ThreadLinkedPool(const ThreadLinkedPool&) = delete;
			ThreadLinkedPool(const ThreadLinkedPool&&) = delete;
			ThreadLinkedPool(std::shared_ptr<TaskPool> pool);
			~ThreadLinkedPool();
		};
	};

	template <typename T>
	using Task = TaskPool::TemplateTask<T>;

	using ThreadLink = TaskPool::ThreadLinkedPool;

}

#endif
