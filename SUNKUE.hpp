#pragma once
/*
* This is sunkue's utility hpp. almost JJamBBong. didn't subdivided.
* sunkue@kakao.com //  korean // yoon_sunkue.
*

*/

#include <concepts>
#include <type_traits>
#include <string>
#include <iostream>
#include <string_view>
#include <ranges>
#include <thread>
#include <any>
#include <random>
#include <thread>
#include <chrono>



#define MY_NAME_SPACE SUNKUE

#ifndef NONAMESPACE
namespace MY_NAME_SPACE {} using namespace MY_NAME_SPACE; using namespace std;
#endif // NONAMESPACE



//	CHRONO
namespace MY_NAME_SPACE
{
	using clk = std::chrono::high_resolution_clock;
	using namespace std::chrono;
	using namespace std::literals::string_view_literals;

	namespace timer {
		class TIMER {
		public:
			static TIMER& instance() {
				static TIMER instance_;
				return instance_;
			}

			void start() noexcept {
				_stop_watch = clk::now();
			}

			void end(std::string_view mess = ""sv, std::ostream& os = std::cout) {
				auto t = clk::now();
				_lap = duration_cast<milliseconds>(t - _stop_watch);
				os << mess << " : " << _lap << std::endl;
			}

			const milliseconds get_last_lap() const noexcept { return _lap; }
		private:
			clk::time_point _stop_watch;
			milliseconds _lap{};
		};
	}
}

//	RANDOM
namespace MY_NAME_SPACE
{
	static std::random_device rd;
	static std::default_random_engine dre{ rd() };	// u(dre)
}

//	TYPE_INFO
namespace MY_NAME_SPACE {
	using namespace std;

	template<class T> consteval auto _GetValueType()
	{
		if constexpr (is_pointer<T>()) return _GetValueType<remove_pointer_t<T>>();
		else return T();
	}

	template<class _Ty> struct TypeInfo
	{
		using value_type = decltype(_GetValueType<_Ty>());


		static constexpr size_t bits{ sizeof(_Ty) * 8 };
	};
	
	template <class _T>
	concept primitive_t = is_arithmetic_v<_T> || is_enum_v<_T>;
}

// OPTIMAIZE benchmark_nessasary
namespace MY_NAME_SPACE {
	using namespace std;

	namespace optimize {
		// branchless
		template<integral _Ty> inline constexpr _Ty abs(const _Ty x) noexcept {
			const _Ty y{ x >> (TypeInfo<_Ty>::bits - 1) }; /* >> = copy MSB // positive=>0 negative =>-1 */
			return (x ^ y) - y;
		}

		// branchless 
		inline constexpr char toupper_alphabets(char c) noexcept {
			return c -= 32 * ('a' <= c && c <= 'z');
		}

		// branchless 
		inline constexpr char tolower_alphabets(char c) noexcept {
			return c += 32 * ('A' <= c && c <= 'Z');
		}
	}
}

//	compile_time
namespace MY_NAME_SPACE {
	template<class Lambda, int = (Lambda{}(), 0) >
	constexpr bool _is_constexpr(Lambda) { return true; }
	constexpr bool _is_constexpr(...) { return false; }

	// 함수 반환값 컴파일 타임인지 확인
#define is_constexpr(Ret_val) _is_constexpr([]() {return Ret_val; })
}

//	Thread
namespace MY_NAME_SPACE {
	class ThreadRAII {
	public:
		enum class DtorAction { join, detach };

		ThreadRAII(std::thread&& t, DtorAction a) :m_action{ a }, m_thread(std::move(t)){}
		~ThreadRAII()
		{
			if (m_thread.joinable()) {
				if (m_action == DtorAction::join) m_thread.join();
				else m_thread.detach();
			}
		}

		ThreadRAII(ThreadRAII&&) = default;
		ThreadRAII& operator=(ThreadRAII&&) = default;

		std::thread& get() { return m_thread; }

	private:
		DtorAction m_action;
		std::thread m_thread;
	};


}

//	UTIL
namespace MY_NAME_SPACE {
	using namespace std;


	// 컨테이너 [b,e) macro
#define ALLOF(cont)(std::begin(cont)),(std::end(cont))
#define cALLOF(cont)(std::cbegin(cont)),(std::cend(cont))


#ifdef UNICODE
	using tstring = wstring;
#else
	using tstring = string;
#endif // UNICODE

	// to_tstring
	template<class T> inline tstring to_tstring(T arg)
	{
		if constexpr (is_same<tstring, wstring>())return to_wstring(arg);
		else return to_string(arg);
	}


	// 람다식 오버로딩 묶음생성. visit, varriant 함께 사용하면 좋음
	template<class... Ts> struct overloaded : Ts...{
		using Ts::operator()...;
	};
	template<class... Ts>overloaded(Ts...)->overloaded<Ts...>;

	inline void checked_join(std::thread& t) {
		if (t.joinable())t.join();
	}

	// length_of_array
#define sizeof_array(t) sizeof(t) / sizeof(t[0])

	template<unsigned_integral _uint>
	constexpr inline bool check_overflow_sum(_uint a, _uint b) noexcept
	{
		if constexpr (a < numeric_limits<_uint>::max() - b) [[likely]]
		{
			return false;
		}
		else /* overflowed */
		{
			return true;
		}
	}

	template<unsigned_integral _uint>
	constexpr inline bool check_underflow_sub(_uint a, _uint b) noexcept
	{
		if constexpr (b < a) [[likely]]
		{
			return false;
		}
		else /* underflow */
		{
			return true;
		}
	} 
}


