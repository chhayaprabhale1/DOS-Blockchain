#pragma once
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/reflect/reflect.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/variant_object.hpp>

namespace dp
{
   template<typename T>
   void to_variant( const T& o, variant& v );
   template<typename T>
   void from_variant( const variant& v, T& o );


   template<typename T>
   class to_variant_visitor
   {
      public:
         to_variant_visitor( mutable_variant_object& mvo, const T& v )
         :vo(mvo),val(v){}

         template<typename Member, class Class, Member (Class::*member)>
         void operator()( const char* name )const
         {
            this->add(vo,name,(val.*member));
         }

      private:
         template<typename M>
         void add( mutable_variant_object& vo, const char* name, const optional<M>& v )const
         { 
            if( v.valid() )
               vo(name,*v);
         }
         template<typename M>
         void add( mutable_variant_object& vo, const char* name, const M& v )const
         { vo(name,v); }

         mutable_variant_object& vo;
         const T& val;
   };

   template<typename T>
   class from_variant_visitor : reflector_verifier_visitor<T>
   {
      public:
         from_variant_visitor( const variant_object& _vo, T& v )
         :reflector_verifier_visitor<T>(v)
         ,vo(_vo){}

         template<typename Member, class Class, Member (Class::*member)>
         void operator()( const char* name )const
         {
            auto itr = vo.find(name);
            if( itr != vo.end() )
               from_variant( itr->value(), this->obj.*member );
         }

         const variant_object& vo;
   };

   template<typename IsReflected=dp::false_type>
   struct if_enum 
   {
     template<typename T>
     static inline void to_variant( const T& v, dp::variant& vo )
     { 
         mutable_variant_object mvo;
         dp::reflector<T>::visit( to_variant_visitor<T>( mvo, v ) );
         vo = dp::move(mvo);
     }
     template<typename T>
     static inline void from_variant( const dp::variant& v, T& o )
     { 
         const variant_object& vo = v.get_object();
         dp::reflector<T>::visit( from_variant_visitor<T>( vo, o ) );
     }
   };

    template<> 
    struct if_enum<dp::true_type>
    {
       template<typename T>
       static inline void to_variant( const T& o, dp::variant& v )
       { 
           v = dp::reflector<T>::to_dp_string(o);
       }
       template<typename T>
       static inline void from_variant( const dp::variant& v, T& o )
       { 
           if( v.is_string() )
              o = dp::reflector<T>::from_string( v.get_string().c_str() );
           else
              o = dp::reflector<T>::from_int( v.as_int64() );
       }
    };


   template<typename T>
   void to_variant( const T& o, variant& v )
   {
      if_enum<typename dp::reflector<T>::is_enum>::to_variant( o, v );
   }

   template<typename T>
   void from_variant( const variant& v, T& o )
   {
      if_enum<typename dp::reflector<T>::is_enum>::from_variant( v, o );
   }

}
