#pragma once
#include <dp/string.hpp>
#include <dp/time.hpp>
#include <dp/shared_ptr.hpp>
#include <dp/log/log_message.hpp>

namespace dp  
{

   class appender;

   /**
    *
    *
    @code
      void my_class::func() 
      {
         dp_dlog( my_class_logger, "Format four: ${arg}  five: ${five}", ("arg",4)("five",5) );
      }
    @endcode
    */
   class logger 
   {
      public:
         static logger get( const dp::string& name = "default");

         logger();
         logger( const string& name, const logger& parent = nullptr );
         logger( std::nullptr_t );
         logger( const logger& c );
         logger( logger&& c );
         ~logger();
         logger& operator=(const logger&);
         logger& operator=(logger&&);
         friend bool operator==( const logger&, nullptr_t );
         friend bool operator!=( const logger&, nullptr_t );

         logger&    set_log_level( log_level e );
         log_level  get_log_level()const;
         logger&    set_parent( const logger& l );
         logger     get_parent()const;

         void  set_name( const dp::string& n );
         const dp::string& name()const;

         void add_appender( const dp::shared_ptr<appender>& a );
         std::vector<dp::shared_ptr<appender> > get_appenders()const;
         void remove_appender( const dp::shared_ptr<appender>& a );

         bool is_enabled( log_level e )const;
         void log( log_message m );

      private:
         class impl;
         dp::shared_ptr<impl> my;
   };

} // namespace dp

#ifndef DEFAULT_LOGGER
#define DEFAULT_LOGGER
#endif

// suppress warning "conditional expression is constant" in the while(0) for visual c++
// http://cnicholson.net/2009/03/stupid-c-tricks-dowhile0-and-c4127/
#define DP_MULTILINE_MACRO_BEGIN do {
#ifdef _MSC_VER
# define DP_MULTILINE_MACRO_END \
    __pragma(warning(push)) \
    __pragma(warning(disable:4127)) \
    } while (0) \
    __pragma(warning(pop))
#else
# define DP_MULTILINE_MACRO_END  } while (0)
#endif

#define dp_dlog( LOGGER, FORMAT, ... ) \
  DP_MULTILINE_MACRO_BEGIN \
   if( (LOGGER).is_enabled( dp::log_level::debug ) ) \
      (LOGGER).log( DP_LOG_MESSAGE( debug, FORMAT, __VA_ARGS__ ) ); \
  DP_MULTILINE_MACRO_END

#define dp_ilog( LOGGER, FORMAT, ... ) \
  DP_MULTILINE_MACRO_BEGIN \
   if( (LOGGER).is_enabled( dp::log_level::info ) ) \
      (LOGGER).log( DP_LOG_MESSAGE( info, FORMAT, __VA_ARGS__ ) ); \
  DP_MULTILINE_MACRO_END

#define dp_wlog( LOGGER, FORMAT, ... ) \
  DP_MULTILINE_MACRO_BEGIN \
   if( (LOGGER).is_enabled( dp::log_level::warn ) ) \
      (LOGGER).log( DP_LOG_MESSAGE( warn, FORMAT, __VA_ARGS__ ) ); \
  DP_MULTILINE_MACRO_END

#define dp_elog( LOGGER, FORMAT, ... ) \
  DP_MULTILINE_MACRO_BEGIN \
   if( (LOGGER).is_enabled( dp::log_level::error ) ) \
      (LOGGER).log( DP_LOG_MESSAGE( error, FORMAT, __VA_ARGS__ ) ); \
  DP_MULTILINE_MACRO_END

#define dlog( FORMAT, ... ) \
  DP_MULTILINE_MACRO_BEGIN \
   if( (dp::logger::get(DEFAULT_LOGGER)).is_enabled( dp::log_level::debug ) ) \
      (dp::logger::get(DEFAULT_LOGGER)).log( DP_LOG_MESSAGE( debug, FORMAT, __VA_ARGS__ ) ); \
  DP_MULTILINE_MACRO_END

