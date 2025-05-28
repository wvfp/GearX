#pragma once
#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <atomic>
namespace GearX {
	class ThreadPool {
	public:
		explicit ThreadPool(size_t threads = std::thread::hardware_concurrency());
		~ThreadPool();

		// 禁用拷贝构造和赋值
		ThreadPool(const ThreadPool&) = delete;
		ThreadPool& operator=(const ThreadPool&) = delete;

		// 任务提交
		template<class F, class... Args>
		auto enqueue(F&& f, Args&&... args)
			-> std::future<typename std::result_of<F(Args...)>::type>;

		// 控制接口
		void pause();
		void resume();
		void waitForCompletion();

		// 状态查询
		size_t getQueueSize() const;
		size_t getActiveThreadCount() const;
		bool isPaused() const;

	private:
		std::vector<std::thread> workers;
		std::queue<std::function<void()>> tasks;

		mutable std::mutex queueMutex;
		std::condition_variable condition;

		std::atomic<bool> stop;
		std::atomic<bool> paused;
		std::atomic<size_t> activeThreads;
	};

	// 模板函数需要在头文件中实现
	template<class F, class... Args>
	auto ThreadPool::enqueue(F&& f, Args&&... args)
		-> std::future<typename std::result_of<F(Args...)>::type> {
		using return_type = typename std::result_of<F(Args...)>::type;

		if (stop)
			throw std::runtime_error("enqueue on stopped ThreadPool");

		auto task = std::make_shared< std::packaged_task<return_type()> >(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...)
		);

		std::future<return_type> res = task->get_future();

		{
			std::unique_lock<std::mutex> lock(queueMutex);
			tasks.emplace([task]() { (*task)(); });
		}

		condition.notify_one();
		return res;
	}
}