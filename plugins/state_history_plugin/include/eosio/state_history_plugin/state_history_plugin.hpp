/**
 *  @file
 *  @copyright defined in dos/LICENSE.txt
 */
#pragma once
#include <appbase/application.hpp>

#include <dosio/chain_plugin/chain_plugin.hpp>

template <typename T>
struct history_serial_big_vector_wrapper {
   T obj;
};

namespace dp {
class variant;
}

namespace dosio {
using chain::bytes;
using std::shared_ptr;

typedef shared_ptr<struct state_history_plugin_impl> state_history_ptr;

struct table_delta {
   dp::unsigned_int                                                       struct_version = 0;
   std::string                                                            name{};
   history_serial_big_vector_wrapper<std::vector<std::pair<bool, bytes>>> rows{};
};

struct block_position {
   uint32_t             block_num = 0;
   chain::block_id_type block_id  = {};
};

struct get_status_request_v0 {};

struct get_status_result_v0 {
   block_position head                    = {};
   block_position last_irreversible       = {};
   uint32_t       trace_begin_block       = 0;
   uint32_t       trace_end_block         = 0;
   uint32_t       chain_state_begin_block = 0;
   uint32_t       chain_state_end_block   = 0;
};

struct get_blocks_request_v0 {
   uint32_t                    start_block_num        = 0;
   uint32_t                    end_block_num          = 0;
   uint32_t                    max_messages_in_flight = 0;
   std::vector<block_position> have_positions         = {};
   bool                        irreversible_only      = false;
   bool                        fetch_block            = false;
   bool                        fetch_traces           = false;
   bool                        fetch_deltas           = false;
};

struct get_blocks_ack_request_v0 {
   uint32_t num_messages = 0;
};

struct get_blocks_result_v0 {
   block_position               head;
   block_position               last_irreversible;
   dp::optional<block_position> this_block;
   dp::optional<block_position> prev_block;
   dp::optional<bytes>          block;
   dp::optional<bytes>          traces;
   dp::optional<bytes>          deltas;
};

using state_request = dp::static_variant<get_status_request_v0, get_blocks_request_v0, get_blocks_ack_request_v0>;
using state_result  = dp::static_variant<get_status_result_v0, get_blocks_result_v0>;

class state_history_plugin : public plugin<state_history_plugin> {
 public:
   APPBASE_PLUGIN_REQUIRES((chain_plugin))

   state_history_plugin();
   virtual ~state_history_plugin();

   virtual void set_program_options(options_description& cli, options_description& cfg) override;

   void plugin_initialize(const variables_map& options);
   void plugin_startup();
   void plugin_shutdown();

 private:
   state_history_ptr my;
};

} // namespace dosio

// clang-format off
DP_REFLECT(dosio::table_delta, (struct_version)(name)(rows));
DP_REFLECT(dosio::block_position, (block_num)(block_id));
DP_REFLECT_EMPTY(dosio::get_status_request_v0);
DP_REFLECT(dosio::get_status_result_v0, (head)(last_irreversible)(trace_begin_block)(trace_end_block)(chain_state_begin_block)(chain_state_end_block));
DP_REFLECT(dosio::get_blocks_request_v0, (start_block_num)(end_block_num)(max_messages_in_flight)(have_positions)(irreversible_only)(fetch_block)(fetch_traces)(fetch_deltas));
DP_REFLECT(dosio::get_blocks_ack_request_v0, (num_messages));
// clang-format on
