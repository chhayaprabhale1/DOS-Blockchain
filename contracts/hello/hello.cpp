#include <dosiolib/dosio.hpp>
using namespace dosio;

class hello : public dosio::contract {
  public:
      using contract::contract;

      /// @abi action 
      void hi( account_name user ) {
         print( "Hello, ", name{user} );
      }
};

DOSIO_ABI( hello, (hi) )
