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

//-----types-------------------------
using BYTE = uint8_t;

using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;

using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;	  
//-----------------------------------

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
		static clk::time_point StopWatch;

		__forceinline void start() {
			StopWatch = clk::now();
		}

		__forceinline void end(const std::string_view mess = ""sv)
		{
			auto t = clk::now();
			std::cout << mess << " : " << duration_cast<milliseconds>(t - StopWatch) << '\n';
		}
	}
}

//	RANDOM
namespace MY_NAME_SPACE
{
	static std::random_device rd;			// hardware non-d random
	static std::default_random_engine dre{ rd() };		// with seed, d random
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

}

// OPTIMAIZE benchmark_nessasary
namespace MY_NAME_SPACE {
	using namespace std;

	// avoid conditional branch miss // pipeline_stall
	template<integral _Ty> __forceinline constexpr _Ty abs(const _Ty x) noexcept
	{
		const _Ty y{ x >> (TypeInfo<_Ty>::bits - 1) }; /* >> = copy MSB // positive=>0 negative =>-1 */
		return (x ^ y) - y;
	}
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

	__forceinline void checked_join(std::thread& t) {
		if (t.joinable())t.join();
	}

// length_of_array
#define sizeof_array(t) sizeof(t) / sizeof(t[0])

	// [m,m)
	template<integral T, integral _T1, integral _T2>
	__forceinline constexpr bool in_range(const T _Value, const _T1 _min_inc, const _T2 _max_noinc) noexcept
	{
		return (_min_inc <= _Value) && (_Value < _max_noinc);
	}

	

	
}


