file(GLOB HEADERS "include/dosio/faucet_testnet_plugin/*.hpp")
add_library( faucet_testnet_plugin
             faucet_testnet_plugin.cpp
             ${HEADERS} )

target_link_libraries( faucet_testnet_plugin appbase dp http_plugin chain_plugin )
target_include_directories( faucet_testnet_plugin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )
