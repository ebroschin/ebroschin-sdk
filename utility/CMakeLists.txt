add_library(ebroschin_utility
    include/ebroschin/utility/variadic.hpp
    include/ebroschin/utility/signal.hpp
    src/signal_subscription.cpp
    include/ebroschin/utility/signal_subscription.hpp
    include/ebroschin/utility/arguments.hpp
    src/arguments.cpp
)

add_library(ebroschin::utility ALIAS ebroschin_utility)

set_target_properties(ebroschin_utility PROPERTIES EXPORT_NAME utility)

target_include_directories(ebroschin_utility PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(ebroschin_utility PUBLIC cxx_std_23)

if(BUILD_TESTING)
    find_package(GTest CONFIG REQUIRED)

    add_executable(ebroschin_utility_tests
        tests/test_signal_subscription.cpp
    )

    target_link_libraries(ebroschin_utility_tests PRIVATE
        ebroschin_utility
        GTest::gtest
        GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(ebroschin_utility_tests)
endif()