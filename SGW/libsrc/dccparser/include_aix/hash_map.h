#ifndef HASH_MAP_MAP
#define HASH_MAP_MAP
#include <iostream>
#include <functional>
#include <iterator>
#include <memory>
#include <vector>
#include <algorithm>

using std::pair;
using std::make_pair;

typedef long int ptrdiff_t;

typedef long unsigned int size_t;


namespace std
{
  template <class _Tp>
    struct _Identity : public unary_function<_Tp,_Tp>
    {
      _Tp&
      operator()(_Tp& __x) const
      { return __x; }

      const _Tp&
      operator()(const _Tp& __x) const
      { return __x; }
    };

  template <class _Pair>
    struct _Select1st : public unary_function<_Pair,
           typename _Pair::first_type>
    {
      typename _Pair::first_type&
      operator()(_Pair& __x) const
      { return __x.first; }

      const typename _Pair::first_type&
      operator()(const _Pair& __x) const
      { return __x.first; }
    };

  template <class _Pair>
    struct _Select2nd : public unary_function<_Pair,
           typename _Pair::second_type>
    {
      typename _Pair::second_type&
      operator()(_Pair& __x) const
      { return __x.second; }

      const typename _Pair::second_type&
      operator()(const _Pair& __x) const
      { return __x.second; }
    };

/*
template<typename _ForwardIterator, typename _Tp>
    _ForwardIterator
    lower_bound(_ForwardIterator __first, _ForwardIterator __last,
  const _Tp& __val)
    {
      typedef typename iterator_traits<_ForwardIterator>::value_type
 _ValueType;
      typedef typename iterator_traits<_ForwardIterator>::difference_type
 _DistanceType;









      ;

      _DistanceType __len = std::distance(__first, __last);
      _DistanceType __half;
      _ForwardIterator __middle;

      while (__len > 0)
 {
   __half = __len >> 1;
   __middle = __first;
   std::advance(__middle, __half);
   if (*__middle < __val)
     {
       __first = __middle;
       ++__first;
       __len = __len - __half - 1;
     }
   else
     __len = __half;
 }
      return __first;
    }
*/
}

namespace std
{
  using ::ptrdiff_t;
  using ::size_t;
}

namespace __gnu_cxx
{
  using std::unary_function;
  using std::binary_function;
  using std::mem_fun1_t;
  using std::const_mem_fun1_t;
  using std::mem_fun1_ref_t;
  using std::const_mem_fun1_ref_t;

  template <class _Tp>
    inline _Tp
    identity_element(std::plus<_Tp>)
    { return _Tp(0); }


  template <class _Tp>
    inline _Tp
    identity_element(std::multiplies<_Tp>)
    { return _Tp(1); }

  template <class _Operation1, class _Operation2>
    class unary_compose
    : public unary_function<typename _Operation2::argument_type,
       typename _Operation1::result_type>
    {
    protected:
      _Operation1 _M_fn1;
      _Operation2 _M_fn2;

    public:
      unary_compose(const _Operation1& __x, const _Operation2& __y)
      : _M_fn1(__x), _M_fn2(__y) {}

      typename _Operation1::result_type
      operator()(const typename _Operation2::argument_type& __x) const
      { return _M_fn1(_M_fn2(__x)); }
    };


  template <class _Operation1, class _Operation2>
    inline unary_compose<_Operation1, _Operation2>
    compose1(const _Operation1& __fn1, const _Operation2& __fn2)
    { return unary_compose<_Operation1,_Operation2>(__fn1, __fn2); }


  template <class _Operation1, class _Operation2, class _Operation3>
    class binary_compose
    : public unary_function<typename _Operation2::argument_type,
       typename _Operation1::result_type>
    {
    protected:
      _Operation1 _M_fn1;
      _Operation2 _M_fn2;
      _Operation3 _M_fn3;

    public:
      binary_compose(const _Operation1& __x, const _Operation2& __y,
       const _Operation3& __z)
      : _M_fn1(__x), _M_fn2(__y), _M_fn3(__z) { }

      typename _Operation1::result_type
      operator()(const typename _Operation2::argument_type& __x) const
      { return _M_fn1(_M_fn2(__x), _M_fn3(__x)); }
    };


  template <class _Operation1, class _Operation2, class _Operation3>
    inline binary_compose<_Operation1, _Operation2, _Operation3>
    compose2(const _Operation1& __fn1, const _Operation2& __fn2,
      const _Operation3& __fn3)
    { return binary_compose<_Operation1, _Operation2, _Operation3>
 (__fn1, __fn2, __fn3); }

  template <class _Tp>
    struct identity : public std::_Identity<_Tp> {};

  template <class _Pair>
    struct select1st : public std::_Select1st<_Pair> {};


  template <class _Pair>
    struct select2nd : public std::_Select2nd<_Pair> {};



  template <class _Arg1, class _Arg2>
    struct _Project1st : public binary_function<_Arg1, _Arg2, _Arg1>
    {
      _Arg1
      operator()(const _Arg1& __x, const _Arg2&) const
      { return __x; }
    };

  template <class _Arg1, class _Arg2>
    struct _Project2nd : public binary_function<_Arg1, _Arg2, _Arg2>
    {
      _Arg2
      operator()(const _Arg1&, const _Arg2& __y) const
      { return __y; }
    };

  template <class _Arg1, class _Arg2>
    struct project1st : public _Project1st<_Arg1, _Arg2> {};


  template <class _Arg1, class _Arg2>
    struct project2nd : public _Project2nd<_Arg1, _Arg2> {};



  template <class _Result>
    struct _Constant_void_fun
    {
      typedef _Result result_type;
      result_type _M_val;

      _Constant_void_fun(const result_type& __v) : _M_val(__v) {}

      const result_type&
      operator()() const
      { return _M_val; }
    };

  template <class _Result, class _Argument>
    struct _Constant_unary_fun
    {
      typedef _Argument argument_type;
      typedef _Result result_type;
      result_type _M_val;

      _Constant_unary_fun(const result_type& __v) : _M_val(__v) {}

      const result_type&
      operator()(const _Argument&) const
      { return _M_val; }
    };

  template <class _Result, class _Arg1, class _Arg2>
    struct _Constant_binary_fun
    {
      typedef _Arg1 first_argument_type;
      typedef _Arg2 second_argument_type;
      typedef _Result result_type;
      _Result _M_val;

      _Constant_binary_fun(const _Result& __v) : _M_val(__v) {}

      const result_type&
      operator()(const _Arg1&, const _Arg2&) const
      { return _M_val; }
    };

  template <class _Result>
    struct constant_void_fun
    : public _Constant_void_fun<_Result>
    {
      constant_void_fun(const _Result& __v)
      : _Constant_void_fun<_Result>(__v) {}
    };


  template <class _Result, class _Argument = _Result>
    struct constant_unary_fun : public _Constant_unary_fun<_Result, _Argument>
    {
      constant_unary_fun(const _Result& __v)
      : _Constant_unary_fun<_Result, _Argument>(__v) {}
    };


  template <class _Result, class _Arg1 = _Result, class _Arg2 = _Arg1>
    struct constant_binary_fun
    : public _Constant_binary_fun<_Result, _Arg1, _Arg2>
    {
      constant_binary_fun(const _Result& __v)
      : _Constant_binary_fun<_Result, _Arg1, _Arg2>(__v) {}
    };


  template <class _Result>
    inline constant_void_fun<_Result>
    constant0(const _Result& __val)
    { return constant_void_fun<_Result>(__val); }


  template <class _Result>
    inline constant_unary_fun<_Result, _Result>
    constant1(const _Result& __val)
    { return constant_unary_fun<_Result, _Result>(__val); }


  template <class _Result>
    inline constant_binary_fun<_Result,_Result,_Result>
    constant2(const _Result& __val)
    { return constant_binary_fun<_Result, _Result, _Result>(__val); }

  class subtractive_rng
  : public unary_function<unsigned int, unsigned int>
  {
  private:
    unsigned int _M_table[55];
    size_t _M_index1;
    size_t _M_index2;

  public:

    unsigned int
    operator()(unsigned int __limit)
    {
      _M_index1 = (_M_index1 + 1) % 55;
      _M_index2 = (_M_index2 + 1) % 55;
      _M_table[_M_index1] = _M_table[_M_index1] - _M_table[_M_index2];
      return _M_table[_M_index1] % __limit;
    }

    void
    _M_initialize(unsigned int __seed)
    {
      unsigned int __k = 1;
      _M_table[54] = __seed;
      size_t __i;
      for (__i = 0; __i < 54; __i++)
 {
   size_t __ii = (21 * (__i + 1) % 55) - 1;
   _M_table[__ii] = __k;
   __k = __seed - __k;
   __seed = _M_table[__ii];
 }
      for (int __loop = 0; __loop < 4; __loop++)
 {
   for (__i = 0; __i < 55; __i++)
            _M_table[__i] = _M_table[__i] - _M_table[(1 + __i + 30) % 55];
 }
      _M_index1 = 0;
      _M_index2 = 31;
    }


    subtractive_rng(unsigned int __seed)
    { _M_initialize(__seed); }


    subtractive_rng()
    { _M_initialize(161803398u); }
  };





  template <class _Ret, class _Tp, class _Arg>
    inline mem_fun1_t<_Ret, _Tp, _Arg>
    mem_fun1(_Ret (_Tp::*__f)(_Arg))
    { return mem_fun1_t<_Ret, _Tp, _Arg>(__f); }

  template <class _Ret, class _Tp, class _Arg>
    inline const_mem_fun1_t<_Ret, _Tp, _Arg>
    mem_fun1(_Ret (_Tp::*__f)(_Arg) const)
    { return const_mem_fun1_t<_Ret, _Tp, _Arg>(__f); }

  template <class _Ret, class _Tp, class _Arg>
    inline mem_fun1_ref_t<_Ret, _Tp, _Arg>
    mem_fun1_ref(_Ret (_Tp::*__f)(_Arg))
    { return mem_fun1_ref_t<_Ret, _Tp, _Arg>(__f); }

  template <class _Ret, class _Tp, class _Arg>
    inline const_mem_fun1_ref_t<_Ret, _Tp, _Arg>
    mem_fun1_ref(_Ret (_Tp::*__f)(_Arg) const)
    { return const_mem_fun1_ref_t<_Ret, _Tp, _Arg>(__f); }
}



using std::unary_function;
using std::binary_function;
using std::plus;
using std::minus;
using std::multiplies;
using std::divides;
using std::modulus;
using std::negate;
using std::equal_to;
using std::not_equal_to;
using std::greater;
using std::less;
using std::greater_equal;
using std::less_equal;
using std::logical_and;
using std::logical_or;
using std::logical_not;
using std::unary_negate;
using std::binary_negate;
using std::not1;
using std::not2;
using std::binder1st;
using std::binder2nd;
using std::bind1st;
using std::bind2nd;
using std::pointer_to_unary_function;
using std::pointer_to_binary_function;
using std::ptr_fun;
using std::mem_fun_t;
using std::const_mem_fun_t;
using std::mem_fun_ref_t;
using std::const_mem_fun_ref_t;
using std::mem_fun1_t;
using std::const_mem_fun1_t;
using std::mem_fun1_ref_t;
using std::const_mem_fun1_ref_t;
using std::mem_fun;
using std::mem_fun_ref;


using __gnu_cxx::identity_element;
using __gnu_cxx::unary_compose;
using __gnu_cxx::binary_compose;
using __gnu_cxx::compose1;
using __gnu_cxx::compose2;
using __gnu_cxx::identity;
using __gnu_cxx::select1st;
using __gnu_cxx::select2nd;
using __gnu_cxx::project1st;
using __gnu_cxx::project2nd;
using __gnu_cxx::constant_void_fun;
using __gnu_cxx::constant_unary_fun;
using __gnu_cxx::constant_binary_fun;
using __gnu_cxx::constant0;
using __gnu_cxx::constant1;
using __gnu_cxx::constant2;
using __gnu_cxx::subtractive_rng;
using __gnu_cxx::mem_fun1;
using __gnu_cxx::mem_fun1_ref;


