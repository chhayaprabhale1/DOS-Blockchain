#pragma once
/**
 * @file dp/reflect.hpp
 *
 * @brief Defines types and macros used to provide reflection.
 *
 */

#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/utility.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/seq/seq.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <stdint.h>
#include <string.h>

#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/reflect/typename.hpp>

namespace dp {

/**
 *  @brief defines visit functions for T
 *  Unless this is specialized, visit() will not be defined for T.
 *
 *  @tparam T - the type that will be visited.
 *
 *  The @ref DP_REFLECT(TYPE,MEMBERS) or DP_STATIC_REFLECT_DERIVED(TYPE,BASES,MEMBERS) macro is used to specialize this
 *  class for your type.
 */
template<typename T>
struct reflector{
    typedef T type;
    typedef dp::false_type is_defined;
    typedef dp::false_type is_enum;

    /**
     *  @tparam Visitor a function object of the form:
     *
     *    @code
     *     struct functor {
     *        template<typename Member, class Class, Member (Class::*member)>
     *        void operator()( const char* name )const;
     *     };
     *    @endcode
     *
     *  If reflection requires a verification (what a constructor might normally assert) then
     *  derive your Visitor from reflector_verifier_visitor and implement a reflector_verify()
     *  on your reflected type.
     *
     *    @code
     *     template<typename Class>
     *     struct functor : reflector_verifier_visitor<Class>  {
     *        functor(Class& _c)
     *        : dp::reflector_verifier_visitor<Class>(_c) {}
     *
     *        template<typename Member, class Class, Member (Class::*member)>
     *        void operator()( const char* name )const;
     *     };
     *    @endcode
     *
     *  If T is an enum then the functor has the following form:
     *    @code
     *     struct functor {
     *        template<int Value>
     *        void operator()( const char* name )const;
     *     };
     *    @endcode
     *
     *  @param v a functor that will be called for each member on T
     *
     *  @note - this method is not defined for non-reflected types.
     */
    #ifdef DOXYGEN
    template<typename Visitor>
    static inline void visit( const Visitor& v );
    #endif // DOXYGEN
};

void throw_bad_enum_cast( int64_t i, const char* e );
void throw_bad_enum_cast( const char* k, const char* e );

template <typename Class>
struct reflector_verifier_visitor {
   explicit reflector_verifier_visitor( Class& c )
     : obj(c) {}

   void reflector_verify() {
      reflect_verify( obj );
   }

 private:

   // int matches 0 if reflector_verify exists SFINAE
   template<class T>
   auto verify_imp(const T& t, int) -> decltype(t.reflector_verify(), void()) {
      t.reflector_verify();
   }

   // if no reflector_verify method exists (SFINAE), 0 matches long
   template<class T>
   auto verify_imp(const T& t, long) -> decltype(t, void()) {}

   template<typename T>
   auto reflect_verify(const T& t) -> decltype(verify_imp(t, 0), void()) {
      verify_imp(t, 0);
   }

 protected:
   Class& obj;
};

} // namespace dp


#ifndef DOXYGEN

#define DP_REFLECT_VISIT_BASE(r, visitor, base) \
  dp::reflector<base>::visit( visitor );


#ifndef _MSC_VER
  #define DP_TEMPLATE template
#else
  // Disable warning C4482: nonstandard extention used: enum 'enum_type::enum_value' used in qualified name
  #pragma warning( disable: 4482 )
  #define DP_TEMPLATE
#endif

#define DP_REFLECT_VISIT_MEMBER( r, visitor, elem ) \
{ typedef decltype((static_cast<type*>(nullptr))->elem) member_type;  \
  visitor.DP_TEMPLATE operator()<member_type,type,&type::elem>( BOOST_PP_STRINGIZE(elem) ); \
}


#define DP_REFLECT_BASE_MEMBER_COUNT( r, OP, elem ) \
  OP DP::reflector<elem>::total_member_count

#define DP_REFLECT_MEMBER_COUNT( r, OP, elem ) \
  OP 1

#define DP_REFLECT_DERIVED_IMPL_INLINE( TYPE, INHERITS, MEMBERS ) \
template<typename Visitor>\
static inline void visit( const Visitor& v ) { \
    BOOST_PP_SEQ_FOR_EACH( DP_REFLECT_VISIT_BASE, v, INHERITS ) \
    BOOST_PP_SEQ_FOR_EACH( DP_REFLECT_VISIT_MEMBER, v, MEMBERS ) \
    verify( v ); \
}

#define DP_REFLECT_DERIVED_IMPL_EXT( TYPE, INHERITS, MEMBERS ) \
template<typename Visitor>\
void dp::reflector<TYPE>::visit( const Visitor& v ) { \
    BOOST_PP_SEQ_FOR_EACH( DP_REFLECT_VISIT_BASE, v, INHERITS ) \
    BOOST_PP_SEQ_FOR_EACH( DP_REFLECT_VISIT_MEMBER, v, MEMBERS ) \
}

