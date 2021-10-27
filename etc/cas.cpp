#include <iostream>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <chrono>
#include <algorithm>
#include <ranges>

using namespace std;

int MAX_THREADS = thread::hardware_concurrency();

volatile int sum = 0;

/////////////////////////////////////////////////////////////

class CAS
{
	using Ty = bool;
public:
	void lock()
	{
		Ty expected = 0;
		const Ty desired = 1;

		while (false == is_locked.compare_exchange_strong(expected, desired))
		{
			expected = 0;
			this_thread::sleep_for(1ns); // 중요함
		}
	}

	void unlock()
	{
		is_locked.store(0);
	}

private:
	volatile atomic<Ty> is_locked{ 0 };
};

/////////////////////////////////////////////////////////////

thread_local static int tid;
class Bakery /* array 로 바꾸면 오류가 줄어듭니다. */
{
public:
	void lock() {
		const auto i = tid;
		flags[i] = true;
		const auto la_i = labels[i] = *ranges::max_element(labels) + 1;

		for (int k = -1; ranges::any_of(labels,
			[&](const size_t la_k) {
				k++;
				return true
					&& this->flags[k]
					&& ((la_k < la_i) || ((la_k == la_i) && (k < i)));
			}); k = -1)
			;;;
	}

	void unlock()
	{
		flags[tid] = false;
	}

	void reset(int max_thread)
	{
		max = max_thread + 1;
		flags.assign(max, false);
		labels.assign(max, 0);
	}

private:
	std::vector<bool> flags;
	std::vector<size_t> labels;
	int max = 0;
};



/////////////////////////////////////////////////////////////

mutex mut;
Bakery bakery;
CAS cas;

/////////////////////////////////////////////////////////////

const int half_taget = 5000'0000;

void f_none(int num_threads)
{
	const int loop_count = half_taget / num_threads;
	for (int i = 0; i < loop_count; i++)
	{
		sum += 2;
	}
}

void f_mut(int num_threads)
{
	const int loop_count = half_taget / num_threads;
	for (int i = 0; i < loop_count; i++)
	{
		scoped_lock lck(mut);
		sum += 2;
	}
}

void f_bakery(int num_threads)
{
	const int loop_count = half_taget / num_threads;
	for (int i = 0; i < loop_count; i++)
	{
		scoped_lock lck(bakery);
		sum += 2;
	}
}

void f_cas(int num_threads)
{
	const int loop_count = half_taget / num_threads;
	for (int i = 0; i < loop_count; i++)
	{
		scoped_lock lck(cas);
		sum += 2;
	}
}

/////////////////////////////////////////////////////////////
const int vers = 4;
decltype(&f_none) _f[vers] = { f_none, f_mut ,f_bakery ,f_cas };
string_view ver_names[vers] = { "no_lock", "mutex", "bakery", "cas" };

void f(int thr_id, int num_of_thrs, int ver)
{
	tid = thr_id;
	_f[ver](num_of_thrs);
}

/////////////////////////////////////////////////////////////

int main()
{
	cout << "max core = [" << MAX_THREADS << "]" << endl;

	for (int ver = 0; ver < vers; ver++)
	{
		cout << endl << "ver = " << ver_names[ver] << endl;
		for (int n_threads = 1; n_threads < MAX_THREADS; n_threads *= 2)
		{
			sum = 0;
			bakery.reset(n_threads);
			vector<thread> workers; workers.reserve(n_threads);
			auto start_t = chrono::high_resolution_clock::now();
			for (int id = 0; id < n_threads; id++)
				workers.emplace_back(f, id, n_threads, ver);
			for (auto& th : workers)
				th.join();
			auto end_t = chrono::high_resolution_clock::now();
			auto du_t = end_t - start_t;

			cout << " num of threads = [" << n_threads;
			cout << "] exe_time = [" << chrono::duration_cast<chrono::milliseconds>(du_t).count();
			cout << "] sum = " << sum << endl;
		}
	}
	cout << "끝" << endl;
}