namespace std
{
  using ::lconv;
  using ::setlocale;
  using ::localeconv;
}


//typedef __builtin_va_list __gnuc_va_list;


namespace std
{
  using ::FILE;
  using ::fpos_t;

  using ::clearerr;
  using ::fclose;
  using ::feof;
  using ::ferror;
  using ::fflush;
  using ::fgetc;
  using ::fgetpos;
  using ::fgets;
  using ::fopen;
  using ::fprintf;
  using ::fputc;
  using ::fputs;
  using ::fread;
  using ::freopen;
  using ::fscanf;
  using ::fseek;
  using ::fsetpos;
  using ::ftell;
  using ::fwrite;
  using ::getc;
  using ::getchar;
  using ::gets;
  using ::perror;
  using ::printf;
  using ::putc;
  using ::putchar;
  using ::puts;
  using ::remove;
  using ::rename;
  using ::rewind;
  using ::scanf;
  using ::setbuf;
  using ::setvbuf;
  using ::sprintf;
  using ::sscanf;
  using ::tmpfile;
  using ::tmpnam;
  using ::ungetc;
  using ::vfprintf;
  using ::vprintf;
  using ::vsprintf;
}

namespace __gnu_cxx
{

  using ::snprintf;
  using ::vfscanf;
  using ::vscanf;
  using ::vsnprintf;
  using ::vsscanf;

}

namespace std
{
  using __gnu_cxx::snprintf;
  using __gnu_cxx::vfscanf;
  using __gnu_cxx::vscanf;
  using __gnu_cxx::vsnprintf;
  using __gnu_cxx::vsscanf;
}




namespace std
{
  typedef int* __c_locale;





  template<typename _Tv>
    int
    __convert_from_v(char* __out,
       const int __size __attribute__((__unused__)),
       const char* __fmt,
       _Tv __v, const __c_locale&, int __prec)
    {
      char* __old = std::setlocale(3, (0));
      char* __sav = (0);
      if (std::strcmp(__old, "C"))
 {
   __sav = new char[std::strlen(__old) + 1];
   std::strcpy(__sav, __old);
   std::setlocale(3, "C");
 }


      const int __ret = std::snprintf(__out, __size, __fmt, __prec, __v);




      if (__sav)
 {
   std::setlocale(3, __sav);
   delete [] __sav;
 }
      return __ret;
    }
}











typedef int __gthread_mutex_t;
typedef int __gthread_recursive_mutex_t;

static inline int
__gthread_active_p (void)
{
  return 0;
}

static inline int
__gthread_mutex_lock (__gthread_mutex_t * )
{
  return 0;
}

static inline int
__gthread_mutex_trylock (__gthread_mutex_t * )
{
  return 0;
}

static inline int
__gthread_mutex_unlock (__gthread_mutex_t * )
{
  return 0;
}

static inline int
__gthread_recursive_mutex_lock (__gthread_recursive_mutex_t *mutex)
{
  return __gthread_mutex_lock (mutex);
}

static inline int
__gthread_recursive_mutex_trylock (__gthread_recursive_mutex_t *mutex)
{
  return __gthread_mutex_trylock (mutex);
}

static inline int
__gthread_recursive_mutex_unlock (__gthread_recursive_mutex_t *mutex)
{
  return __gthread_mutex_unlock (mutex);
}












namespace std
{
  typedef __gthread_mutex_t __c_lock;


  typedef FILE __c_file;
}









 extern int isalpha(int);
 extern int isalnum(int);

 extern int isblank(int);

 extern int iscntrl(int);
 extern int isdigit(int);
 extern int isgraph(int);
 extern int islower(int);
 extern int isprint(int);
 extern int ispunct(int);
 extern int isspace(int);
 extern int isupper(int);
 extern int isxdigit(int);
 extern int toupper(int);
 extern int tolower(int);

 extern int isascii(int);
 extern int toascii(int);




namespace std
{
  using ::isalnum;
  using ::isalpha;
  using ::iscntrl;
  using ::isdigit;
  using ::isgraph;
  using ::islower;
  using ::isprint;
  using ::ispunct;
  using ::isspace;
  using ::isupper;
  using ::isxdigit;
  using ::tolower;
  using ::toupper;
}




















namespace std
{
  using ::mbstate_t;
}

namespace std
{
  using ::wint_t;

  using ::btowc;
  using ::fgetwc;
  using ::fgetws;
  using ::fputwc;
  using ::fputws;
  using ::fwide;
  using ::fwprintf;
  using ::fwscanf;
  using ::getwc;
  using ::getwchar;
  using ::mbrlen;
  using ::mbrtowc;
  using ::mbsinit;
  using ::mbsrtowcs;
  using ::putwc;
  using ::putwchar;
  using ::swprintf;
  using ::swscanf;
  using ::ungetwc;
  using ::vfwprintf;

  using ::vfwscanf;

  using ::vswprintf;

  using ::vswscanf;

  using ::vwprintf;

  using ::vwscanf;

  using ::wcrtomb;
  using ::wcscat;
  using ::wcscmp;
  using ::wcscoll;
  using ::wcscpy;
  using ::wcscspn;
  using ::wcsftime;
  using ::wcslen;
  using ::wcsncat;
  using ::wcsncmp;
  using ::wcsncpy;
  using ::wcsrtombs;
  using ::wcsspn;
  using ::wcstod;

  using ::wcstof;

  using ::wcstok;
  using ::wcstol;
  using ::wcstoul;
  using ::wcsxfrm;
  using ::wctob;
  using ::wmemcmp;
  using ::wmemcpy;
  using ::wmemmove;
  using ::wmemset;
  using ::wprintf;
  using ::wscanf;

  using ::wcschr;

  inline wchar_t*
  wcschr(wchar_t* __p, wchar_t __c)
  { return wcschr(const_cast<const wchar_t*>(__p), __c); }

  using ::wcspbrk;

  inline wchar_t*
  wcspbrk(wchar_t* __s1, const wchar_t* __s2)
  { return wcspbrk(const_cast<const wchar_t*>(__s1), __s2); }

  using ::wcsrchr;

  inline wchar_t*
  wcsrchr(wchar_t* __p, wchar_t __c)
  { return wcsrchr(const_cast<const wchar_t*>(__p), __c); }

  using ::wcsstr;

  inline wchar_t*
  wcsstr(wchar_t* __s1, const wchar_t* __s2)
  { return wcsstr(const_cast<const wchar_t*>(__s1), __s2); }

  using ::wmemchr;

  inline wchar_t*
  wmemchr(wchar_t* __p, wchar_t __c, size_t __n)
  { return wmemchr(const_cast<const wchar_t*>(__p), __c, __n); }
}







namespace __gnu_cxx
{





  using ::wcstold;

  using ::wcstoll;
  using ::wcstoull;

}

namespace std
{
  using __gnu_cxx::wcstold;
  using __gnu_cxx::wcstoll;
  using __gnu_cxx::wcstoull;
}




extern "C++" {

namespace __gnu_cxx
{

  void __verbose_terminate_handler ();
}

}









































namespace std
{
  using ::div_t;
  using ::ldiv_t;

  using ::abort;
  using ::abs;
  using ::atexit;
  using ::atof;
  using ::atoi;
  using ::atol;
  using ::bsearch;
  using ::calloc;
  using ::div;
  using ::exit;
  using ::free;
  using ::getenv;
  using ::labs;
  using ::ldiv;
  using ::malloc;

  using ::mblen;
  using ::mbstowcs;
  using ::mbtowc;

  using ::qsort;
  using ::rand;
  using ::realloc;
  using ::srand;
  using ::strtod;
  using ::strtol;
  using ::strtoul;
  using ::system;

  using ::wcstombs;
  using ::wctomb;


//  inline long
//  abs(long __i) { return labs(__i); }

//  inline ldiv_t
//  div(long __i, long __j) { return ldiv(__i, __j); }
}

namespace __gnu_cxx
{

  using ::lldiv_t;





  using ::_Exit;


  inline long long
  abs(long long __x) { return __x >= 0 ? __x : -__x; }


  using ::llabs;

  inline lldiv_t
  div(long long __n, long long __d)
  { lldiv_t __q; __q.quot = __n / __d; __q.rem = __n % __d; return __q; }

  using ::lldiv;

  using ::atoll;
  using ::strtoll;
  using ::strtoull;

  using ::strtof;
  using ::strtold;
}

namespace std
{

  using ::__gnu_cxx::lldiv_t;

  using ::__gnu_cxx::_Exit;
  using ::__gnu_cxx::abs;

  using ::__gnu_cxx::llabs;
  using ::__gnu_cxx::div;
  using ::__gnu_cxx::lldiv;

  using ::__gnu_cxx::atoll;
  using ::__gnu_cxx::strtof;
  using ::__gnu_cxx::strtoll;
  using ::__gnu_cxx::strtoull;
  using ::__gnu_cxx::strtold;
}









namespace __gnu_internal
{
  typedef char __one;
  typedef char __two[2];

  template<typename _Tp>
  __one __test_type(int _Tp::*);
  template<typename _Tp>
  __two& __test_type(...);
}


namespace __gnu_cxx
{
  template<typename _Iterator, typename _Container>
    class __normal_iterator;
}

struct __true_type { };
struct __false_type { };


namespace __gnu_cxx
{







  using std::iterator_traits;
  using std::iterator;
  template<typename _Iterator, typename _Container>
    class __normal_iterator
    {
    protected:
      _Iterator _M_current;

    public:
      typedef typename iterator_traits<_Iterator>::iterator_category
                                                             iterator_category;
      typedef typename iterator_traits<_Iterator>::value_type value_type;
      typedef typename iterator_traits<_Iterator>::difference_type
                                                             difference_type;
      typedef typename iterator_traits<_Iterator>::reference reference;
      typedef typename iterator_traits<_Iterator>::pointer pointer;

      __normal_iterator() : _M_current(_Iterator()) { }

      explicit
      __normal_iterator(const _Iterator& __i) : _M_current(__i) { }


      template<typename _Iter>
        __normal_iterator(const __normal_iterator<_Iter,
     typename std::__enable_if<_Container,
     (std::__are_same<_Iter,
      typename _Container::pointer>::__value)
     >::__type>& __i)
        : _M_current(__i.base()) { }


      reference
      operator*() const
      { return *_M_current; }

      pointer
      operator->() const
      { return _M_current; }

      __normal_iterator&
      operator++()
      {
 ++_M_current;
 return *this;
      }

      __normal_iterator
      operator++(int)
      { return __normal_iterator(_M_current++); }


      __normal_iterator&
      operator--()
      {
 --_M_current;
 return *this;
      }

      __normal_iterator
      operator--(int)
      { return __normal_iterator(_M_current--); }


      reference
      operator[](const difference_type& __n) const
      { return _M_current[__n]; }

      __normal_iterator&
      operator+=(const difference_type& __n)
      { _M_current += __n; return *this; }

      __normal_iterator
      operator+(const difference_type& __n) const
      { return __normal_iterator(_M_current + __n); }

      __normal_iterator&
      operator-=(const difference_type& __n)
      { _M_current -= __n; return *this; }

      __normal_iterator
      operator-(const difference_type& __n) const
      { return __normal_iterator(_M_current - __n); }

      const _Iterator&
      base() const
      { return _M_current; }
    };

  template<typename _IteratorL, typename _IteratorR, typename _Container>
    inline bool
    operator==(const __normal_iterator<_IteratorL, _Container>& __lhs,
        const __normal_iterator<_IteratorR, _Container>& __rhs)
    { return __lhs.base() == __rhs.base(); }

  template<typename _Iterator, typename _Container>
    inline bool
    operator==(const __normal_iterator<_Iterator, _Container>& __lhs,
        const __normal_iterator<_Iterator, _Container>& __rhs)
    { return __lhs.base() == __rhs.base(); }

  template<typename _IteratorL, typename _IteratorR, typename _Container>
    inline bool
    operator!=(const __normal_iterator<_IteratorL, _Container>& __lhs,
        const __normal_iterator<_IteratorR, _Container>& __rhs)
    { return __lhs.base() != __rhs.base(); }

