#include "thread_pool.hpp"
#include <iostream>
namespace GearX {
	ThreadPool::ThreadPool(size_t threads)
		: stop(false), paused(false), activeThreads(0) {
		if (threads == 0) threads = 4;

		for (size_t i = 0; i < threads; ++i) {
			workers.emplace_back([this] {
				while (true) {
					std::function<void()> task;

					{
						std::unique_lock<std::mutex> lock(this->queueMutex);

						this->condition.wait(lock, [this] {
							return this->stop || (!this->tasks.empty() && !this->paused);
							});

						if (this->stop && this->tasks.empty())
							return;

						if (this->paused)
							continue;

						task = std::move(this->tasks.front());
						this->tasks.pop();
						++this->activeThreads;
					}

					try {
						task();
					}
					catch (const std::exception& e) {
						std::cerr << "Task exception: " << e.what() << std::endl;
					}

					{
						std::unique_lock<std::mutex> lock(this->queueMutex);
						--this->activeThreads;
						this->condition.notify_all();
					}
				}
				});
		}
	}

	ThreadPool::~ThreadPool() {
		{
			std::unique_lock<std::mutex> lock(queueMutex);
			stop = true;
		}
		condition.notify_all();
		for (std::thread& worker : workers) {
			worker.join();
		}
	}

	void ThreadPool::pause() {
		{
			std::unique_lock<std::mutex> lock(queueMutex);
			paused = true;
		}
		condition.notify_all();
	}

	void ThreadPool::resume() {
		{
			std::unique_lock<std::mutex> lock(queueMutex);
			paused = false;
		}
		condition.notify_all();
	}

	void ThreadPool::waitForCompletion() {
		std::unique_lock<std::mutex> lock(queueMutex);
		condition.wait(lock, [this] {
			return tasks.empty() && activeThreads == 0;
			});
	}

	size_t ThreadPool::getQueueSize() const {
		std::unique_lock<std::mutex> lock(queueMutex);
		return tasks.size();
	}

	size_t ThreadPool::getActiveThreadCount() const {
		std::unique_lock<std::mutex> lock(queueMutex);
		return activeThreads;
	}

	bool ThreadPool::isPaused() const {
		std::unique_lock<std::mutex> lock(queueMutex);
		return paused;
	}
}