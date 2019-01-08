#include <chain/include/chain_id_type.hpp>
#include <chain/include/exceptions.hpp>

namespace dosio { namespace chain {

   void chain_id_type::reflector_verify()const {
      DOS_ASSERT( *reinterpret_cast<const dp::sha256*>(this) != dp::sha256(), chain_id_type_exception, "chain_id_type cannot be zero" );
   }

} }

namespace dp {

   void to_variant(const chain::chain_id_type& cid, dp::variant& v) {
      to_variant( static_cast<const fc::sha256&>(cid), v);
   }

   void from_variant(const dp::variant& v, chain::chain_id_type& cid) {
      from_variant( v, static_cast<dp::sha256&>(cid) );
   }

}