  template<typename _Iterator, typename _Container>
    inline bool
    operator!=(const __normal_iterator<_Iterator, _Container>& __lhs,
        const __normal_iterator<_Iterator, _Container>& __rhs)
    { return __lhs.base() != __rhs.base(); }


  template<typename _IteratorL, typename _IteratorR, typename _Container>
    inline bool
    operator<(const __normal_iterator<_IteratorL, _Container>& __lhs,
       const __normal_iterator<_IteratorR, _Container>& __rhs)
    { return __lhs.base() < __rhs.base(); }

  template<typename _Iterator, typename _Container>
    inline bool
    operator<(const __normal_iterator<_Iterator, _Container>& __lhs,
       const __normal_iterator<_Iterator, _Container>& __rhs)
    { return __lhs.base() < __rhs.base(); }

  template<typename _IteratorL, typename _IteratorR, typename _Container>
    inline bool
    operator>(const __normal_iterator<_IteratorL, _Container>& __lhs,
       const __normal_iterator<_IteratorR, _Container>& __rhs)
    { return __lhs.base() > __rhs.base(); }

  template<typename _Iterator, typename _Container>
    inline bool
    operator>(const __normal_iterator<_Iterator, _Container>& __lhs,
       const __normal_iterator<_Iterator, _Container>& __rhs)
    { return __lhs.base() > __rhs.base(); }

  template<typename _IteratorL, typename _IteratorR, typename _Container>
    inline bool
    operator<=(const __normal_iterator<_IteratorL, _Container>& __lhs,
        const __normal_iterator<_IteratorR, _Container>& __rhs)
    { return __lhs.base() <= __rhs.base(); }

  template<typename _Iterator, typename _Container>
    inline bool
    operator<=(const __normal_iterator<_Iterator, _Container>& __lhs,
        const __normal_iterator<_Iterator, _Container>& __rhs)
    { return __lhs.base() <= __rhs.base(); }

  template<typename _IteratorL, typename _IteratorR, typename _Container>
    inline bool
    operator>=(const __normal_iterator<_IteratorL, _Container>& __lhs,
        const __normal_iterator<_IteratorR, _Container>& __rhs)
    { return __lhs.base() >= __rhs.base(); }

  template<typename _Iterator, typename _Container>
    inline bool
    operator>=(const __normal_iterator<_Iterator, _Container>& __lhs,
        const __normal_iterator<_Iterator, _Container>& __rhs)
    { return __lhs.base() >= __rhs.base(); }





  template<typename _IteratorL, typename _IteratorR, typename _Container>
    inline typename __normal_iterator<_IteratorL, _Container>::difference_type
    operator-(const __normal_iterator<_IteratorL, _Container>& __lhs,
       const __normal_iterator<_IteratorR, _Container>& __rhs)
    { return __lhs.base() - __rhs.base(); }

  template<typename _Iterator, typename _Container>
    inline __normal_iterator<_Iterator, _Container>
    operator+(typename __normal_iterator<_Iterator, _Container>::difference_type
       __n, const __normal_iterator<_Iterator, _Container>& __i)
    { return __normal_iterator<_Iterator, _Container>(__i.base() + __n); }
}






namespace __gnu_cxx
{

  template <class _CharT>
    struct _Char_types
    {
      typedef unsigned long int_type;
      typedef std::streampos pos_type;
      typedef std::streamoff off_type;
      typedef std::mbstate_t state_type;
    };

  template<typename _CharT>
    struct char_traits
    {
      typedef _CharT char_type;
      typedef typename _Char_types<_CharT>::int_type int_type;
      typedef typename _Char_types<_CharT>::pos_type pos_type;
      typedef typename _Char_types<_CharT>::off_type off_type;
      typedef typename _Char_types<_CharT>::state_type state_type;

      static void
      assign(char_type& __c1, const char_type& __c2)
      { __c1 = __c2; }

      static bool
      eq(const char_type& __c1, const char_type& __c2)
      { return __c1 == __c2; }

      static bool
      lt(const char_type& __c1, const char_type& __c2)
      { return __c1 < __c2; }

      static int
      compare(const char_type* __s1, const char_type* __s2, std::size_t __n);

      static std::size_t
      length(const char_type* __s);

      static const char_type*
      find(const char_type* __s, std::size_t __n, const char_type& __a);

      static char_type*
      move(char_type* __s1, const char_type* __s2, std::size_t __n);

      static char_type*
      copy(char_type* __s1, const char_type* __s2, std::size_t __n);

      static char_type*
      assign(char_type* __s, std::size_t __n, char_type __a);

      static char_type
      to_char_type(const int_type& __c)
      { return static_cast<char_type>(__c); }

      static int_type
      to_int_type(const char_type& __c)
      { return static_cast<int_type>(__c); }

      static bool
      eq_int_type(const int_type& __c1, const int_type& __c2)
      { return __c1 == __c2; }

      static int_type
      eof()
      { return static_cast<int_type>((-1)); }

      static int_type
      not_eof(const int_type& __c)
      { return !eq_int_type(__c, eof()) ? __c : to_int_type(char_type()); }
    };

  template<typename _CharT>
    int
    char_traits<_CharT>::
    compare(const char_type* __s1, const char_type* __s2, std::size_t __n)
    {
      for (size_t __i = 0; __i < __n; ++__i)
 if (lt(__s1[__i], __s2[__i]))
   return -1;
 else if (lt(__s2[__i], __s1[__i]))
   return 1;
      return 0;
    }

  template<typename _CharT>
    std::size_t
    char_traits<_CharT>::
    length(const char_type* __p)
    {
      std::size_t __i = 0;
      while (!eq(__p[__i], char_type()))
        ++__i;
      return __i;
    }

  template<typename _CharT>
    const typename char_traits<_CharT>::char_type*
    char_traits<_CharT>::
    find(const char_type* __s, std::size_t __n, const char_type& __a)
    {
      for (std::size_t __i = 0; __i < __n; ++__i)
        if (eq(__s[__i], __a))
          return __s + __i;
      return 0;
    }

  template<typename _CharT>
    typename char_traits<_CharT>::char_type*
    char_traits<_CharT>::
    move(char_type* __s1, const char_type* __s2, std::size_t __n)
    {
      return static_cast<_CharT*>(std::memmove(__s1, __s2,
            __n * sizeof(char_type)));
    }

  template<typename _CharT>
    typename char_traits<_CharT>::char_type*
    char_traits<_CharT>::
    copy(char_type* __s1, const char_type* __s2, std::size_t __n)
    {
      std::copy(__s2, __s2 + __n, __s1);
      return __s1;
    }

  template<typename _CharT>
    typename char_traits<_CharT>::char_type*
    char_traits<_CharT>::
    assign(char_type* __s, std::size_t __n, char_type __a)
    {
      std::fill_n(__s, __n, __a);
      return __s;
    }
}




typedef int _Atomic_word;


namespace __gnu_cxx
{
  _Atomic_word
  __attribute__ ((__unused__))
  __exchange_and_add(volatile _Atomic_word* __mem, int __val);

  void
  __attribute__ ((__unused__))
  __atomic_add(volatile _Atomic_word* __mem, int __val);
}






























namespace __gnu_cxx
{







  template<typename _Tp>
    class new_allocator
    {
    public:
      typedef size_t size_type;
      typedef ptrdiff_t difference_type;
      typedef _Tp* pointer;
      typedef const _Tp* const_pointer;
      typedef _Tp& reference;
      typedef const _Tp& const_reference;
      typedef _Tp value_type;

      template<typename _Tp1>
        struct rebind
        { typedef new_allocator<_Tp1> other; };

      new_allocator() throw() { }

      new_allocator(const new_allocator&) throw() { }

      template<typename _Tp1>
        new_allocator(const new_allocator<_Tp1>&) throw() { }

      ~new_allocator() throw() { }

      pointer
      address(reference __x) const { return &__x; }

      const_pointer
      address(const_reference __x) const { return &__x; }



      pointer
      allocate(size_type __n, const void* = 0)
      {
 if (__builtin_expect(__n > this->max_size(), false))
   std::__throw_bad_alloc();

 return static_cast<_Tp*>(::operator new(__n * sizeof(_Tp)));
      }


      void
      deallocate(pointer __p, size_type)
      { ::operator delete(__p); }

      size_type
      max_size() const throw()
      { return size_t(-1) / sizeof(_Tp); }



      void
      construct(pointer __p, const _Tp& __val)
      { ::new(__p) _Tp(__val); }

      void
      destroy(pointer __p) { __p->~_Tp(); }
    };

  template<typename _Tp>
    inline bool
    operator==(const new_allocator<_Tp>&, const new_allocator<_Tp>&)
    { return true; }

  template<typename _Tp>
    inline bool
    operator!=(const new_allocator<_Tp>&, const new_allocator<_Tp>&)
    { return false; }
}






extern "C++" {

namespace __cxxabiv1
{
  class __class_type_info;
}



}

namespace __gnu_cxx
{
  template<typename _InputIterator, typename _Distance>
    inline void
    __distance(_InputIterator __first, _InputIterator __last,
        _Distance& __n, std::input_iterator_tag)
    {


      while (__first != __last)
 {
   ++__first;
   ++__n;
 }
    }

  template<typename _RandomAccessIterator, typename _Distance>
    inline void
    __distance(_RandomAccessIterator __first, _RandomAccessIterator __last,
        _Distance& __n, std::random_access_iterator_tag)
    {



      __n += __last - __first;
    }






  template<typename _InputIterator, typename _Distance>
    inline void
    distance(_InputIterator __first, _InputIterator __last,
             _Distance& __n)
    {

      __distance(__first, __last, __n, std::__iterator_category(__first));
    }
}



using std::input_iterator_tag;
using std::output_iterator_tag;
using std::forward_iterator_tag;
using std::bidirectional_iterator_tag;
using std::random_access_iterator_tag;

template<typename _Tp, typename _Distance>
  struct input_iterator {
    typedef input_iterator_tag iterator_category;
    typedef _Tp value_type;
    typedef _Distance difference_type;
    typedef _Tp* pointer;
    typedef _Tp& reference;
  };

struct output_iterator {
  typedef output_iterator_tag iterator_category;
  typedef void value_type;
  typedef void difference_type;
  typedef void pointer;
  typedef void reference;
};

template<typename _Tp, typename _Distance>
  struct forward_iterator {
    typedef forward_iterator_tag iterator_category;
    typedef _Tp value_type;
    typedef _Distance difference_type;
    typedef _Tp* pointer;
    typedef _Tp& reference;
  };

template<typename _Tp, typename _Distance>
  struct bidirectional_iterator {
    typedef bidirectional_iterator_tag iterator_category;
    typedef _Tp value_type;
    typedef _Distance difference_type;
    typedef _Tp* pointer;
    typedef _Tp& reference;
  };

template<typename _Tp, typename _Distance>
  struct random_access_iterator {
    typedef random_access_iterator_tag iterator_category;
    typedef _Tp value_type;
    typedef _Distance difference_type;
    typedef _Tp* pointer;
    typedef _Tp& reference;
  };

using std::iterator_traits;

template <class _Iter>
  inline typename iterator_traits<_Iter>::iterator_category
  iterator_category(const _Iter& __i)
  { return __iterator_category(__i); }

template <class _Iter>
  inline typename iterator_traits<_Iter>::difference_type*
  distance_type(const _Iter&)
  { return static_cast<typename iterator_traits<_Iter>::difference_type*>(0); }

template<class _Iter>
  inline typename iterator_traits<_Iter>::value_type*
  value_type(const _Iter& __i)
  { return static_cast<typename iterator_traits<_Iter>::value_type*>(0); }

using std::distance;
using __gnu_cxx::distance;
using std::advance;

using std::insert_iterator;
using std::front_insert_iterator;
using std::back_insert_iterator;
using std::inserter;
using std::front_inserter;
using std::back_inserter;

using std::reverse_iterator;

using std::istream_iterator;
using std::ostream_iterator;


template<class _T1, class _T2>
  inline void
  construct(_T1* __p, const _T2& __value)
  { std::_Construct(__p, __value); }

