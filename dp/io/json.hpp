#pragma once
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/variant.hpp>
#include </home/cyperts/Desktop/Desktoa/DOS-Blockchain/dp/filesystem.hpp>

#define DEFAULT_MAX_RECURSION_DEPTH 200

namespace dp
{
   using std::ostream;

   /**
    *  Provides interface for json serialization.
    *
    *  json strings are always UTF8
    */
   class json
   {
      public:
         enum parse_type
         {
            legacy_parser         = 0,
            strict_parser         = 1,
            relaxed_parser        = 2,
            legacy_parser_with_string_doubles = 3
         };
         enum output_formatting
         {
            stringify_large_ints_and_doubles = 0,
            legacy_generator = 1
         };

         static ostream& to_stream( ostream& out, const dp::string&);
         static ostream& to_stream( ostream& out, const variant& v, output_formatting format = stringify_large_ints_and_doubles );
         static ostream& to_stream( ostream& out, const variants& v, output_formatting format = stringify_large_ints_and_doubles );
         static ostream& to_stream( ostream& out, const variant_object& v, output_formatting format = stringify_large_ints_and_doubles );

         static variant  from_string( const string& utf8_str, parse_type ptype = legacy_parser, uint32_t max_depth = DEFAULT_MAX_RECURSION_DEPTH );
         static variants variants_from_string( const string& utf8_str, parse_type ptype = legacy_parser, uint32_t max_depth = DEFAULT_MAX_RECURSION_DEPTH );
         static string   to_string( const variant& v, output_formatting format = stringify_large_ints_and_doubles );
         static string   to_pretty_string( const variant& v, output_formatting format = stringify_large_ints_and_doubles );

         static bool     is_valid( const std::string& json_str, parse_type ptype = legacy_parser, uint32_t max_depth = DEFAULT_MAX_RECURSION_DEPTH );

         template<typename T>
         static void     save_to_file( const T& v, const dp::path& fi, bool pretty = true, output_formatting format = stringify_large_ints_and_doubles )
         {
            save_to_file( variant(v), fi, pretty, format );
         }

         static void     save_to_file( const variant& v, const dp::path& fi, bool pretty = true, output_formatting format = stringify_large_ints_and_doubles );
         static variant  from_file( const dp::path& p, parse_type ptype = legacy_parser, uint32_t max_depth = DEFAULT_MAX_RECURSION_DEPTH );

         template<typename T>
         static T from_file( const dp::path& p, parse_type ptype = legacy_parser, uint32_t max_depth = DEFAULT_MAX_RECURSION_DEPTH )
         {
            return json::from_file(p, ptype, max_depth).as<T>();
         }

         template<typename T>
         static string   to_string( const T& v, output_formatting format = stringify_large_ints_and_doubles )
         {
            return to_string( variant(v), format );
         }

         template<typename T>
         static string   to_pretty_string( const T& v, output_formatting format = stringify_large_ints_and_doubles ) 
         {
            return to_pretty_string( variant(v), format );
         }

         template<typename T>
         static void save_to_file( const T& v, const std::string& p, bool pretty = true, output_formatting format = stringify_large_ints_and_doubles ) 
         {
            save_to_file( variant(v), dp::path(p), pretty, format );
         } 
   };

} // dp

#undef DEFAULT_MAX_RECURSION_DEPTH
