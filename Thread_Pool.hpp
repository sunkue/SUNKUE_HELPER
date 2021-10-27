#pragma once


#include <future>
#include <queue>

namespace THREAD_POOL {
	using namespace std;
	class Thread_Pool
	{
	public:
		static Thread_Pool& instance();

		Thread_Pool(const Thread_Pool&) = delete;
		Thread_Pool& operator=(const Thread_Pool&) = delete;

	public:
		template <class F, class... Args>
		future<typename invoke_result<F, Args...>::type>
			enqueue_job(F&& f, Args&&... args);

	private:
		Thread_Pool(size_t num_threads = thread::hardware_concurrency());
		~Thread_Pool();


	private:
		size_t _num_threads;
		vector<thread> _worker_threads;
		queue<function<void()>> _jobs;
		condition_variable _cv_job_q;
		mutex _m_job_q;

		bool stop_all = false;

		void WorkerThread();
	};

	/* --------------------------- */

	Thread_Pool& Thread_Pool::instance()
	{
		static Thread_Pool instance_;
		return instance_;
	}

	Thread_Pool::Thread_Pool(size_t num_threads)
		: _num_threads{ num_threads }
	{
		_worker_threads.reserve(_num_threads);
		for (size_t i = 0; i < _num_threads; i++) {
			_worker_threads.emplace_back([this]() { this->WorkerThread(); });
		}
	}

	void Thread_Pool::WorkerThread()
	{
		do {
			unique_lock<mutex> lock(_m_job_q);
			_cv_job_q.wait(lock, [this]() { return !this->_jobs.empty() || stop_all; });

			if (stop_all && this->_jobs.empty()) {
				return;
			}

			function<void()> job = move(_jobs.front());
			_jobs.pop();
			lock.unlock();

			job();
		} while (true);
	}

	Thread_Pool::~Thread_Pool()
	{
		stop_all = true;
		_cv_job_q.notify_all();

		for (auto& t : _worker_threads) {
			t.join();
		}
	}

	template <class F, class... Args>
	future<typename invoke_result<F, Args...>::type>
		Thread_Pool::enqueue_job(F&& f, Args&&... args)
	{
		if (stop_all) {
			throw runtime_error("Thread_Pool is stopped.");
		}

		using return_type = typename invoke_result<F, Args...>::type;

		auto job = make_shared<packaged_task<return_type()>>
			(bind(forward<F>(f), forward<Args>(args)...));

		future<return_type> job_result_future = job->get_future();
		{
			scoped_lock<mutex> lock(_m_job_q);
			_jobs.push([job]() { (*job)(); });
		}

		_cv_job_q.notify_one();
		return job_result_future;
	}
}