template<class _T1>
  inline void
  construct(_T1* __p)
  { std::_Construct(__p); }

template <class _Tp>
  inline void
  destroy(_Tp* __pointer)
  { std::_Destroy(__pointer); }

template <class _ForwardIterator>
  inline void
  destroy(_ForwardIterator __first, _ForwardIterator __last)
  { std::_Destroy(__first, __last); }



using std::raw_storage_iterator;










namespace __gnu_cxx
{
  using std::ptrdiff_t;
  using std::min;
  using std::pair;
  using std::input_iterator_tag;
  using std::random_access_iterator_tag;
  using std::iterator_traits;




  template<typename _InputIterator, typename _Size, typename _OutputIterator>
    pair<_InputIterator, _OutputIterator>
    __copy_n(_InputIterator __first, _Size __count,
      _OutputIterator __result,
      input_iterator_tag)
    {
      for ( ; __count > 0; --__count)
 {
   *__result = *__first;
   ++__first;
   ++__result;
 }
      return pair<_InputIterator, _OutputIterator>(__first, __result);
    }

  template<typename _RAIterator, typename _Size, typename _OutputIterator>
    inline pair<_RAIterator, _OutputIterator>
    __copy_n(_RAIterator __first, _Size __count,
      _OutputIterator __result,
      random_access_iterator_tag)
    {
      _RAIterator __last = __first + __count;
      return pair<_RAIterator, _OutputIterator>(__last, std::copy(__first,
          __last,
          __result));
    }

  template<typename _InputIterator, typename _Size, typename _OutputIterator>
    inline pair<_InputIterator, _OutputIterator>
    copy_n(_InputIterator __first, _Size __count, _OutputIterator __result)
    {





      return __copy_n(__first, __count, __result,
        std::__iterator_category(__first));
    }

  template<typename _InputIterator1, typename _InputIterator2>
    int
    __lexicographical_compare_3way(_InputIterator1 __first1,
       _InputIterator1 __last1,
       _InputIterator2 __first2,
       _InputIterator2 __last2)
    {
      while (__first1 != __last1 && __first2 != __last2)
 {
   if (*__first1 < *__first2)
     return -1;
   if (*__first2 < *__first1)
     return 1;
   ++__first1;
   ++__first2;
 }
      if (__first2 == __last2)
 return !(__first1 == __last1);
      else
 return -1;
    }

  inline int
  __lexicographical_compare_3way(const unsigned char* __first1,
     const unsigned char* __last1,
     const unsigned char* __first2,
     const unsigned char* __last2)
  {
    const ptrdiff_t __len1 = __last1 - __first1;
    const ptrdiff_t __len2 = __last2 - __first2;
    const int __result = std::memcmp(__first1, __first2, min(__len1, __len2));
    return __result != 0 ? __result
    : (__len1 == __len2 ? 0 : (__len1 < __len2 ? -1 : 1));
  }

  inline int
  __lexicographical_compare_3way(const char* __first1, const char* __last1,
     const char* __first2, const char* __last2)
  {






    return __lexicographical_compare_3way((const unsigned char*) __first1,
       (const unsigned char*) __last1,
       (const unsigned char*) __first2,
       (const unsigned char*) __last2);

  }

  template<typename _InputIterator1, typename _InputIterator2>
    int
    lexicographical_compare_3way(_InputIterator1 __first1,
     _InputIterator1 __last1,
     _InputIterator2 __first2,
     _InputIterator2 __last2)
    {







      ;
      ;

      return __lexicographical_compare_3way(__first1, __last1, __first2,
         __last2);
    }



  template<typename _InputIterator, typename _Tp, typename _Size>
    void
    count(_InputIterator __first, _InputIterator __last,
   const _Tp& __value,
   _Size& __n)
    {





      ;

      for ( ; __first != __last; ++__first)
 if (*__first == __value)
   ++__n;
    }

  template<typename _InputIterator, typename _Predicate, typename _Size>
    void
    count_if(_InputIterator __first, _InputIterator __last,
      _Predicate __pred,
      _Size& __n)
    {




      ;

      for ( ; __first != __last; ++__first)
 if (__pred(*__first))
   ++__n;
    }

  template<typename _ForwardIterator, typename _OutputIterator,
    typename _Distance>
    _OutputIterator
    random_sample_n(_ForwardIterator __first, _ForwardIterator __last,
                    _OutputIterator __out, const _Distance __n)
    {




      ;

      _Distance __remaining = std::distance(__first, __last);
      _Distance __m = min(__n, __remaining);

      while (__m > 0)
 {
   if ((std::rand() % __remaining) < __m)
     {
       *__out = *__first;
       ++__out;
       --__m;
     }
   --__remaining;
   ++__first;
 }
      return __out;
    }






  template<typename _ForwardIterator, typename _OutputIterator,
    typename _Distance, typename _RandomNumberGenerator>
    _OutputIterator
    random_sample_n(_ForwardIterator __first, _ForwardIterator __last,
                   _OutputIterator __out, const _Distance __n,
     _RandomNumberGenerator& __rand)
    {






      ;

      _Distance __remaining = std::distance(__first, __last);
      _Distance __m = min(__n, __remaining);

      while (__m > 0)
 {
   if (__rand(__remaining) < __m)
     {
       *__out = *__first;
       ++__out;
       --__m;
     }
   --__remaining;
   ++__first;
 }
      return __out;
    }

  template<typename _InputIterator, typename _RandomAccessIterator,
    typename _Distance>
    _RandomAccessIterator
    __random_sample(_InputIterator __first, _InputIterator __last,
      _RandomAccessIterator __out,
      const _Distance __n)
    {
      _Distance __m = 0;
      _Distance __t = __n;
      for ( ; __first != __last && __m < __n; ++__m, ++__first)
 __out[__m] = *__first;

      while (__first != __last)
 {
   ++__t;
   _Distance __M = std::rand() % (__t);
   if (__M < __n)
     __out[__M] = *__first;
   ++__first;
 }
      return __out + __m;
    }

  template<typename _InputIterator, typename _RandomAccessIterator,
    typename _RandomNumberGenerator, typename _Distance>
    _RandomAccessIterator
    __random_sample(_InputIterator __first, _InputIterator __last,
      _RandomAccessIterator __out,
      _RandomNumberGenerator& __rand,
      const _Distance __n)
    {




      _Distance __m = 0;
      _Distance __t = __n;
      for ( ; __first != __last && __m < __n; ++__m, ++__first)
 __out[__m] = *__first;

      while (__first != __last)
 {
   ++__t;
   _Distance __M = __rand(__t);
   if (__M < __n)
     __out[__M] = *__first;
   ++__first;
 }
      return __out + __m;
    }






  template<typename _InputIterator, typename _RandomAccessIterator>
    inline _RandomAccessIterator
    random_sample(_InputIterator __first, _InputIterator __last,
    _RandomAccessIterator __out_first,
    _RandomAccessIterator __out_last)
    {




      ;
      ;

      return __random_sample(__first, __last,
        __out_first, __out_last - __out_first);
    }






  template<typename _InputIterator, typename _RandomAccessIterator,
    typename _RandomNumberGenerator>
    inline _RandomAccessIterator
    random_sample(_InputIterator __first, _InputIterator __last,
    _RandomAccessIterator __out_first,
    _RandomAccessIterator __out_last,
    _RandomNumberGenerator& __rand)
    {




      ;
      ;

      return __random_sample(__first, __last,
        __out_first, __rand,
        __out_last - __out_first);
    }






  template<typename _RandomAccessIterator>
    inline bool
    is_heap(_RandomAccessIterator __first, _RandomAccessIterator __last)
    {





      ;

      return std::__is_heap(__first, __last - __first);
    }






  template<typename _RandomAccessIterator, typename _StrictWeakOrdering>
    inline bool
    is_heap(_RandomAccessIterator __first, _RandomAccessIterator __last,
     _StrictWeakOrdering __comp)
    {






      ;

      return std::__is_heap(__first, __comp, __last - __first);
    }

  template<typename _ForwardIterator>
    bool
    is_sorted(_ForwardIterator __first, _ForwardIterator __last)
    {




      ;

      if (__first == __last)
 return true;

      _ForwardIterator __next = __first;
      for (++__next; __next != __last; __first = __next, ++__next)
 if (*__next < *__first)
   return false;
      return true;
    }






  template<typename _ForwardIterator, typename _StrictWeakOrdering>
    bool
    is_sorted(_ForwardIterator __first, _ForwardIterator __last,
       _StrictWeakOrdering __comp)
    {





      ;

      if (__first == __last)
 return true;

      _ForwardIterator __next = __first;
      for (++__next; __next != __last; __first = __next, ++__next)
 if (__comp(*__next, *__first))
   return false;
      return true;
    }
}









namespace __gnu_cxx
{
  using std::ptrdiff_t;
  using std::pair;
  //using std::__iterator_category;
  //using std::_Temporary_buffer;

  template<typename _InputIter, typename _Size, typename _ForwardIter>
    pair<_InputIter, _ForwardIter>
    __uninitialized_copy_n(_InputIter __first, _Size __count,
      _ForwardIter __result, std::input_iterator_tag)
    {
      _ForwardIter __cur = __result;
      try
 {
   for (; __count > 0 ; --__count, ++__first, ++__cur)
     std::_Construct(&*__cur, *__first);
   return pair<_InputIter, _ForwardIter>(__first, __cur);
 }
      catch(...)
 {
   std::_Destroy(__result, __cur);
   throw;
 }
    }

  template<typename _RandomAccessIter, typename _Size, typename _ForwardIter>
    inline pair<_RandomAccessIter, _ForwardIter>
    __uninitialized_copy_n(_RandomAccessIter __first, _Size __count,
      _ForwardIter __result,
      std::random_access_iterator_tag)
    {
      _RandomAccessIter __last = __first + __count;
      return (pair<_RandomAccessIter, _ForwardIter>
       (__last, std::uninitialized_copy(__first, __last, __result)));
    }

  template<typename _InputIter, typename _Size, typename _ForwardIter>
    inline pair<_InputIter, _ForwardIter>
    __uninitialized_copy_n(_InputIter __first, _Size __count,
    _ForwardIter __result)
    { return __uninitialized_copy_n(__first, __count, __result,
        __iterator_category(__first)); }

  template<typename _InputIter, typename _Size, typename _ForwardIter>
    inline pair<_InputIter, _ForwardIter>
    uninitialized_copy_n(_InputIter __first, _Size __count,
    _ForwardIter __result)
    { return __uninitialized_copy_n(__first, __count, __result,
        __iterator_category(__first)); }




  template<typename _InputIter, typename _Size, typename _ForwardIter,
           typename _Allocator>
    pair<_InputIter, _ForwardIter>
    __uninitialized_copy_n_a(_InputIter __first, _Size __count,
        _ForwardIter __result,
        _Allocator __alloc)
    {
      _ForwardIter __cur = __result;
      try
 {
   for (; __count > 0 ; --__count, ++__first, ++__cur)
     __alloc.construct(&*__cur, *__first);
   return pair<_InputIter, _ForwardIter>(__first, __cur);
 }
      catch(...)
 {
   std::_Destroy(__result, __cur, __alloc);
   throw;
 }
    }

  template<typename _InputIter, typename _Size, typename _ForwardIter,
           typename _Tp>
    inline pair<_InputIter, _ForwardIter>
    __uninitialized_copy_n_a(_InputIter __first, _Size __count,
        _ForwardIter __result,
        std::allocator<_Tp>)
    {
      return uninitialized_copy_n(__first, __count, __result);
    }

  template <class _ForwardIterator, class _Tp
     = typename std::iterator_traits<_ForwardIterator>::value_type >
    struct temporary_buffer : public _Temporary_buffer<_ForwardIterator, _Tp>
    {

      temporary_buffer(_ForwardIterator __first, _ForwardIterator __last)
      : _Temporary_buffer<_ForwardIterator, _Tp>(__first, __last) { }


      ~temporary_buffer() { }
    };
}



//using std::iter_swap;
using std::swap;
using std::min;
using std::max;
using std::copy;
using std::copy_backward;
using std::fill;
using std::fill_n;
using std::mismatch;
using std::equal;
using std::lexicographical_compare;


