#pragma once
#include <memory>
#include <vector>
#include <dp/crypto/sha1.hpp>
#include <dp/crypto/sha256.hpp>
#include <dp/io/raw_fwd.hpp>
#include <dp/array.hpp>

namespace dp {
    namespace detail { class pke_impl; }

    class private_key;
    class public_key;
    void generate_key_pair( public_key&, private_key& );

    typedef std::vector<char> bytes;
    typedef bytes             signature;

    class public_key
    {
       public:
          public_key();
          explicit public_key( const bytes& d );
          public_key( const public_key& k );
          public_key( public_key&& k );
          ~public_key();

          operator bool()const;

          public_key& operator=(const public_key&  p );
          public_key& operator=(public_key&& p );

          bool verify( const sha1& digest, const array<char,2048/8>& sig )const;
          bool verify( const sha1& digest, const signature& sig )const;
          bool verify( const sha256& digest, const signature& sig )const;
          bytes encrypt( const char* data, size_t len )const;
          bytes encrypt( const bytes& )const;
          bytes decrypt( const bytes& )const;

          bytes serialize()const;
          friend void generate_key_pair( public_key&, private_key& );
       private:
          std::shared_ptr<detail::pke_impl> my;
    };

    class private_key
    {
       public:
          private_key();
          explicit private_key( const bytes& d );
          private_key( const private_key& k );
          private_key( private_key&& k );
          ~private_key();

          operator bool()const;

          private_key& operator=(const private_key&  p );
          private_key& operator=(private_key&& p );

          void sign( const sha1& digest, array<char,2048/8>& sig )const;
          signature sign( const sha1& digest )const;
          signature sign( const sha256& digest )const;

          bytes decrypt( const char* bytes, size_t len )const;
          bytes decrypt( const bytes& )const;
          bytes encrypt( const bytes& )const;

          bytes serialize()const;
          friend void generate_key_pair( public_key&, private_key& );

       private:
          std::shared_ptr<detail::pke_impl> my;
    };
    bool operator==( const private_key& a, const private_key& b );

    namespace raw
    {
        template<typename Stream>
        void unpack( Stream& s, dp::public_key& pk)
        {
            bytes ser;
            dp::raw::unpack(s,ser);
            pk = dp::public_key( ser );
        }

        template<typename Stream>
        void pack( Stream& s, const dp::public_key& pk)
        {
            dp::raw::pack( s, pk.serialize() );
        }

        template<typename Stream>
        void unpack( Stream& s, dp::private_key& pk)
        {
            bytes ser;
            dp::raw::unpack(s,ser);
            pk = dp::private_key( ser );
        }

        template<typename Stream>
        void pack( Stream& s, const dp::private_key& pk)
        {
            dp::raw::pack( s, pk.serialize() );
        }
    }
  class variant;
  void to_variant( const public_key& bi, variant& v );
  void from_variant( const variant& v, public_key& bi );
  void to_variant( const private_key& bi, variant& v );
  void from_variant( const variant& v, private_key& bi );

}

