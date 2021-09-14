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

using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;

using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;	  

//-----------------------------------

#ifndef NONAMESPACE
namespace SUNKUE {} using namespace SUNKUE; using namespace std;
#endif // NONAMESPACE


namespace SUNKUE {
	using namespace std;
	using BYTE = uint8_t;

#ifndef NORANDOM
	static random_device rd;			// hardware non-d random
	static default_random_engine dre(rd());		// with seed, d random
#endif // NORANDOM

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

	template<class T> struct TypeInfo
	{
		// 포인터 벗기기
		using value_type = decltype(_GetValueType<T>());
	};

	template<class T> consteval auto _GetValueType()
	{
		if constexpr (is_pointer<T>()) return _GetValueType<remove_pointer_t<T>>();
		else return T();
	}

	// 람다식 오버로딩 묶음생성. visit, varriant 함께 사용하면 좋음
	template<class... Ts> struct overloaded : Ts...{
		using Ts::operator()...;
	};
	template<class... Ts>overloaded(Ts...)->overloaded<Ts...>;

	__forceinline void checked_join(std::thread& t) {
		if (t.joinable())t.join();
	}

#define sizeof_array(t) sizeof(t) / sizeof(t[0])

	template<class T, class _T1, class _T2>
	constexpr bool in_range(const T _Value, const _T1 _min_inc, const _T2 _max_noinc) noexcept
		requires integral<T>&& integral<_T1>&& integral<_T2>
	{
		return (_min_inc <= _Value) && (_Value < _max_noinc);
	}
}


