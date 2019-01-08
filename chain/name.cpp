#include <chain/include/name.hpp>
#include <dp/variant.hpp>
#include <boost/algorithm/string.hpp>
#include <dp/exception/exception.hpp>
#include <chain/include/exceptions.hpp>

namespace dosio { namespace chain { 

   void name::set( const char* str ) {
      const auto len = strnlen(str, 14);
      DOS_ASSERT(len <= 13, name_type_exception, "Name is longer than 13 characters (${name}) ", ("name", string(str)));
      value = string_to_name(str);
      DOS_ASSERT(to_string() == string(str), name_type_exception,
                 "Name not properly normalized (name: ${name}, normalized: ${normalized}) ",
                 ("name", string(str))("normalized", to_string()));
   }

   // keep in sync with name::to_string() in contract definition for name
   name::operator string()const {
     static const char* charmap = ".12345abcdefghijklmnopqrstuvwxyz";

      string str(13,'.');

      uint64_t tmp = value;
      for( uint32_t i = 0; i <= 12; ++i ) {
         char c = charmap[tmp & (i == 0 ? 0x0f : 0x1f)];
         str[12-i] = c;
         tmp >>= (i == 0 ? 4 : 5);
      }

      boost::algorithm::trim_right_if( str, []( char c ){ return c == '.'; } );
      return str;
   }

} } 

namespace dp {
  void to_variant(const dosio::chain::name& c, dp::variant& v) { v = std::string(c); }
  void from_variant(const dp::variant& v, dosio::chain::name& check) { check = v.get_string(); }
} 