using std::uninitialized_copy;
using std::uninitialized_fill;
using std::uninitialized_fill_n;


using __gnu_cxx::copy_n;
using __gnu_cxx::lexicographical_compare_3way;


using __gnu_cxx::uninitialized_copy_n;


namespace __gnu_cxx
{
  using std::size_t;

  template<class _Key>
    struct hash { };

  inline size_t
  __stl_hash_string(const char* __s)
  {
    unsigned long __h = 0;
    for ( ; *__s; ++__s)
      __h = 5 * __h + *__s;
    return size_t(__h);
  }

  template<>
    struct hash<char*>
    {
      size_t
      operator()(const char* __s) const
      { return __stl_hash_string(__s); }
    };

  template<>
    struct hash<const char*>
    {
      size_t
      operator()(const char* __s) const
      { return __stl_hash_string(__s); }
    };

  template<>
    struct hash<char>
    {
      size_t
      operator()(char __x) const
      { return __x; }
    };

  template<>
    struct hash<unsigned char>
    {
      size_t
      operator()(unsigned char __x) const
      { return __x; }
    };

  template<>
    struct hash<signed char>
    {
      size_t
      operator()(unsigned char __x) const
      { return __x; }
    };

  template<>
    struct hash<short>
    {
      size_t
      operator()(short __x) const
      { return __x; }
    };

  template<>
    struct hash<unsigned short>
    {
      size_t
      operator()(unsigned short __x) const
      { return __x; }
    };

  template<>
    struct hash<int>
    {
      size_t
      operator()(int __x) const
      { return __x; }
    };

  template<>
    struct hash<unsigned int>
    {
      size_t
      operator()(unsigned int __x) const
      { return __x; }
    };

  template<>
    struct hash<long>
    {
      size_t
      operator()(long __x) const
      { return __x; }
    };

  template<>
    struct hash<unsigned long>
    {
      size_t
      operator()(unsigned long __x) const
      { return __x; }
    };
}


namespace __gnu_cxx
{
  using std::size_t;
  using std::ptrdiff_t;
  using std::forward_iterator_tag;
  using std::input_iterator_tag;
  using std::_Construct;
  using std::_Destroy;
  using std::distance;
  using std::vector;
  using std::pair;
  //using std::__iterator_category;

  template <class _Val>
    struct _Hashtable_node
    {
      _Hashtable_node* _M_next;
      _Val _M_val;
    };

  template <class _Val, class _Key, class _HashFcn, class _ExtractKey,
     class _EqualKey, class _Alloc = std::allocator<_Val> >
    class hashtable;

  template <class _Val, class _Key, class _HashFcn,
     class _ExtractKey, class _EqualKey, class _Alloc>
    struct _Hashtable_iterator;

  template <class _Val, class _Key, class _HashFcn,
     class _ExtractKey, class _EqualKey, class _Alloc>
    struct _Hashtable_const_iterator;

  template <class _Val, class _Key, class _HashFcn,
     class _ExtractKey, class _EqualKey, class _Alloc>
    struct _Hashtable_iterator
    {
      typedef hashtable<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>
        _Hashtable;
      typedef _Hashtable_iterator<_Val, _Key, _HashFcn,
      _ExtractKey, _EqualKey, _Alloc>
        iterator;
      typedef _Hashtable_const_iterator<_Val, _Key, _HashFcn,
     _ExtractKey, _EqualKey, _Alloc>
        const_iterator;
      typedef _Hashtable_node<_Val> _Node;
      typedef forward_iterator_tag iterator_category;
      typedef _Val value_type;
      typedef ptrdiff_t difference_type;
      typedef size_t size_type;
      typedef _Val& reference;
      typedef _Val* pointer;

      _Node* _M_cur;
      _Hashtable* _M_ht;

      _Hashtable_iterator(_Node* __n, _Hashtable* __tab)
      : _M_cur(__n), _M_ht(__tab) {}

      _Hashtable_iterator() {}

      reference
      operator*() const
      { return _M_cur->_M_val; }

      pointer
      operator->() const
      { return &(operator*()); }

      iterator&
      operator++();

      iterator
      operator++(int);

      bool
      operator==(const iterator& __it) const
      { return _M_cur == __it._M_cur; }

      bool
      operator!=(const iterator& __it) const
      { return _M_cur != __it._M_cur; }
    };

  template <class _Val, class _Key, class _HashFcn,
     class _ExtractKey, class _EqualKey, class _Alloc>
    struct _Hashtable_const_iterator
    {
      typedef hashtable<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>
        _Hashtable;
      typedef _Hashtable_iterator<_Val,_Key,_HashFcn,
      _ExtractKey,_EqualKey,_Alloc>
        iterator;
      typedef _Hashtable_const_iterator<_Val, _Key, _HashFcn,
     _ExtractKey, _EqualKey, _Alloc>
        const_iterator;
      typedef _Hashtable_node<_Val> _Node;

      typedef forward_iterator_tag iterator_category;
      typedef _Val value_type;
      typedef ptrdiff_t difference_type;
      typedef size_t size_type;
      typedef const _Val& reference;
      typedef const _Val* pointer;

      const _Node* _M_cur;
      const _Hashtable* _M_ht;

      _Hashtable_const_iterator(const _Node* __n, const _Hashtable* __tab)
      : _M_cur(__n), _M_ht(__tab) {}

      _Hashtable_const_iterator() {}

      _Hashtable_const_iterator(const iterator& __it)
      : _M_cur(__it._M_cur), _M_ht(__it._M_ht) {}

      reference
      operator*() const
      { return _M_cur->_M_val; }

      pointer
      operator->() const
      { return &(operator*()); }

      const_iterator&
      operator++();

      const_iterator
      operator++(int);

      bool
      operator==(const const_iterator& __it) const
      { return _M_cur == __it._M_cur; }

      bool
      operator!=(const const_iterator& __it) const
      { return _M_cur != __it._M_cur; }
    };


  enum { _S_num_primes = 28 };

  static const unsigned long __stl_prime_list[_S_num_primes] =
    {
      53ul, 97ul, 193ul, 389ul, 769ul,
      1543ul, 3079ul, 6151ul, 12289ul, 24593ul,
      49157ul, 98317ul, 196613ul, 393241ul, 786433ul,
      1572869ul, 3145739ul, 6291469ul, 12582917ul, 25165843ul,
      50331653ul, 100663319ul, 201326611ul, 402653189ul, 805306457ul,
      1610612741ul, 3221225473ul, 4294967291ul
    };

  inline unsigned long
  __stl_next_prime(unsigned long __n)
  {
    const unsigned long* __first = __stl_prime_list;
    const unsigned long* __last = __stl_prime_list + (int)_S_num_primes;
    const unsigned long* pos = std::lower_bound(__first, __last, __n);
    return pos == __last ? *(__last - 1) : *pos;
//return 0;
  }



  template <class _Val, class _Key, class _HF, class _Ex,
     class _Eq, class _All>
    class hashtable;

  template <class _Val, class _Key, class _HF, class _Ex,
     class _Eq, class _All>
    bool
    operator==(const hashtable<_Val, _Key, _HF, _Ex, _Eq, _All>& __ht1,
        const hashtable<_Val, _Key, _HF, _Ex, _Eq, _All>& __ht2);

  template <class _Val, class _Key, class _HashFcn,
     class _ExtractKey, class _EqualKey, class _Alloc>
    class hashtable
    {
    public:
      typedef _Key key_type;
      typedef _Val value_type;
      typedef _HashFcn hasher;
      typedef _EqualKey key_equal;

      typedef size_t size_type;
      typedef ptrdiff_t difference_type;
      typedef value_type* pointer;
      typedef const value_type* const_pointer;
      typedef value_type& reference;
      typedef const value_type& const_reference;

      hasher
      hash_funct() const
      { return _M_hash; }

      key_equal
      key_eq() const
      { return _M_equals; }

    private:
      typedef _Hashtable_node<_Val> _Node;

    public:
      typedef typename _Alloc::template rebind<value_type>::other allocator_type;
      allocator_type
      get_allocator() const
      { return _M_node_allocator; }

    private:
      typedef typename _Alloc::template rebind<_Node>::other _Node_Alloc;
      typedef typename _Alloc::template rebind<_Node*>::other _Nodeptr_Alloc;
      typedef vector<_Node*, _Nodeptr_Alloc> _Vector_type;

      _Node_Alloc _M_node_allocator;

      _Node*
      _M_get_node()
      { return _M_node_allocator.allocate(1); }

      void
      _M_put_node(_Node* __p)
      { _M_node_allocator.deallocate(__p, 1); }

    private:
      hasher _M_hash;
      key_equal _M_equals;
      _ExtractKey _M_get_key;
      _Vector_type _M_buckets;
      size_type _M_num_elements;

    public:
      typedef _Hashtable_iterator<_Val, _Key, _HashFcn, _ExtractKey,
      _EqualKey, _Alloc>
        iterator;
      typedef _Hashtable_const_iterator<_Val, _Key, _HashFcn, _ExtractKey,
     _EqualKey, _Alloc>
        const_iterator;

      friend struct
      _Hashtable_iterator<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>;

      friend struct
      _Hashtable_const_iterator<_Val, _Key, _HashFcn, _ExtractKey,
    _EqualKey, _Alloc>;

    public:
      hashtable(size_type __n, const _HashFcn& __hf,
  const _EqualKey& __eql, const _ExtractKey& __ext,
  const allocator_type& __a = allocator_type())
      : _M_node_allocator(__a), _M_hash(__hf), _M_equals(__eql),
 _M_get_key(__ext), _M_buckets(__a), _M_num_elements(0)
      { _M_initialize_buckets(__n); }

      hashtable(size_type __n, const _HashFcn& __hf,
  const _EqualKey& __eql,
  const allocator_type& __a = allocator_type())
      : _M_node_allocator(__a), _M_hash(__hf), _M_equals(__eql),
 _M_get_key(_ExtractKey()), _M_buckets(__a), _M_num_elements(0)
      { _M_initialize_buckets(__n); }

      hashtable(const hashtable& __ht)
      : _M_node_allocator(__ht.get_allocator()), _M_hash(__ht._M_hash),
      _M_equals(__ht._M_equals), _M_get_key(__ht._M_get_key),
      _M_buckets(__ht.get_allocator()), _M_num_elements(0)
      { _M_copy_from(__ht); }

      hashtable&
      operator= (const hashtable& __ht)
      {
 if (&__ht != this)
   {
     clear();
     _M_hash = __ht._M_hash;
     _M_equals = __ht._M_equals;
     _M_get_key = __ht._M_get_key;
     _M_copy_from(__ht);
   }
 return *this;
      }

      ~hashtable()
      { clear(); }

      size_type
      size() const
      { return _M_num_elements; }

      size_type
      max_size() const
      { return size_type(-1); }

      bool
      empty() const
      { return size() == 0; }

      void
      swap(hashtable& __ht)
      {
 std::swap(_M_hash, __ht._M_hash);
 std::swap(_M_equals, __ht._M_equals);
 std::swap(_M_get_key, __ht._M_get_key);
 _M_buckets.swap(__ht._M_buckets);
 std::swap(_M_num_elements, __ht._M_num_elements);
      }

      iterator
      begin()
      {
 for (size_type __n = 0; __n < _M_buckets.size(); ++__n)
   if (_M_buckets[__n])
     return iterator(_M_buckets[__n], this);
 return end();
      }

      iterator
      end()
      { return iterator(0, this); }

      const_iterator
      begin() const
      {
 for (size_type __n = 0; __n < _M_buckets.size(); ++__n)
   if (_M_buckets[__n])
     return const_iterator(_M_buckets[__n], this);
 return end();
      }

      const_iterator
      end() const
      { return const_iterator(0, this); }

      template <class _Vl, class _Ky, class _HF, class _Ex, class _Eq,
  class _Al>
        friend bool
        operator==(const hashtable<_Vl, _Ky, _HF, _Ex, _Eq, _Al>&,
     const hashtable<_Vl, _Ky, _HF, _Ex, _Eq, _Al>&);