#endif // DOXYGEN


#define DP_REFLECT_VISIT_ENUM( r, enum_type, elem ) \
  v.operator()(BOOST_PP_STRINGIZE(elem), int64_t(enum_type::elem) );
#define DP_REFLECT_ENUM_TO_STRING( r, enum_type, elem ) \
   case enum_type::elem: return BOOST_PP_STRINGIZE(elem);
#define DP_REFLECT_ENUM_TO_DP_STRING( r, enum_type, elem ) \
   case enum_type::elem: return dp::string(BOOST_PP_STRINGIZE(elem));

#define DP_REFLECT_ENUM_FROM_STRING( r, enum_type, elem ) \
  if( strcmp( s, BOOST_PP_STRINGIZE(elem)  ) == 0 ) return enum_type::elem;
#define DP_REFLECT_ENUM_FROM_STRING_CASE( r, enum_type, elem ) \
   case enum_type::elem:

#define DP_REFLECT_ENUM( ENUM, FIELDS ) \
namespace dp { \
template<> struct reflector<ENUM> { \
    typedef dp::true_type is_defined; \
    typedef dp::true_type is_enum; \
    static const char* to_string(ENUM elem) { \
      switch( elem ) { \
        BOOST_PP_SEQ_FOR_EACH( DP_REFLECT_ENUM_TO_STRING, ENUM, FIELDS ) \
        default: \
           dp::throw_bad_enum_cast( dp::to_string(int64_t(elem)).c_str(), BOOST_PP_STRINGIZE(ENUM) ); \
      }\
      return nullptr; \
    } \
    static const char* to_string(int64_t i) { \
      return to_string(ENUM(i)); \
    } \
    static dp::string to_dp_string(ENUM elem) { \
      switch( elem ) { \
        BOOST_PP_SEQ_FOR_EACH( DP_REFLECT_ENUM_TO_DP_STRING, ENUM, FIELDS ) \
      } \
      return dp::to_string(int64_t(elem)); \
    } \
    static dp::string to_dp_string(int64_t i) { \
      return to_dp_string(ENUM(i)); \
    } \
    static ENUM from_int(int64_t i) { \
      ENUM e = ENUM(i); \
      switch( e ) \
      { \
        BOOST_PP_SEQ_FOR_EACH( DP_REFLECT_ENUM_FROM_STRING_CASE, ENUM, FIELDS ) \
          break; \
        default: \
          dp::throw_bad_enum_cast( i, BOOST_PP_STRINGIZE(ENUM) ); \
      } \
      return e;\
    } \
    static ENUM from_string( const char* s ) { \
        BOOST_PP_SEQ_FOR_EACH( DP_REFLECT_ENUM_FROM_STRING, ENUM, FIELDS ) \
        int64_t i = 0; \
        try \
        { \
           i = boost::lexical_cast<int64_t>(s); \
        } \
        catch( const boost::bad_lexical_cast& e ) \
        { \
           dp::throw_bad_enum_cast( s, BOOST_PP_STRINGIZE(ENUM) ); \
        } \
        return from_int(i); \
    } \
    template< typename Visitor > \
    static void visit( Visitor& v ) \
    { \
        BOOST_PP_SEQ_FOR_EACH( DP_REFLECT_VISIT_ENUM, ENUM, FIELDS ) \
    } \
};  \
template<> struct get_typename<ENUM>  { static const char* name()  { return BOOST_PP_STRINGIZE(ENUM);  } }; \
}

/*  Note: DP_REFLECT_ENUM previously defined this function, but I don't think it ever
 *        did what we expected it to do.  I've disabled it for now.
 *
 *  template<typename Visitor> \
 *  static inline void visit( const Visitor& v ) { \
 *      BOOST_PP_SEQ_FOR_EACH( DP_REFLECT_VISIT_ENUM, ENUM, FIELDS ) \
 *  }\
 */

/**
 *  @def DP_REFLECT_DERIVED(TYPE,INHERITS,MEMBERS)
 *
 *  @brief Specializes dp::reflector for TYPE where
 *         type inherits other reflected classes
 *
 *  @param INHERITS - a sequence of base class names (basea)(baseb)(basec)
 *  @param MEMBERS - a sequence of member names.  (field1)(field2)(field3)
 */
