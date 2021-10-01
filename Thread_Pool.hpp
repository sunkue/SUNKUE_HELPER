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
		size_t num_threads_;
		vector<thread> worker_threads_;
		queue<function<void()>> jobs_;
		condition_variable cv_job_q_;
		mutex m_job_q_;

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
		: num_threads_{ num_threads }
	{
		worker_threads_.reserve(num_threads_);
		for (size_t i = 0; i < num_threads_; i++) {
			worker_threads_.emplace_back([this]() { this->WorkerThread(); });
		}
	}

	void Thread_Pool::WorkerThread()
	{
		do {
			unique_lock<mutex> lock(m_job_q_);
			cv_job_q_.wait(lock, [this]() { return !this->jobs_.empty() || stop_all; });

			if (stop_all && this->jobs_.empty()) {
				return;
			}

			function<void()> job = move(jobs_.front());
			jobs_.pop();
			lock.unlock();

			job();
		} while (true);
	}

	Thread_Pool::~Thread_Pool()
	{
		stop_all = true;
		cv_job_q_.notify_all();

		for (auto& t : worker_threads_) {
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
			scoped_lock<mutex> lock(m_job_q_);
			jobs_.push([job]() { (*job)(); });
		}

		cv_job_q_.notify_one();
		return job_result_future;
	}
}