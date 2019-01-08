#include <chain/include/block.hpp>
#include <chain/include/merkle.hpp>
#include <dp/io/raw.hpp>
#include <dp/bitutil.hpp>
#include <algorithm>

namespace dosio { namespace chain {
   digest_type block_header::digest()const
   {
      return digest_type::hash(*this);
   }

   uint32_t block_header::num_from_id(const block_id_type& id)
   {
      return dp::endian_reverse_u32(id._hash[0]);
   }

   block_id_type block_header::id()const
   {
      // Do not include signed_block_header attributes in id, specifically exclude producer_signature.
      block_id_type result = digest();
      result._hash[0] &= 0xffffffff00000000;
      result._hash[0] += dp::endian_reverse_u32(block_num()); // store the block num in the ID, 160 bits is plenty for the hash
      return result;
   }


} }
