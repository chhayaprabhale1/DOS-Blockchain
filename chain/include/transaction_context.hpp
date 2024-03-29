#pragma once
#include <dosio/chain/controller.hpp>
#include <dosio/chain/trace.hpp>
#include <signal.h>

namespace dosio { namespace chain {

   struct deadline_timer {
         deadline_timer();
         ~deadline_timer();

         void start(dp::time_point tp);
         void stop();

         static volatile sig_atomic_t expired;
      private:
         static void timer_expired(int);
         static bool initialized;
   };

   class transaction_context {
      private:
         void init( uint64_t initial_net_usage);

      public:

         transaction_context( controller& c,
                              const signed_transaction& t,
                              const transaction_id_type& trx_id,
                              dp::time_point start = dp::time_point::now() );

         void init_for_implicit_trx( uint64_t initial_net_usage = 0 );

         void init_for_input_trx( uint64_t packed_trx_unprunable_size,
                                  uint64_t packed_trx_prunable_size,
                                  uint32_t num_signatures,
                                  bool skip_recording);

         void init_for_deferred_trx( dp::time_point published );

         void exec();
         void finalize();
         void squash();
         void undo();

         inline void add_net_usage( uint64_t u ) { net_usage += u; check_net_usage(); }

         void check_net_usage()const;

         void checktime()const;

         void pause_billing_timer();
         void resume_billing_timer();

         uint32_t update_billed_cpu_time( dp::time_point now );

         std::tuple<int64_t, int64_t, bool, bool> max_bandwidth_billed_accounts_can_pay( bool force_elastic_limits = false )const;

         void validate_referenced_accounts( const transaction& trx, bool enforce_actor_whitelist_blacklist )const;

      private:

         friend struct controller_impl;
         friend class apply_context;

         void add_ram_usage( account_name account, int64_t ram_delta );

         void dispatch_action( action_trace& trace, const action& a, account_name receiver, bool context_free = false, uint32_t recurse_depth = 0 );
         inline void dispatch_action( action_trace& trace, const action& a, bool context_free = false ) {
            dispatch_action(trace, a, a.account, context_free);
         };
         void schedule_transaction();
         void record_transaction( const transaction_id_type& id, dp::time_point_sec expire );

         void validate_cpu_usage_to_bill( int64_t u, bool check_minimum = true )const;

      /// Fields:
      public:

         controller&                   control;
         const signed_transaction&     trx;
         transaction_id_type           id;
         optional<chainbase::database::session>  undo_session;
         transaction_trace_ptr         trace;
         dp::time_point                start;

         dp::time_point                published;


         vector<action_receipt>        executed;
         flat_set<account_name>        bill_to_accounts;
         flat_set<account_name>        validate_ram_usage;

         /// the maximum number of virtual CPU instructions of the transaction that can be safely billed to the billable accounts
         uint64_t                      initial_max_billable_cpu = 0;

         dp::microseconds              delay;
         bool                          is_input           = false;
         bool                          apply_context_free = true;
         bool                          enforce_whiteblacklist = true;

         dp::time_point                deadline = dp::time_point::maximum();
         dp::microseconds              leeway = dp::microseconds(3000);
         int64_t                       billed_cpu_time_us = 0;
         bool                          explicit_billed_cpu_time = false;

      private:
         bool                          is_initialized = false;


         uint64_t                      net_limit = 0;
         bool                          net_limit_due_to_block = true;
         bool                          net_limit_due_to_greylist = false;
         uint64_t                      eager_net_limit = 0;
         uint64_t&                     net_usage; /// reference to trace->net_usage

         bool                          cpu_limit_due_to_greylist = false;

         dp::microseconds              initial_objective_duration_limit;
         dp::microseconds              objective_duration_limit;
         dp::time_point                _deadline = dp::time_point::maximum();
         int64_t                       deadline_exception_code = block_cpu_usage_exceeded::code_value;
         int64_t                       billing_timer_exception_code = block_cpu_usage_exceeded::code_value;
         dp::time_point                pseudo_start;
         dp::microseconds              billed_time;
         dp::microseconds              billing_timer_duration_limit;

         deadline_timer                _deadline_timer;
   };

} }
