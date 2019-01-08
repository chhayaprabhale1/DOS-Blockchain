#pragma once
#include <dp/string.hpp>
#include <dp/optional.hpp>
#include <stdint.h>
#include <dp/filesystem.hpp>
#include <dp/variant_object.hpp>
#include <memory>

namespace dp {

  typedef dp::optional<dp::string>           ostring;
  typedef dp::optional<dp::path>             opath;
  typedef dp::optional<dp::variant_object>   ovariant_object;

  namespace detail { class url_impl; }

  class mutable_url;
  
  /**
   *  Used to pass an immutable URL and
   *  query its parts.
   */
  class url 
  {
    public:
      url();
      explicit url( const string& u );
      url( const url& c );
      url( url&& c );
      url( const string& proto, const ostring& host, const ostring& user, const ostring& pass,
           const opath& path, const ostring& query, const ovariant_object& args, const dp::optional<uint16_t>& port);
      ~url();
      
      url& operator=( const url& c );
      url& operator=( url&& c );

      url& operator=( const mutable_url& c );
      url& operator=( mutable_url&& c );
      
      bool operator==( const url& cmp )const;
      
      operator string()const;
      
      //// file, ssh, tcp, http, ssl, etc...
      string                    proto()const; 
      ostring                   host()const;
      ostring                   user()const;
      ostring                   pass()const;
      opath                     path()const;
      ostring                   query()const;
      ovariant_object           args()const;
      dp::optional<uint16_t>    port()const;

    private:
      friend class mutable_url;
      std::shared_ptr<detail::url_impl> my;
  };

  void to_variant( const url& u, dp::variant& v );
  void from_variant( const dp::variant& v, url& u );

} // namespace dp

