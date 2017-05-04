// clang++-4.0 -std=c++1z -c 
// g++-6 -std=c++11 -c # is_detected_v<...> fails  to compile, can be substituted with is_detected<...>()

// based on The Detection Idiom - a simpler way to SFINAE - Marshall Clow [ACCU 2017]
//   https://www.youtube.com/watch?v=o1ekBpEFcPc
// which bases on section called "Detection idiom" hidden in the Library fundamentals Technical Specification


#include <type_traits>

#if 1
template <class ...>
using void_t = void;// ignore ...Args
#else
//^ shorter then 
template <class ...>
struct void_t_impl;
template <class ...Args>
using void_t = void_t_impl<Args...>;
#endif

struct nonesuch {
	nonesuch() = delete;
	~nonesuch() = delete;
	nonesuch(nonesuch const &) = delete;
	void operator=(nonesuch const &) = delete;
};

////////////////////////////////////////////////////////////////////////

// the following always successfully instantiates 
template <class Default, class AlwaysVoid, template <class...> class Op, class...Args>
struct Detector {
	using value_t = std::false_type;
	using type = Default;
};

// the following instantiates only if Op<Args...> and then is choosen as the best overload /it is more specialized/
template <class Default, template <class...> class Op, class...Args>
struct Detector<Default, void_t<Op<Args...>>, Op, Args...> {
	                     // ^ must be a template instantiation to kick off SFINAE instead hard error
	using value_t = std::true_type;
	using type = Op<Args...>;
};

////////////////////////////////////////////////////////////////////////
template <template <class...> class Op, class...Args>
using is_detected = typename Detector<nonesuch, void, Op, Args...>::value_t;
//		since C++17 - clang-4 supports, gcc-6 does not support
template <template <class...> class Op, class...Args>
inline constexpr bool is_detected_v = is_detected<Op, Args...>::value;

template <template <class...> class Op, class...Args>
using detected_t = typename Detector<nonesuch, void, Op, Args...>::type;
// returns nonesuch if not detected - so it is rather usable when we already detected

////////////////////////////////////////////////////////////////////////
template <class Default, template <class...> class Op, class...Args>
using detected_or = Detector<Default, void, Op, Args...>;

template <class Expected, template <class...> class Op, class...Args>
using is_detected_exact = std::is_same<Expected, detected_t<Op, Args...> >;

template <class To, template <class...> class Op, class...Args>
using is_detected_convertible = std::is_convertible<detected_t<Op, Args...>, To >;

// Copy assigment example
namespace test_detect {
  struct Yes { Yes& operator=(Yes const&) { return *this; } };
  struct Huh { void operator=(Huh const&) {} };
  struct No { No& operator=(No const&) = delete; };
  
  template <class T>
  using copy_assign = decltype( std::declval<T&>() = std::declval<T const&>() );
  
  static_assert(is_detected<copy_assign, Yes>());
  static_assert(is_detected_v<copy_assign, Huh>);
  static_assert(!is_detected_v<copy_assign, No>);
  
  static_assert(is_detected_exact<Yes&,copy_assign, Yes>());
  static_assert(!is_detected_exact<Yes,copy_assign, Yes>());
  static_assert(!is_detected_exact<Huh&,copy_assign, Huh>());
  static_assert(is_detected_exact<void,copy_assign, Huh>());
  static_assert(!is_detected_exact<No&,copy_assign, No>());
}

