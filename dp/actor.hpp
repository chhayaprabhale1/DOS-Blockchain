#pragma once
#include <dp/api.hpp>
#include <dp/thread/thread.hpp>

namespace dp {

  namespace detail {
    struct actor_member { 
       #if 1 // BOOST_NO_VARIADIC_TEMPLATES
         #define RPC_MEMBER_FUNCTOR(z,n,IS_CONST) \
         template<typename R, typename C, typename P BOOST_PP_ENUM_TRAILING_PARAMS( n, typename A)> \
         static std::function<dp::future<R>( BOOST_PP_ENUM_PARAMS(n,A) ) > \
             functor( P p, R (C::*mem_func)(BOOST_PP_ENUM_PARAMS(n,A)) IS_CONST, dp::thread* c = 0)  { \
             return [=](BOOST_PP_ENUM_BINARY_PARAMS(n,A,a))->dp::future<R>{ \
                 return c->async( [=](){ return (p->*mem_func)(BOOST_PP_ENUM_PARAMS(n,a)); } ); }; \
         } 
         BOOST_PP_REPEAT( 8, RPC_MEMBER_FUNCTOR, const )
         BOOST_PP_REPEAT( 8, RPC_MEMBER_FUNCTOR, BOOST_PP_EMPTY() )
         #undef RPC_MEMBER_FUNCTOR

       #else  // g++ has a bug that prevents lambdas and varidic templates from working together (G++ Bug 41933)

         template<typename R, typename C, typename P, typename... Args>
         static std::function<dp::future<R>(Args...)> functor( P&& p, R (C::*mem_func)(Args...), dp::thread* c ) {
            return [=](Args... args)->dp::future<R>{  c->async( [=]()->R { return p->*mem_func( dp::forward<Args>(args)... ); } ) };
         }
         template<typename R, typename C, typename P, typename... Args>
         static std::function<dp::future<R>(Args...)> functor( P&& p, R (C::*mem_func)(Args...)const, dp::thread* c ){
            return [=](Args... args)->dp::future<R>{  c->async( [=]()->R { return p->*mem_func( dp::forward<Args>(args)... ); } ) };
         }
       #endif
    };

    template<typename ThisPtr>
    struct actor_vtable_visitor {
        template<typename U>
        actor_vtable_visitor( dp::thread* t, U&& u ):_thread(t),_this( dp::forward<U>(u) ){}

        template<typename Function, typename MemberPtr>
        void operator()( const char* name, Function& memb, MemberPtr m )const {
          memb = actor_member::functor( _this, m, _thread );
        }
        dp::thread* _thread;
        ThisPtr _this;
    };
  }

  /**
   *  Posts all method calls to another thread and
   *  returns a future.
   */
  template<typename Interface>
  class actor : public api<Interface, detail::actor_member> {
    public:
      actor(){}

      template<typename InterfaceType>
      actor( InterfaceType* p, dp::thread* t = &dp::thread::current() )
      {
          this->_vtable.reset(new detail::vtable<Interface,detail::actor_member>() );
          this->_vtable->template visit<InterfaceType>( detail::actor_vtable_visitor<InterfaceType*>(t, p) );
      }
  };

} // namespace dp
