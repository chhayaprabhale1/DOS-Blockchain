#pragma once
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/exception/exception.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/io/raw_fwd.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/variant_object.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/variant.hpp>

namespace dp { namespace raw {

    template<typename Stream>
    class variant_packer : public variant::visitor
    {
       public:
         variant_packer( Stream& _s ):s(_s){}
         virtual void handle()const { }
         virtual void handle( const int64_t& v )const
         {
            dp::raw::pack( s, v );
         }
         virtual void handle( const uint64_t& v )const
         {
            dp::raw::pack( s, v );
         }
         virtual void handle( const double& v )const 
         {
            dp::raw::pack( s, v );
         }
         virtual void handle( const bool& v )const
         {
            dp::raw::pack( s, v );
         }
         virtual void handle( const string& v )const
         {
            dp::raw::pack( s, v );
         }
         virtual void handle( const variant_object& v)const
         {
            dp::raw::pack( s, v );
         }
         virtual void handle( const variants& v)const
         {
            dp::raw::pack( s, v );
         }
        
         Stream& s;
        
    };


    template<typename Stream> 
    inline void pack( Stream& s, const variant& v )
    {
       pack( s, uint8_t(v.get_type()) );
       v.visit( variant_packer<Stream>(s) );
    }
    template<typename Stream> 
    inline void unpack( Stream& s, variant& v )
    {
      uint8_t t;
      unpack( s, t );
      switch( t )
      {
         case variant::null_type:
            return;
         case variant::int64_type:
         {
            int64_t val;
            raw::unpack(s,val);
            v = val;
            return;
         }
         case variant::uint64_type:
         {
            uint64_t val;
            raw::unpack(s,val);
            v = val;
            return;
         }
         case variant::double_type:
         {
            double val;
            raw::unpack(s,val);
            v = val;
            return;
         }
         case variant::bool_type:
         {
            bool val;
            raw::unpack(s,val);
            v = val;
            return;
         }
         case variant::string_type:
         {
            dp::string val;
            raw::unpack(s,val);
            v = dp::move(val);
            return;
         }
         case variant::array_type:
         {
            variants val;
            raw::unpack(s,val);
            v = dp::move(val);
            return;
         }
         case variant::object_type:
         {
            variant_object val; 
            raw::unpack(s,val);
            v = dp::move(val);
            return;
         }
         default:
            DP_THROW_EXCEPTION( parse_error_exception, "Unknown Variant Type ${t}", ("t", t) );
      }
    }

    template<typename Stream> 
    inline void pack( Stream& s, const variant_object& v ) 
    {
       unsigned_int vs = (uint32_t)v.size();
       pack( s, vs );
       for( auto itr = v.begin(); itr != v.end(); ++itr )
       {
          pack( s, itr->key() );
          pack( s, itr->value() );
       }
    }
    template<typename Stream> 
    inline void unpack( Stream& s, variant_object& v ) 
    {
       unsigned_int vs;
       unpack( s, vs );

       mutable_variant_object mvo;
       mvo.reserve(vs.value);
       for( uint32_t i = 0; i < vs.value; ++i )
       {
          dp::string key;
          dp::variant value;
          dp::raw::unpack(s,key);
          dp::raw::unpack(s,value);
          mvo.set( dp::move(key), dp::move(value) );
       }
       v = dp::move(mvo);
    }

} } // dp::raw
