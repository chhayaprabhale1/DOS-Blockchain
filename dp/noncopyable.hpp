#pragma once

namespace dp
{
   class noncopyable
   {
      public:
         noncopyable(){}
      private:
         noncopyable( const noncopyable& ) = delete;
         noncopyable& operator=(  const noncopyable& ) = delete;
   };
}

