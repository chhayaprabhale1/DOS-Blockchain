/**
 *  @file
 *  @copyright defined in dos/LICENSE.txt
 */

#pragma once
#include <dosio/history_plugin/history_plugin.hpp>
#include <dosio/chain_plugin/chain_plugin.hpp>
#include <dosio/http_plugin/http_plugin.hpp>

#include <appbase/application.hpp>

namespace dosio {

   using namespace appbase;

   class history_api_plugin : public plugin<history_api_plugin> {
      public:
        APPBASE_PLUGIN_REQUIRES((history_plugin)(chain_plugin)(http_plugin))

        history_api_plugin();
        virtual ~history_api_plugin();

        virtual void set_program_options(options_description&, options_description&) override;

        void plugin_initialize(const variables_map&);
        void plugin_startup();
        void plugin_shutdown();

      private:
   };

}
