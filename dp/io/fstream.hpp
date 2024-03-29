#include <dp/filesystem.hpp>
#include <string>

namespace dp {
  /**
   * Grab the full contents of a file into a string object.
   * NB reading a full file into memory is a poor choice
   * if the file may be very large.
   */
  void read_file_contents( const dp::path& filename, std::string& result );
}