    public:
      size_type
      bucket_count() const
      { return _M_buckets.size(); }

      size_type
      max_bucket_count() const
      { return __stl_prime_list[(int)_S_num_primes - 1]; }

      size_type
      elems_in_bucket(size_type __bucket) const
      {
 size_type __result = 0;
 for (_Node* __cur = _M_buckets[__bucket]; __cur; __cur = __cur->_M_next)
   __result += 1;
 return __result;
      }

      pair<iterator, bool>
      insert_unique(const value_type& __obj)
      {
 resize(_M_num_elements + 1);
 return insert_unique_noresize(__obj);
      }

      iterator
      insert_equal(const value_type& __obj)
      {
 resize(_M_num_elements + 1);
 return insert_equal_noresize(__obj);
      }

      pair<iterator, bool>
      insert_unique_noresize(const value_type& __obj);

      iterator
      insert_equal_noresize(const value_type& __obj);

      template <class _InputIterator>
        void
        insert_unique(_InputIterator __f, _InputIterator __l)
        { insert_unique(__f, __l, __iterator_category(__f)); }

      template <class _InputIterator>
        void
        insert_equal(_InputIterator __f, _InputIterator __l)
        { insert_equal(__f, __l, __iterator_category(__f)); }

      template <class _InputIterator>
        void
        insert_unique(_InputIterator __f, _InputIterator __l,
        input_iterator_tag)
        {
   for ( ; __f != __l; ++__f)
     insert_unique(*__f);
 }

      template <class _InputIterator>
        void
        insert_equal(_InputIterator __f, _InputIterator __l,
       input_iterator_tag)
        {
   for ( ; __f != __l; ++__f)
     insert_equal(*__f);
 }

      template <class _ForwardIterator>
        void
        insert_unique(_ForwardIterator __f, _ForwardIterator __l,
        forward_iterator_tag)
        {
   size_type __n = distance(__f, __l);
   resize(_M_num_elements + __n);
   for ( ; __n > 0; --__n, ++__f)
     insert_unique_noresize(*__f);
 }

      template <class _ForwardIterator>
        void
        insert_equal(_ForwardIterator __f, _ForwardIterator __l,
       forward_iterator_tag)
        {
   size_type __n = distance(__f, __l);
   resize(_M_num_elements + __n);
   for ( ; __n > 0; --__n, ++__f)
     insert_equal_noresize(*__f);
 }

      reference
      find_or_insert(const value_type& __obj);

      iterator
      find(const key_type& __key)
      {
 size_type __n = _M_bkt_num_key(__key);
 _Node* __first;
 for (__first = _M_buckets[__n];
      __first && !_M_equals(_M_get_key(__first->_M_val), __key);
      __first = __first->_M_next)
   {}
 return iterator(__first, this);
      }

      const_iterator
      find(const key_type& __key) const
      {
 size_type __n = _M_bkt_num_key(__key);
 const _Node* __first;
 for (__first = _M_buckets[__n];
      __first && !_M_equals(_M_get_key(__first->_M_val), __key);
      __first = __first->_M_next)
   {}
 return const_iterator(__first, this);
      }

      size_type
      count(const key_type& __key) const
      {
 const size_type __n = _M_bkt_num_key(__key);
 size_type __result = 0;

 for (const _Node* __cur = _M_buckets[__n]; __cur;
      __cur = __cur->_M_next)
   if (_M_equals(_M_get_key(__cur->_M_val), __key))
     ++__result;
 return __result;
      }

      pair<iterator, iterator>
      equal_range(const key_type& __key);

      pair<const_iterator, const_iterator>
      equal_range(const key_type& __key) const;

      size_type
      erase(const key_type& __key);

      void
      erase(const iterator& __it);

      void
      erase(iterator __first, iterator __last);

      void
      erase(const const_iterator& __it);

      void
      erase(const_iterator __first, const_iterator __last);

      void
      resize(size_type __num_elements_hint);

      void
      clear();

    private:
      size_type
      _M_next_size(size_type __n) const
      { return __stl_next_prime(__n); }

      void
      _M_initialize_buckets(size_type __n)
      {
 const size_type __n_buckets = _M_next_size(__n);
 _M_buckets.reserve(__n_buckets);
 _M_buckets.insert(_M_buckets.end(), __n_buckets, (_Node*) 0);
 _M_num_elements = 0;
      }

      size_type
      _M_bkt_num_key(const key_type& __key) const
      { return _M_bkt_num_key(__key, _M_buckets.size()); }

      size_type
      _M_bkt_num(const value_type& __obj) const
      { return _M_bkt_num_key(_M_get_key(__obj)); }

      size_type
      _M_bkt_num_key(const key_type& __key, size_t __n) const
      { return _M_hash(__key) % __n; }

      size_type
      _M_bkt_num(const value_type& __obj, size_t __n) const
      { return _M_bkt_num_key(_M_get_key(__obj), __n); }

      _Node*
      _M_new_node(const value_type& __obj)
      {
 _Node* __n = _M_get_node();
 __n->_M_next = 0;
 try
   {
     this->get_allocator().construct(&__n->_M_val, __obj);
     return __n;
   }
 catch(...)
   {
     _M_put_node(__n);
     throw;
   }
      }

      void
      _M_delete_node(_Node* __n)
      {
 this->get_allocator().destroy(&__n->_M_val);
 _M_put_node(__n);
      }

      void
      _M_erase_bucket(const size_type __n, _Node* __first, _Node* __last);

      void
      _M_erase_bucket(const size_type __n, _Node* __last);

      void
      _M_copy_from(const hashtable& __ht);
    };

  template <class _Val, class _Key, class _HF, class _ExK, class _EqK,
     class _All>
    _Hashtable_iterator<_Val, _Key, _HF, _ExK, _EqK, _All>&
    _Hashtable_iterator<_Val, _Key, _HF, _ExK, _EqK, _All>::
    operator++()
    {
      const _Node* __old = _M_cur;
      _M_cur = _M_cur->_M_next;
      if (!_M_cur)
 {
   size_type __bucket = _M_ht->_M_bkt_num(__old->_M_val);
   while (!_M_cur && ++__bucket < _M_ht->_M_buckets.size())
     _M_cur = _M_ht->_M_buckets[__bucket];
 }
      return *this;
    }

  template <class _Val, class _Key, class _HF, class _ExK, class _EqK,
     class _All>
    inline _Hashtable_iterator<_Val, _Key, _HF, _ExK, _EqK, _All>
    _Hashtable_iterator<_Val, _Key, _HF, _ExK, _EqK, _All>::
    operator++(int)
    {
      iterator __tmp = *this;
      ++*this;
      return __tmp;
    }

  template <class _Val, class _Key, class _HF, class _ExK, class _EqK,
     class _All>
    _Hashtable_const_iterator<_Val, _Key, _HF, _ExK, _EqK, _All>&
    _Hashtable_const_iterator<_Val, _Key, _HF, _ExK, _EqK, _All>::
    operator++()
    {
      const _Node* __old = _M_cur;
      _M_cur = _M_cur->_M_next;
      if (!_M_cur)
 {
   size_type __bucket = _M_ht->_M_bkt_num(__old->_M_val);
   while (!_M_cur && ++__bucket < _M_ht->_M_buckets.size())
     _M_cur = _M_ht->_M_buckets[__bucket];
 }
      return *this;
    }

  template <class _Val, class _Key, class _HF, class _ExK, class _EqK,
     class _All>
    inline _Hashtable_const_iterator<_Val, _Key, _HF, _ExK, _EqK, _All>
    _Hashtable_const_iterator<_Val, _Key, _HF, _ExK, _EqK, _All>::
    operator++(int)
    {
      const_iterator __tmp = *this;
      ++*this;
      return __tmp;
    }

  template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
    bool
    operator==(const hashtable<_Val, _Key, _HF, _Ex, _Eq, _All>& __ht1,
        const hashtable<_Val, _Key, _HF, _Ex, _Eq, _All>& __ht2)
    {
      typedef typename hashtable<_Val, _Key, _HF, _Ex, _Eq, _All>::_Node _Node;

      if (__ht1._M_buckets.size() != __ht2._M_buckets.size())
 return false;

      for (size_t __n = 0; __n < __ht1._M_buckets.size(); ++__n)
 {
   _Node* __cur1 = __ht1._M_buckets[__n];
   _Node* __cur2 = __ht2._M_buckets[__n];

   for (; __cur1 && __cur2;
        __cur1 = __cur1->_M_next, __cur2 = __cur2->_M_next)
     {}
   if (__cur1 || __cur2)
     return false;

   for (__cur1 = __ht1._M_buckets[__n] ; __cur1;
        __cur1 = __cur1->_M_next)
     {
       bool _found__cur1 = false;
       for (_Node* __cur2 = __ht2._M_buckets[__n];
     __cur2; __cur2 = __cur2->_M_next)
  {
    if (__cur1->_M_val == __cur2->_M_val)
      {
        _found__cur1 = true;
        break;
      }
  }
       if (!_found__cur1)
  return false;
     }
 }
      return true;
    }

  template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
    inline bool
    operator!=(const hashtable<_Val, _Key, _HF, _Ex, _Eq, _All>& __ht1,
        const hashtable<_Val, _Key, _HF, _Ex, _Eq, _All>& __ht2)
    { return !(__ht1 == __ht2); }

  template <class _Val, class _Key, class _HF, class _Extract, class _EqKey,
     class _All>
    inline void
    swap(hashtable<_Val, _Key, _HF, _Extract, _EqKey, _All>& __ht1,
  hashtable<_Val, _Key, _HF, _Extract, _EqKey, _All>& __ht2)
    { __ht1.swap(__ht2); }

  template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
    pair<typename hashtable<_Val, _Key, _HF, _Ex, _Eq, _All>::iterator, bool>
    hashtable<_Val, _Key, _HF, _Ex, _Eq, _All>::
    insert_unique_noresize(const value_type& __obj)
    {
      const size_type __n = _M_bkt_num(__obj);
      _Node* __first = _M_buckets[__n];

      for (_Node* __cur = __first; __cur; __cur = __cur->_M_next)
 if (_M_equals(_M_get_key(__cur->_M_val), _M_get_key(__obj)))
   return pair<iterator, bool>(iterator(__cur, this), false);

      _Node* __tmp = _M_new_node(__obj);
      __tmp->_M_next = __first;
      _M_buckets[__n] = __tmp;
      ++_M_num_elements;
      return pair<iterator, bool>(iterator(__tmp, this), true);
    }

  template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
    typename hashtable<_Val, _Key, _HF, _Ex, _Eq, _All>::iterator
    hashtable<_Val, _Key, _HF, _Ex, _Eq, _All>::
    insert_equal_noresize(const value_type& __obj)
    {
      const size_type __n = _M_bkt_num(__obj);
      _Node* __first = _M_buckets[__n];

      for (_Node* __cur = __first; __cur; __cur = __cur->_M_next)
 if (_M_equals(_M_get_key(__cur->_M_val), _M_get_key(__obj)))
   {
     _Node* __tmp = _M_new_node(__obj);
     __tmp->_M_next = __cur->_M_next;
     __cur->_M_next = __tmp;
     ++_M_num_elements;
     return iterator(__tmp, this);
   }

      _Node* __tmp = _M_new_node(__obj);
      __tmp->_M_next = __first;
      _M_buckets[__n] = __tmp;
      ++_M_num_elements;
      return iterator(__tmp, this);
    }

  template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
    typename hashtable<_Val, _Key, _HF, _Ex, _Eq, _All>::reference
    hashtable<_Val, _Key, _HF, _Ex, _Eq, _All>::
    find_or_insert(const value_type& __obj)
    {
      resize(_M_num_elements + 1);

      size_type __n = _M_bkt_num(__obj);
      _Node* __first = _M_buckets[__n];

      for (_Node* __cur = __first; __cur; __cur = __cur->_M_next)
 if (_M_equals(_M_get_key(__cur->_M_val), _M_get_key(__obj)))
   return __cur->_M_val;

      _Node* __tmp = _M_new_node(__obj);
      __tmp->_M_next = __first;
      _M_buckets[__n] = __tmp;
      ++_M_num_elements;
      return __tmp->_M_val;
    }

