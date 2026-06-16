add_library(ebroschin_logging
    include/ebroschin/logging/logger.hpp
    include/ebroschin/logging/log.hpp
    include/ebroschin/logging/null_logger.hpp
    src/log.cpp
    include/ebroschin/logging/log_level.hpp
    include/ebroschin/logging/log_stream.hpp
)

add_library(ebroschin::logging ALIAS ebroschin_logging)
set_target_properties(ebroschin_logging PROPERTIES EXPORT_NAME logging)

target_include_directories(ebroschin_logging PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(ebroschin_logging PUBLIC cxx_std_23)

if(BUILD_TESTING)
    find_package(GTest CONFIG REQUIRED)

    add_executable(ebroschin_logging_tests
        tests/test_log_system.cpp
    )

    target_link_libraries(ebroschin_logging_tests PRIVATE
        ebroschin_logging
        GTest::gtest
        GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(ebroschin_logging_tests)
endif()