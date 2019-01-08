#pragma once
#include <dp/any.hpp>
#include <dp/shared_ptr.hpp>
#include <dp/string.hpp>

#if BOOST_VERSION >= 106600
namespace boost { namespace asio { class io_context; typedef io_context io_service; } }
#else
namespace boost { namespace asio { class io_service; } }
#endif

namespace dp {
   class appender;
   class log_message;
   class variant;

   class appender_factory : public dp::retainable {
      public:
       typedef dp::shared_ptr<appender_factory> ptr;

       virtual ~appender_factory(){};
       virtual dp::shared_ptr<appender> create( const variant& args ) = 0;
   };

   namespace detail {
      template<typename T>
      class appender_factory_impl : public appender_factory {
        public:
           virtual dp::shared_ptr<appender> create( const variant& args ) {
              return dp::shared_ptr<appender>(new T(args));
           }
      };
   }

   class appender : public dp::retainable {
      public:
         typedef dp::shared_ptr<appender> ptr;

         template<typename T>
         static bool register_appender(const dp::string& type) {
            return register_appender( type, new detail::appender_factory_impl<T>() );
         }

         static appender::ptr create( const dp::string& name, const dp::string& type, const variant& args  );
         static appender::ptr get( const dp::string& name );
         static bool          register_appender( const dp::string& type, const appender_factory::ptr& f );

         virtual void initialize( boost::asio::io_service& io_service ) = 0;
         virtual void log( const log_message& m ) = 0;
   };
}