  template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
    pair<typename hashtable<_Val, _Key, _HF, _Ex, _Eq, _All>::iterator,
  typename hashtable<_Val, _Key, _HF, _Ex, _Eq, _All>::iterator>
    hashtable<_Val, _Key, _HF, _Ex, _Eq, _All>::
    equal_range(const key_type& __key)
    {
      typedef pair<iterator, iterator> _Pii;
      const size_type __n = _M_bkt_num_key(__key);

      for (_Node* __first = _M_buckets[__n]; __first;
    __first = __first->_M_next)
 if (_M_equals(_M_get_key(__first->_M_val), __key))
   {
     for (_Node* __cur = __first->_M_next; __cur;
   __cur = __cur->_M_next)
       if (!_M_equals(_M_get_key(__cur->_M_val), __key))
  return _Pii(iterator(__first, this), iterator(__cur, this));
     for (size_type __m = __n + 1; __m < _M_buckets.size(); ++__m)
       if (_M_buckets[__m])
  return _Pii(iterator(__first, this),
       iterator(_M_buckets[__m], this));
     return _Pii(iterator(__first, this), end());
   }
      return _Pii(end(), end());
    }

  template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
    pair<typename hashtable<_Val, _Key, _HF, _Ex, _Eq, _All>::const_iterator,
  typename hashtable<_Val, _Key, _HF, _Ex, _Eq, _All>::const_iterator>
    hashtable<_Val, _Key, _HF, _Ex, _Eq, _All>::
    equal_range(const key_type& __key) const
    {
      typedef pair<const_iterator, const_iterator> _Pii;
      const size_type __n = _M_bkt_num_key(__key);

      for (const _Node* __first = _M_buckets[__n]; __first;
    __first = __first->_M_next)
 {
   if (_M_equals(_M_get_key(__first->_M_val), __key))
     {
       for (const _Node* __cur = __first->_M_next; __cur;
     __cur = __cur->_M_next)
  if (!_M_equals(_M_get_key(__cur->_M_val), __key))
    return _Pii(const_iterator(__first, this),
         const_iterator(__cur, this));
       for (size_type __m = __n + 1; __m < _M_buckets.size(); ++__m)
  if (_M_buckets[__m])
    return _Pii(const_iterator(__first, this),
         const_iterator(_M_buckets[__m], this));
       return _Pii(const_iterator(__first, this), end());
     }
 }
      return _Pii(end(), end());
    }

  template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
    typename hashtable<_Val, _Key, _HF, _Ex, _Eq, _All>::size_type
    hashtable<_Val, _Key, _HF, _Ex, _Eq, _All>::
    erase(const key_type& __key)
    {
      const size_type __n = _M_bkt_num_key(__key);
      _Node* __first = _M_buckets[__n];
      size_type __erased = 0;

      if (__first)
 {
   _Node* __cur = __first;
   _Node* __next = __cur->_M_next;
   while (__next)
     {
       if (_M_equals(_M_get_key(__next->_M_val), __key))
  {
    __cur->_M_next = __next->_M_next;
    _M_delete_node(__next);
    __next = __cur->_M_next;
    ++__erased;
    --_M_num_elements;
  }
       else
  {
    __cur = __next;
    __next = __cur->_M_next;
  }
     }
   if (_M_equals(_M_get_key(__first->_M_val), __key))
     {
       _M_buckets[__n] = __first->_M_next;
       _M_delete_node(__first);
       ++__erased;
       --_M_num_elements;
     }
 }
      return __erased;
    }

  template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
    void hashtable<_Val, _Key, _HF, _Ex, _Eq, _All>::
    erase(const iterator& __it)
    {
      _Node* __p = __it._M_cur;
      if (__p)
 {
   const size_type __n = _M_bkt_num(__p->_M_val);
   _Node* __cur = _M_buckets[__n];

   if (__cur == __p)
     {
       _M_buckets[__n] = __cur->_M_next;
       _M_delete_node(__cur);
       --_M_num_elements;
     }
   else
     {
       _Node* __next = __cur->_M_next;
       while (__next)
  {
    if (__next == __p)
      {
        __cur->_M_next = __next->_M_next;
        _M_delete_node(__next);
        --_M_num_elements;
        break;
      }
    else
      {
        __cur = __next;
        __next = __cur->_M_next;
      }
  }
     }
 }
    }

  template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
    void
    hashtable<_Val, _Key, _HF, _Ex, _Eq, _All>::
    erase(iterator __first, iterator __last)
    {
      size_type __f_bucket = __first._M_cur ? _M_bkt_num(__first._M_cur->_M_val)
                                     : _M_buckets.size();

      size_type __l_bucket = __last._M_cur ? _M_bkt_num(__last._M_cur->_M_val)
                                    : _M_buckets.size();

      if (__first._M_cur == __last._M_cur)
 return;
      else if (__f_bucket == __l_bucket)
 _M_erase_bucket(__f_bucket, __first._M_cur, __last._M_cur);
      else
 {
   _M_erase_bucket(__f_bucket, __first._M_cur, 0);
   for (size_type __n = __f_bucket + 1; __n < __l_bucket; ++__n)
     _M_erase_bucket(__n, 0);
   if (__l_bucket != _M_buckets.size())
     _M_erase_bucket(__l_bucket, __last._M_cur);
 }
    }

  template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
    inline void
    hashtable<_Val, _Key, _HF, _Ex, _Eq, _All>::
    erase(const_iterator __first, const_iterator __last)
    {
      erase(iterator(const_cast<_Node*>(__first._M_cur),
       const_cast<hashtable*>(__first._M_ht)),
     iterator(const_cast<_Node*>(__last._M_cur),
       const_cast<hashtable*>(__last._M_ht)));
    }

  template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
    inline void
    hashtable<_Val, _Key, _HF, _Ex, _Eq, _All>::
    erase(const const_iterator& __it)
    { erase(iterator(const_cast<_Node*>(__it._M_cur),
       const_cast<hashtable*>(__it._M_ht))); }

  template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
    void
    hashtable<_Val, _Key, _HF, _Ex, _Eq, _All>::
    resize(size_type __num_elements_hint)
    {
      const size_type __old_n = _M_buckets.size();
      if (__num_elements_hint > __old_n)
 {
   const size_type __n = _M_next_size(__num_elements_hint);
   if (__n > __old_n)
     {
       _Vector_type __tmp(__n, (_Node*)(0), _M_buckets.get_allocator());
       try
  {
    for (size_type __bucket = 0; __bucket < __old_n; ++__bucket)
      {
        _Node* __first = _M_buckets[__bucket];
        while (__first)
   {
     size_type __new_bucket = _M_bkt_num(__first->_M_val,
             __n);
     _M_buckets[__bucket] = __first->_M_next;
     __first->_M_next = __tmp[__new_bucket];
     __tmp[__new_bucket] = __first;
     __first = _M_buckets[__bucket];
   }
      }
    _M_buckets.swap(__tmp);
  }
       catch(...)
  {
    for (size_type __bucket = 0; __bucket < __tmp.size();
         ++__bucket)
      {
        while (__tmp[__bucket])
   {
     _Node* __next = __tmp[__bucket]->_M_next;
     _M_delete_node(__tmp[__bucket]);
     __tmp[__bucket] = __next;
   }
      }
    throw;
  }
     }
 }
    }

  template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
    void
    hashtable<_Val, _Key, _HF, _Ex, _Eq, _All>::
    _M_erase_bucket(const size_type __n, _Node* __first, _Node* __last)
    {
      _Node* __cur = _M_buckets[__n];
      if (__cur == __first)
 _M_erase_bucket(__n, __last);
      else
 {
   _Node* __next;
   for (__next = __cur->_M_next;
        __next != __first;
        __cur = __next, __next = __cur->_M_next)
     ;
   while (__next != __last)
     {
       __cur->_M_next = __next->_M_next;
       _M_delete_node(__next);
       __next = __cur->_M_next;
       --_M_num_elements;
     }
 }
    }

  template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
    void
    hashtable<_Val, _Key, _HF, _Ex, _Eq, _All>::
    _M_erase_bucket(const size_type __n, _Node* __last)
    {
      _Node* __cur = _M_buckets[__n];
      while (__cur != __last)
 {
   _Node* __next = __cur->_M_next;
   _M_delete_node(__cur);
   __cur = __next;
   _M_buckets[__n] = __cur;
   --_M_num_elements;
 }
    }

  template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
    void
    hashtable<_Val, _Key, _HF, _Ex, _Eq, _All>::
    clear()
    {
      for (size_type __i = 0; __i < _M_buckets.size(); ++__i)
 {
   _Node* __cur = _M_buckets[__i];
   while (__cur != 0)
     {
       _Node* __next = __cur->_M_next;
       _M_delete_node(__cur);
       __cur = __next;
     }
   _M_buckets[__i] = 0;
 }
      _M_num_elements = 0;
    }

  template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
    void
    hashtable<_Val, _Key, _HF, _Ex, _Eq, _All>::
    _M_copy_from(const hashtable& __ht)
    {
      _M_buckets.clear();
      _M_buckets.reserve(__ht._M_buckets.size());
      _M_buckets.insert(_M_buckets.end(), __ht._M_buckets.size(), (_Node*) 0);
      try
 {
   for (size_type __i = 0; __i < __ht._M_buckets.size(); ++__i) {
     const _Node* __cur = __ht._M_buckets[__i];
     if (__cur)
       {
  _Node* __local_copy = _M_new_node(__cur->_M_val);
  _M_buckets[__i] = __local_copy;

  for (_Node* __next = __cur->_M_next;
       __next;
       __cur = __next, __next = __cur->_M_next)
    {
      __local_copy->_M_next = _M_new_node(__next->_M_val);
      __local_copy = __local_copy->_M_next;
    }
       }
   }
   _M_num_elements = __ht._M_num_elements;
 }
      catch(...)
 {
   clear();
   throw;
 }
    }
}



namespace __gnu_cxx
{
  using std::equal_to;
  using std::allocator;
  using std::pair;
  using std::_Select1st;



  template<class _Key, class _Tp, class _HashFcn = hash<_Key>,
    class _EqualKey = equal_to<_Key>, class _Alloc = allocator<_Tp> >
    class hash_map;

  template<class _Key, class _Tp, class _HashFn, class _EqKey, class _Alloc>
    inline bool
    operator==(const hash_map<_Key, _Tp, _HashFn, _EqKey, _Alloc>&,
        const hash_map<_Key, _Tp, _HashFn, _EqKey, _Alloc>&);

  template <class _Key, class _Tp, class _HashFcn, class _EqualKey,
     class _Alloc>
    class hash_map
    {
    private:
      typedef hashtable<pair<const _Key, _Tp>,_Key, _HashFcn,
   _Select1st<pair<const _Key, _Tp> >,
   _EqualKey, _Alloc> _Ht;

      _Ht _M_ht;

    public:
      typedef typename _Ht::key_type key_type;
      typedef _Tp data_type;
      typedef _Tp mapped_type;
      typedef typename _Ht::value_type value_type;
      typedef typename _Ht::hasher hasher;
      typedef typename _Ht::key_equal key_equal;

      typedef typename _Ht::size_type size_type;
      typedef typename _Ht::difference_type difference_type;
      typedef typename _Ht::pointer pointer;
      typedef typename _Ht::const_pointer const_pointer;
      typedef typename _Ht::reference reference;
      typedef typename _Ht::const_reference const_reference;

      typedef typename _Ht::iterator iterator;
      typedef typename _Ht::const_iterator const_iterator;

      typedef typename _Ht::allocator_type allocator_type;

      hasher
      hash_funct() const
      { return _M_ht.hash_funct(); }

      key_equal
      key_eq() const
      { return _M_ht.key_eq(); }

      allocator_type
      get_allocator() const
      { return _M_ht.get_allocator(); }

    public:
      hash_map()
      : _M_ht(100, hasher(), key_equal(), allocator_type()) {}

      explicit
      hash_map(size_type __n)
      : _M_ht(__n, hasher(), key_equal(), allocator_type()) {}

