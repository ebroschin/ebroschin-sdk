add_library(ebroschin_scheduling
    src/scheduling_system.cpp
    include/ebroschin/scheduling/commons.hpp
)

target_link_libraries(ebroschin_scheduling PRIVATE
    ebroschin::core
)

add_library(ebroschin::scheduling ALIAS ebroschin_scheduling)
set_target_properties(ebroschin_scheduling PROPERTIES EXPORT_NAME scheduling)
target_include_directories(ebroschin_scheduling PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(BUILD_TESTING)
    find_package(GTest CONFIG REQUIRED)

    add_executable(ebroschin_scheduling_tests
        tests/test_scheduling_system.cpp
    )

    target_link_libraries(ebroschin_scheduling_tests PRIVATE
        ebroschin_scheduling
        ebroschin::core
        GTest::gtest
        GTest::gtest_main
    )

    enable_testing()

    add_test(
        NAME SchedulingTestsStable
        COMMAND ebroschin_scheduling_tests
            --gtest_filter=-SchedulingSystemTest.PeriodicScheduling:SchedulingSystemTest.OneShotAndPeriodicScheduling
    )

    add_test(
        NAME SchedulingTestsFlaky
        COMMAND ebroschin_scheduling_tests
            --gtest_filter=SchedulingSystemTest.PeriodicScheduling:SchedulingSystemTest.OneShotAndPeriodicScheduling
    )

    set_tests_properties(SchedulingTestsFlaky PROPERTIES LABELS flaky)
endif()

target_compile_features(ebroschin_scheduling PUBLIC cxx_std_23)