#include <dosiolib/dosio.hpp>
#include <dosiolib/print.hpp>
using namespace dosio;

class payloadless : public dosio::contract {
  public:
      using contract::contract;

      void doit() {
         print( "Im a payloadless action" );
      }
};

DOSIO_ABI( payloadless, (doit) )