/**
 * Sends the log message to a special 'user' log stream designed for messages that
 * the end user may like to see.
 */
#define ulog( FORMAT, ... ) \
  DP_MULTILINE_MACRO_BEGIN \
   if( (dp::logger::get("user")).is_enabled( dp::log_level::debug ) ) \
      (dp::logger::get("user")).log( DP_LOG_MESSAGE( debug, FORMAT, __VA_ARGS__ ) ); \
  DP_MULTILINE_MACRO_END


#define ilog( FORMAT, ... ) \
  DP_MULTILINE_MACRO_BEGIN \
   if( (dp::logger::get(DEFAULT_LOGGER)).is_enabled( dp::log_level::info ) ) \
      (dp::logger::get(DEFAULT_LOGGER)).log( DP_LOG_MESSAGE( info, FORMAT, __VA_ARGS__ ) ); \
  DP_MULTILINE_MACRO_END

#define wlog( FORMAT, ... ) \
  DP_MULTILINE_MACRO_BEGIN \
   if( (dp::logger::get(DEFAULT_LOGGER)).is_enabled( dp::log_level::warn ) ) \
      (dp::logger::get(DEFAULT_LOGGER)).log( DP_LOG_MESSAGE( warn, FORMAT, __VA_ARGS__ ) ); \
  DP_MULTILINE_MACRO_END

#define elog( FORMAT, ... ) \
  DP_MULTILINE_MACRO_BEGIN \
   if( (dp::logger::get(DEFAULT_LOGGER)).is_enabled( dp::log_level::error ) ) \
      (dp::logger::get(DEFAULT_LOGGER)).log( DP_LOG_MESSAGE( error, FORMAT, __VA_ARGS__ ) ); \
  DP_MULTILINE_MACRO_END

#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/seq/seq.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/punctuation/paren.hpp>


#define DP_FORMAT_ARG(r, unused, base) \
  BOOST_PP_STRINGIZE(base) ": ${" BOOST_PP_STRINGIZE( base ) "} "

#define DP_FORMAT_ARGS(r, unused, base) \
  BOOST_PP_LPAREN() BOOST_PP_STRINGIZE(base),dp::variant(base) BOOST_PP_RPAREN()

#define DP_FORMAT( SEQ )\
    BOOST_PP_SEQ_FOR_EACH( DP_FORMAT_ARG, v, SEQ ) 

// takes a ... instead of a SEQ arg because it can be called with an empty SEQ 
// from DP_CAPTURE_AND_THROW()
#define DP_FORMAT_ARG_PARAMS( ... )\
    BOOST_PP_SEQ_FOR_EACH( DP_FORMAT_ARGS, v, __VA_ARGS__ ) 

#define idump( SEQ ) \
    ilog( DP_FORMAT(SEQ), DP_FORMAT_ARG_PARAMS(SEQ) )  
#define wdump( SEQ ) \
    wlog( DP_FORMAT(SEQ), DP_FORMAT_ARG_PARAMS(SEQ) )  
#define edump( SEQ ) \
    elog( DP_FORMAT(SEQ), DP_FORMAT_ARG_PARAMS(SEQ) )  

// this disables all normal logging statements -- not something you'd normally want to do,
// but it's useful if you're benchmarking something and suspect logging is causing
// a slowdown.
#ifdef DP_DISABLE_LOGGING
# undef ulog
# define ulog(...) DP_MULTILINE_MACRO_BEGIN DP_MULTILINE_MACRO_END
# undef elog
# define elog(...) DP_MULTILINE_MACRO_BEGIN DP_MULTILINE_MACRO_END
# undef wlog
# define wlog(...) DP_MULTILINE_MACRO_BEGIN DP_MULTILINE_MACRO_END
# undef ilog
# define ilog(...) DP_MULTILINE_MACRO_BEGIN DP_MULTILINE_MACRO_END
# undef dlog
# define dlog(...) DP_MULTILINE_MACRO_BEGIN DP_MULTILINE_MACRO_END
#endif