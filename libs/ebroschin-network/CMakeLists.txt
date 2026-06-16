add_library(ebroschin_network INTERFACE
    include/ebroschin/network/tcp/tcp_system.hpp
    include/ebroschin/network/tcp/tcp_connection.hpp
    include/ebroschin/network/commons.hpp
    include/ebroschin/network/tcp/tcp_system_concepts.hpp
    include/ebroschin/network/tcp/tcp_system_builder.hpp
    include/ebroschin/network/tcp/tcp_message_processor.hpp
    include/ebroschin/network/tcp/tcp_system_connection_facade.hpp
    include/ebroschin/network/rpc/commons.hpp
    include/ebroschin/network/rpc/rpc_call_builder.hpp
    include/ebroschin/network/rpc/rpc_system.hpp
    include/ebroschin/network/tcp/tcp_connection_event_handler.hpp
    include/ebroschin/network/tcp/tcp_connector.hpp
)

add_library(ebroschin::network ALIAS ebroschin_network)
set_target_properties(ebroschin_network PROPERTIES EXPORT_NAME network)

target_include_directories(ebroschin_network INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(ebroschin_network INTERFACE
    ebroschin::utility
    ebroschin::core
)

if(BUILD_TESTING)
    find_package(GTest CONFIG REQUIRED)

    add_executable(ebroschin_network_tests
        tests/test_tcp_system.cpp
        tests/test_rpc_system.cpp
        tests/mock_connection_event_handler.hpp
        tests/mock_tcp_connection.hpp
        tests/mock_tcp_connector.hpp
        tests/api.hpp
        tests/mock_network_codec.hpp
        tests/mock_tcp_system.hpp
        tests/mock_rpc_timeout_handler.hpp
        tests/mock_rpc_system.hpp
    )

    target_link_libraries(ebroschin_network_tests PRIVATE
        ebroschin_network
        ebroschin::utility
        ebroschin::core
        ebroschin::network-modules
        GTest::gtest
        GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(ebroschin_network_tests)
endif()

target_compile_features(ebroschin_network INTERFACE cxx_std_23)