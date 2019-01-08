#pragma once
/*
#include <dp/io/iostream.hpp>
#include <dp/fwd.hpp>

namespace dp {

  class stringstream : virtual public iostream {
    public:
      stringstream();
      stringstream( dp::string& s);
      stringstream( const dp::string& s);
      ~stringstream();

      dp::string str();
      void str(const dp::string& s);

      void clear();

      virtual bool     eof()const;
      virtual size_t   writesome( const char* buf, size_t len );
      virtual size_t   writesome( const std::shared_ptr<const char>& buf, size_t len, size_t offset );
      virtual size_t   readsome( char* buf, size_t len );
      virtual size_t   readsome( const std::shared_ptr<char>& buf, size_t len, size_t offset );
      virtual void     close();
      virtual void     flush();
              char     peek();

    private:
      class impl;
      fwd<impl, 392> my;
  };

}
*/