      hash_map(size_type __n, const hasher& __hf)
      : _M_ht(__n, __hf, key_equal(), allocator_type()) {}

      hash_map(size_type __n, const hasher& __hf, const key_equal& __eql,
        const allocator_type& __a = allocator_type())
      : _M_ht(__n, __hf, __eql, __a) {}

      template <class _InputIterator>
        hash_map(_InputIterator __f, _InputIterator __l)
 : _M_ht(100, hasher(), key_equal(), allocator_type())
        { _M_ht.insert_unique(__f, __l); }

      template <class _InputIterator>
        hash_map(_InputIterator __f, _InputIterator __l, size_type __n)
 : _M_ht(__n, hasher(), key_equal(), allocator_type())
        { _M_ht.insert_unique(__f, __l); }

      template <class _InputIterator>
        hash_map(_InputIterator __f, _InputIterator __l, size_type __n,
   const hasher& __hf)
 : _M_ht(__n, __hf, key_equal(), allocator_type())
        { _M_ht.insert_unique(__f, __l); }

      template <class _InputIterator>
        hash_map(_InputIterator __f, _InputIterator __l, size_type __n,
   const hasher& __hf, const key_equal& __eql,
   const allocator_type& __a = allocator_type())
 : _M_ht(__n, __hf, __eql, __a)
        { _M_ht.insert_unique(__f, __l); }

    public:
      size_type
      size() const
      { return _M_ht.size(); }

      size_type
      max_size() const
      { return _M_ht.max_size(); }

      bool
      empty() const
      { return _M_ht.empty(); }

      void
      swap(hash_map& __hs)
      { _M_ht.swap(__hs._M_ht); }

      template <class _K1, class _T1, class _HF, class _EqK, class _Al>
        friend bool
        operator== (const hash_map<_K1, _T1, _HF, _EqK, _Al>&,
      const hash_map<_K1, _T1, _HF, _EqK, _Al>&);

      iterator
      begin()
      { return _M_ht.begin(); }

      iterator
      end()
      { return _M_ht.end(); }

      const_iterator
      begin() const
      { return _M_ht.begin(); }

      const_iterator
      end() const
      { return _M_ht.end(); }

    public:
      pair<iterator, bool>
      insert(const value_type& __obj)
      { return _M_ht.insert_unique(__obj); }

      template <class _InputIterator>
        void
        insert(_InputIterator __f, _InputIterator __l)
        { _M_ht.insert_unique(__f, __l); }

      pair<iterator, bool>
      insert_noresize(const value_type& __obj)
      { return _M_ht.insert_unique_noresize(__obj); }

      iterator
      find(const key_type& __key)
      { return _M_ht.find(__key); }

      const_iterator
      find(const key_type& __key) const
      { return _M_ht.find(__key); }

      _Tp&
      operator[](const key_type& __key)
      { return _M_ht.find_or_insert(value_type(__key, _Tp())).second; }

      size_type
      count(const key_type& __key) const
      { return _M_ht.count(__key); }

      pair<iterator, iterator>
      equal_range(const key_type& __key)
      { return _M_ht.equal_range(__key); }

      pair<const_iterator, const_iterator>
      equal_range(const key_type& __key) const
      { return _M_ht.equal_range(__key); }

      size_type
      erase(const key_type& __key)
      {return _M_ht.erase(__key); }

      void
      erase(iterator __it)
      { _M_ht.erase(__it); }

      void
      erase(iterator __f, iterator __l)
      { _M_ht.erase(__f, __l); }

      void
      clear()
      { _M_ht.clear(); }

      void
      resize(size_type __hint)
      { _M_ht.resize(__hint); }

      size_type
      bucket_count() const
      { return _M_ht.bucket_count(); }

      size_type
      max_bucket_count() const
      { return _M_ht.max_bucket_count(); }

      size_type
      elems_in_bucket(size_type __n) const
      { return _M_ht.elems_in_bucket(__n); }
    };

  template <class _Key, class _Tp, class _HashFcn, class _EqlKey, class _Alloc>
    inline bool
    operator==(const hash_map<_Key, _Tp, _HashFcn, _EqlKey, _Alloc>& __hm1,
        const hash_map<_Key, _Tp, _HashFcn, _EqlKey, _Alloc>& __hm2)
    { return __hm1._M_ht == __hm2._M_ht; }

  template <class _Key, class _Tp, class _HashFcn, class _EqlKey, class _Alloc>
    inline bool
    operator!=(const hash_map<_Key, _Tp, _HashFcn, _EqlKey, _Alloc>& __hm1,
        const hash_map<_Key, _Tp, _HashFcn, _EqlKey, _Alloc>& __hm2)
    { return !(__hm1 == __hm2); }

  template <class _Key, class _Tp, class _HashFcn, class _EqlKey, class _Alloc>
    inline void
    swap(hash_map<_Key, _Tp, _HashFcn, _EqlKey, _Alloc>& __hm1,
  hash_map<_Key, _Tp, _HashFcn, _EqlKey, _Alloc>& __hm2)
    { __hm1.swap(__hm2); }



  template <class _Key, class _Tp,
     class _HashFcn = hash<_Key>,
     class _EqualKey = equal_to<_Key>,
     class _Alloc = allocator<_Tp> >
    class hash_multimap;

  template <class _Key, class _Tp, class _HF, class _EqKey, class _Alloc>
    inline bool
    operator==(const hash_multimap<_Key, _Tp, _HF, _EqKey, _Alloc>& __hm1,
        const hash_multimap<_Key, _Tp, _HF, _EqKey, _Alloc>& __hm2);


  template <class _Key, class _Tp, class _HashFcn, class _EqualKey,
     class _Alloc>
    class hash_multimap
    {

    private:
      typedef hashtable<pair<const _Key, _Tp>, _Key, _HashFcn,
   _Select1st<pair<const _Key, _Tp> >, _EqualKey, _Alloc>
          _Ht;

      _Ht _M_ht;

    public:
      typedef typename _Ht::key_type key_type;
      typedef _Tp data_type;
      typedef _Tp mapped_type;
      typedef typename _Ht::value_type value_type;
      typedef typename _Ht::hasher hasher;
      typedef typename _Ht::key_equal key_equal;

      typedef typename _Ht::size_type size_type;
      typedef typename _Ht::difference_type difference_type;
      typedef typename _Ht::pointer pointer;
      typedef typename _Ht::const_pointer const_pointer;
      typedef typename _Ht::reference reference;
      typedef typename _Ht::const_reference const_reference;

      typedef typename _Ht::iterator iterator;
      typedef typename _Ht::const_iterator const_iterator;

      typedef typename _Ht::allocator_type allocator_type;

      hasher
      hash_funct() const
      { return _M_ht.hash_funct(); }

      key_equal
      key_eq() const
      { return _M_ht.key_eq(); }

      allocator_type
      get_allocator() const
      { return _M_ht.get_allocator(); }

    public:
      hash_multimap()
      : _M_ht(100, hasher(), key_equal(), allocator_type()) {}

      explicit
      hash_multimap(size_type __n)
      : _M_ht(__n, hasher(), key_equal(), allocator_type()) {}

      hash_multimap(size_type __n, const hasher& __hf)
      : _M_ht(__n, __hf, key_equal(), allocator_type()) {}

      hash_multimap(size_type __n, const hasher& __hf, const key_equal& __eql,
      const allocator_type& __a = allocator_type())
      : _M_ht(__n, __hf, __eql, __a) {}

      template <class _InputIterator>
        hash_multimap(_InputIterator __f, _InputIterator __l)
 : _M_ht(100, hasher(), key_equal(), allocator_type())
        { _M_ht.insert_equal(__f, __l); }

      template <class _InputIterator>
        hash_multimap(_InputIterator __f, _InputIterator __l, size_type __n)
 : _M_ht(__n, hasher(), key_equal(), allocator_type())
        { _M_ht.insert_equal(__f, __l); }

      template <class _InputIterator>
        hash_multimap(_InputIterator __f, _InputIterator __l, size_type __n,
        const hasher& __hf)
 : _M_ht(__n, __hf, key_equal(), allocator_type())
        { _M_ht.insert_equal(__f, __l); }

      template <class _InputIterator>
        hash_multimap(_InputIterator __f, _InputIterator __l, size_type __n,
        const hasher& __hf, const key_equal& __eql,
        const allocator_type& __a = allocator_type())
 : _M_ht(__n, __hf, __eql, __a)
        { _M_ht.insert_equal(__f, __l); }

    public:
      size_type
      size() const
      { return _M_ht.size(); }

      size_type
      max_size() const
      { return _M_ht.max_size(); }

      bool
      empty() const
      { return _M_ht.empty(); }

      void
      swap(hash_multimap& __hs)
      { _M_ht.swap(__hs._M_ht); }

      template <class _K1, class _T1, class _HF, class _EqK, class _Al>
        friend bool
        operator==(const hash_multimap<_K1, _T1, _HF, _EqK, _Al>&,
     const hash_multimap<_K1, _T1, _HF, _EqK, _Al>&);

      iterator
      begin()
      { return _M_ht.begin(); }

      iterator
      end()
      { return _M_ht.end(); }

      const_iterator
      begin() const
      { return _M_ht.begin(); }

      const_iterator
      end() const
      { return _M_ht.end(); }

public:
      iterator
      insert(const value_type& __obj)
      { return _M_ht.insert_equal(__obj); }

      template <class _InputIterator>
        void
        insert(_InputIterator __f, _InputIterator __l)
        { _M_ht.insert_equal(__f,__l); }

      iterator
      insert_noresize(const value_type& __obj)
      { return _M_ht.insert_equal_noresize(__obj); }

      iterator
      find(const key_type& __key)
      { return _M_ht.find(__key); }

      const_iterator
      find(const key_type& __key) const
      { return _M_ht.find(__key); }

      size_type
      count(const key_type& __key) const
      { return _M_ht.count(__key); }

      pair<iterator, iterator>
      equal_range(const key_type& __key)
      { return _M_ht.equal_range(__key); }

      pair<const_iterator, const_iterator>
      equal_range(const key_type& __key) const
      { return _M_ht.equal_range(__key); }

      size_type
      erase(const key_type& __key)
      { return _M_ht.erase(__key); }

      void
      erase(iterator __it)
      { _M_ht.erase(__it); }

      void
      erase(iterator __f, iterator __l)
      { _M_ht.erase(__f, __l); }

      void
      clear()
      { _M_ht.clear(); }

    public:
      void
      resize(size_type __hint)
      { _M_ht.resize(__hint); }

      size_type
      bucket_count() const
      { return _M_ht.bucket_count(); }

      size_type
      max_bucket_count() const
      { return _M_ht.max_bucket_count(); }

      size_type
      elems_in_bucket(size_type __n) const
      { return _M_ht.elems_in_bucket(__n); }
};

  template <class _Key, class _Tp, class _HF, class _EqKey, class _Alloc>
    inline bool
    operator==(const hash_multimap<_Key, _Tp, _HF, _EqKey, _Alloc>& __hm1,
        const hash_multimap<_Key, _Tp, _HF, _EqKey, _Alloc>& __hm2)
    { return __hm1._M_ht == __hm2._M_ht; }

  template <class _Key, class _Tp, class _HF, class _EqKey, class _Alloc>
    inline bool
    operator!=(const hash_multimap<_Key, _Tp, _HF, _EqKey, _Alloc>& __hm1,
        const hash_multimap<_Key, _Tp, _HF, _EqKey, _Alloc>& __hm2)
    { return !(__hm1 == __hm2); }

  template <class _Key, class _Tp, class _HashFcn, class _EqlKey, class _Alloc>
    inline void
    swap(hash_multimap<_Key, _Tp, _HashFcn, _EqlKey, _Alloc>& __hm1,
  hash_multimap<_Key, _Tp, _HashFcn, _EqlKey, _Alloc>& __hm2)
    { __hm1.swap(__hm2); }

}



using __gnu_cxx::hash;
using __gnu_cxx::hashtable;
using __gnu_cxx::hash_map;
using __gnu_cxx::hash_multimap;

#endif
