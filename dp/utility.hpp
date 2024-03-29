#pragma once
#include <stdint.h>
#include <algorithm>
#include <new>
#include <vector>

#ifdef _MSC_VER
#pragma warning(disable: 4482) // nonstandard extension used enum Name::Val, standard in C++11
#define NO_RETURN __declspec(noreturn)
#else
#define NO_RETURN __attribute__((noreturn))
#endif


//namespace std {
//  typedef decltype(sizeof(int)) size_t;
//  typedef decltype(nullptr) nullptr_t;
//}

namespace dp {
  using std::size_t;
  typedef decltype(nullptr) nullptr_t;

  template<typename T> struct remove_reference           { typedef T type;       };
  template<typename T> struct remove_reference<T&>       { typedef T type;       };
  template<typename T> struct remove_reference<T&&>      { typedef T type;       };

  template<typename T> struct deduce           { typedef T type; };
  template<typename T> struct deduce<T&>       { typedef T type; };
  template<typename T> struct deduce<const T&> { typedef T type; };
  template<typename T> struct deduce<T&&>      { typedef T type; };
  template<typename T> struct deduce<const T&&>{ typedef T type; };


  using std::move;
  using std::forward;

  struct true_type  { enum _value { value = 1 }; };
  struct false_type { enum _value { value = 0 }; };

  namespace detail {
    template<typename T> dp::true_type is_class_helper(void(T::*)());
    template<typename T> dp::false_type is_class_helper(...);
  }

  template<typename T>
  struct is_class { typedef decltype(detail::is_class_helper<T>(0)) type; enum value_enum { value = type::value }; };
#ifdef min 
#undef min
#endif
  template<typename T>
  const T& min( const T& a, const T& b ) { return a < b ? a: b; }

  constexpr size_t const_strlen(const char* str) {
     int i = 0;
     while(*(str+i) != '\0')
        i++;
     return i; 
  }


  template<typename T>
  void move_append(std::vector<T> &dest, std::vector<T>&& src ) {
    if (src.empty()) {
      return;
    } else if (dest.empty()) {
      dest = std::move(src);
    } else {
      dest.insert(std::end(dest), std::make_move_iterator(std::begin(src)), std::make_move_iterator(std::end(src)));
    }
  }

  template<typename T>
  void copy_append(std::vector<T> &dest, const std::vector<T>& src ) {
    if (src.empty()) {
      return;
    } else {
      dest.insert(std::end(dest), std::begin(src), std::end(src));
    }
  }

  template<typename T>
  void deduplicate( std::vector<T>& entries ) {
    if (entries.size() > 1) {
      std::sort( entries.begin(), entries.end() );
      auto itr = std::unique( entries.begin(), entries.end() );
      entries.erase( itr, entries.end() );
    }
  }
}

  // outside of namespace dp becuase of VC++ conflict with std::swap
  template<typename T>
  void dp_swap( T& a, T& b ) {     
    T tmp = dp::move(a);
    a = dp::move(b);
    b = dp::move(tmp);
  }

#define LLCONST(constant)   static_cast<int64_t>(constant##ll)
#define ULLCONST(constant)  static_cast<uint64_t>(constant##ull)

