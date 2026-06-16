add_library(ebroschin_persistence INTERFACE
    include/ebroschin/persistence/persistence_system.hpp
    include/ebroschin/persistence/persistence_adapter.hpp
    include/ebroschin/persistence/persistence_store.hpp
)

add_library(ebroschin::persistence ALIAS ebroschin_persistence)
set_target_properties(ebroschin_persistence PROPERTIES EXPORT_NAME persistence)

target_include_directories(ebroschin_persistence INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(ebroschin_persistence INTERFACE
    ebroschin::utility
    ebroschin::core
)

if(BUILD_TESTING)
    find_package(GTest CONFIG REQUIRED)

    add_executable(ebroschin_persistence_tests
        tests/test_persistence_system.cpp
        tests/mock_persistence_store.hpp
        tests/commons.hpp
    )

    target_link_libraries(ebroschin_persistence_tests PRIVATE
        ebroschin_persistence
        ebroschin::utility
        ebroschin::core
        GTest::gtest
        GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(ebroschin_persistence_tests)
endif()

target_compile_features(ebroschin_persistence INTERFACE cxx_std_23)