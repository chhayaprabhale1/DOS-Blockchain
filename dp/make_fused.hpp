#pragma once
#include <dp/tuple.hpp>
#include <dp/function.hpp>

namespace dp {
   template<typename R>
   std::function<R, dp::tuple<> > make_fused( const std::function<R>& f ) {
      return [=]( dp::tuple<> ){ return f(); };
   }
   template<typename R,typename A>
   std::function<R(dp::tuple<A>) > make_fused( const std::function<R(A)>& f ) {
      return [f]( dp::tuple<A> t){ return f(t.a); };
   }
   template<typename R,typename A,typename B>
   std::function<R(dp::tuple<A,B>) > make_fused( const std::function<R(A,B)>& f ) {
      return [f]( dp::tuple<A,B> t){ return f(t.a,t.b); };
   }
   template<typename R,typename A,typename B,typename C>
   std::function<R(dp::tuple<A,B,C>) > make_fused( const std::function<R(A,B,C)>& f ) {
      return [f]( dp::tuple<A,B,C> t){ return f(t.a,t.b,t.c); };
   }
   template<typename R,typename A,typename B,typename C,typename D>
   std::function<R(dp::tuple<A,B,C,D>) > make_fused( const std::function<R(A,B,C,D)>& f ) {
      return [f]( dp::tuple<A,B,C> t){ return f(t.a,t.b,t.c,t.d); };
   }
}