#define DP_REFLECT_DERIVED( TYPE, INHERITS, MEMBERS ) \
namespace dp {  \
  template<> struct get_typename<TYPE>  { static const char* name()  { return BOOST_PP_STRINGIZE(TYPE);  } }; \
template<> struct reflector<TYPE> {\
    typedef TYPE type; \
    typedef dp::true_type  is_defined; \
    typedef dp::false_type is_enum; \
    template<typename Visitor> \
    static auto verify_imp(const Visitor& v, int) -> decltype(v.reflector_verify(), void()) { \
       v.reflector_verify(); \
    } \
    template<typename Visitor> \
    static auto verify_imp(const Visitor& v, long) -> decltype(v, void()) {} \
    template<typename Visitor> \
    static auto verify(const Visitor& v) -> decltype(verify_imp(v, 0), void()) { \
       verify_imp(v, 0); \
    } \
    enum  member_count_enum {  \
      local_member_count = 0  BOOST_PP_SEQ_FOR_EACH( DP_REFLECT_MEMBER_COUNT, +, MEMBERS ),\
      total_member_count = local_member_count BOOST_PP_SEQ_FOR_EACH( DP_REFLECT_BASE_MEMBER_COUNT, +, INHERITS )\
    }; \
    DP_REFLECT_DERIVED_IMPL_INLINE( TYPE, INHERITS, MEMBERS ) \
}; }
#define DP_REFLECT_DERIVED_TEMPLATE( TEMPLATE_ARGS, TYPE, INHERITS, MEMBERS ) \
namespace dp {  \
  template<BOOST_PP_SEQ_ENUM(TEMPLATE_ARGS)> struct get_typename<TYPE>  { static const char* name()  { return BOOST_PP_STRINGIZE(TYPE);  } }; \
template<BOOST_PP_SEQ_ENUM(TEMPLATE_ARGS)> struct reflector<TYPE> {\
    typedef TYPE type; \
    typedef dp::true_type  is_defined; \
    typedef dp::false_type is_enum; \
    template<typename Visitor> \
    static auto verify_imp(const Visitor& v, int) -> decltype(v.reflector_verify(), void()) { \
       v.reflector_verify(); \
    } \
    template<typename Visitor> \
    static auto verify_imp(const Visitor& v, long) -> decltype(v, void()) {} \
    template<typename Visitor> \
    static auto verify(const Visitor& v) -> decltype(verify_imp(v, 0), void()) { \
       verify_imp(v, 0); \
    } \
    enum  member_count_enum {  \
      local_member_count = 0  BOOST_PP_SEQ_FOR_EACH( DP_REFLECT_MEMBER_COUNT, +, MEMBERS ),\
      total_member_count = local_member_count BOOST_PP_SEQ_FOR_EACH( DP_REFLECT_BASE_MEMBER_COUNT, +, INHERITS )\
    }; \
    DP_REFLECT_DERIVED_IMPL_INLINE( TYPE, INHERITS, MEMBERS ) \
}; }

//BOOST_PP_SEQ_SIZE(MEMBERS),

/**
 *  @def DP_REFLECT(TYPE,MEMBERS)
 *  @brief Specializes DP::reflector for TYPE
 *
 *  @param MEMBERS - a sequence of member names.  (field1)(field2)(field3)
 *
 *  @see DP_REFLECT_DERIVED
 */
#define DP_REFLECT( TYPE, MEMBERS ) \
    DP_REFLECT_DERIVED( TYPE, BOOST_PP_SEQ_NIL, MEMBERS )

#define DP_REFLECT_TEMPLATE( TEMPLATE_ARGS, TYPE, MEMBERS ) \
    DP_REFLECT_DERIVED_TEMPLATE( TEMPLATE_ARGS, TYPE, BOOST_PP_SEQ_NIL, MEMBERS )

#define DP_REFLECT_EMPTY( TYPE ) \
    DP_REFLECT_DERIVED( TYPE, BOOST_PP_SEQ_NIL, BOOST_PP_SEQ_NIL )

#define DP_REFLECT_TYPENAME( TYPE ) \
namespace dp { \
  template<> struct get_typename<TYPE>  { static const char* name()  { return BOOST_PP_STRINGIZE(TYPE);  } }; \
}

#define DP_REFLECT_FWD( TYPE ) \
namespace dp { \
  template<> struct get_typename<TYPE>  { static const char* name()  { return BOOST_PP_STRINGIZE(TYPE);  } }; \
template<> struct reflector<TYPE> {\
    typedef TYPE type; \
    typedef dp::true_type is_defined; \
    enum  member_count_enum {  \
      local_member_count = BOOST_PP_SEQ_SIZE(MEMBERS), \
      total_member_count = local_member_count BOOST_PP_SEQ_FOR_EACH( DP_REFLECT_BASE_MEMBER_COUNT, +, INHERITS )\
    }; \
    template<typename Visitor> static void visit( const Visitor& v ); \
}; }


#define DP_REFLECT_DERIVED_IMPL( TYPE, MEMBERS ) \
    DP_REFLECT_IMPL_DERIVED_EXT( TYPE, BOOST_PP_SEQ_NIL, MEMBERS )

#define DP_REFLECT_IMPL( TYPE, MEMBERS ) \
    DP_REFLECT_DERIVED_IMPL_EXT( TYPE, BOOST_PP_SEQ_NIL, MEMBERS )



