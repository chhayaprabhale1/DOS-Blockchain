/**
 *  @file
 *  @copyright defined in dos/LICENSE
 */

#include <dosiolib/dosio.hpp>

namespace asserter {
   struct assertdef {
      int8_t      condition;
      std::string message;

      DOSLIB_SERIALIZE( assertdef, (condition)(message) )
   };
}
