add_library(ebroschin_commands INTERFACE
    include/ebroschin/commands/commands_system.hpp
)

add_library(ebroschin::commands ALIAS ebroschin_commands)
set_target_properties(ebroschin_commands PROPERTIES EXPORT_NAME core)

target_include_directories(ebroschin_commands INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(BUILD_TESTING)
    find_package(GTest CONFIG REQUIRED)

    add_executable(ebroschin_commands_tests
        tests/test_command_system.cpp
    )

    target_link_libraries(ebroschin_commands_tests PRIVATE
        ebroschin_commands
        ebroschin::core
        ebroschin::utility
        GTest::gtest
        GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(ebroschin_commands_tests)
endif()

target_compile_features(ebroschin_commands INTERFACE cxx_std_23)