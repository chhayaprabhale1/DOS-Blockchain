#pragma once
#include <dp/interprocess/iprocess.hpp>

namespace dp {

  dp::path find_executable_in_path( const dp::string name );

  /**
   *  @brief start and manage an local process
   *  @note this class implements reference semantics.
   */
  class process  : public iprocess
  {
    public:
      process();
      ~process();

      virtual iprocess& exec( const dp::path&  exe, 
                              std::vector<std::string>   args, 
                              const dp::path&  work_dir = dp::path(), 
                              int              opts     = open_all    );

      
      virtual int                        result(const microseconds& timeout = microseconds::maximum());
      virtual void                       kill();
      virtual dp::buffered_ostream_ptr   in_stream();
      virtual dp::buffered_istream_ptr   out_stream();
      virtual dp::buffered_istream_ptr   err_stream();

      class impl;
    private:
      std::unique_ptr<impl> my;
  };

  typedef std::shared_ptr<process> process_ptr;

} // namespace dp
