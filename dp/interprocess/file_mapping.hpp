#pragma once
#include <dp/fwd.hpp>
#include <dp/utility.hpp>

namespace boost {
  namespace interprocess {
    class file_mapping;
    class mapped_region;
  }
}
namespace dp {
  enum mode_t {
    read_only, 
    write_only,
    read_write
  };

  class file_mapping {
    public:
      file_mapping( const char* file, mode_t );
      ~file_mapping();
    private:
      friend class mapped_region;
    #ifdef _WIN64
      dp::fwd<boost::interprocess::file_mapping,0x38> my;
    #else
      dp::fwd<boost::interprocess::file_mapping,0x24> my;
    #endif
  };

  class mapped_region {
    public:
      mapped_region( const file_mapping& fm, mode_t m, uint64_t start, size_t size );
      mapped_region( const file_mapping& fm, mode_t m );
      ~mapped_region();
      void  flush();
      void* get_address()const;
      size_t get_size()const;
    private:
      dp::fwd<boost::interprocess::mapped_region,40> my;
  };
}
