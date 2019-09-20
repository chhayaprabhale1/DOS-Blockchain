/**
 *  @file
 *  @copyright defined in dos/LICENSE
 */

#include <dosiolib/dosio.hpp>

namespace dosio {

   class noop: public contract {
      public:
         noop( account_name self ): contract( self ) { }
         void anyaction( account_name from,
                         const std::string& /*type*/,
                         const std::string& /*data*/ )
         {
            require_auth( from );
         }
   };

   DOSIO_ABI( noop, ( anyaction ) )

} /// dosio     
