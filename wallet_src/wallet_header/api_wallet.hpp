
#pragma once

#include <dos/chain/types.hpp>
#include <dos/chain/transaction.hpp>

using namespace std;
using namespace dos::chain;

namespace dos { namespace wallet {

class wallet_api
{
   public:
      virtual ~wallet_api() {}

      virtual private_key_type get_private_key( public_key_type pubkey ) const = 0;

      virtual bool    is_locked() const = 0;

      virtual void    lock() = 0;

      virtual void    unlock(string password) = 0;

      virtual void    check_password(string password) = 0;

      virtual void    set_password(string password) = 0;

      virtual map<public_key_type, private_key_type> list_keys() = 0;

      virtual flat_set<public_key_type> list_public_keys() = 0;

      virtual bool import_key( string wif_key ) = 0;

      virtual bool remove_key( string key ) = 0;

      virtual string create_key( string key_type ) = 0;

      virtual optional<signature_type> try_sign_digest( const digest_type digest, const public_key_type public_key ) = 0;
};

}}

