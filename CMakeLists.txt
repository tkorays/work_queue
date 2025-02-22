cmake_minimum_required(VERSION 3.14)
project(workqueue)

option(WORKQUEUE_ENABLE_TEST "enable unittest for workqueue" OFF)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB WORKQUEUE_SRC ${CMAKE_SOURCE_DIR}/*.h ${CMAKE_SOURCE_DIR}/*.cc)
add_library(workqueue STATIC ${WORKQUEUE_SRC})


if (WORKQUEUE_ENABLE_TEST)
    add_subdirectory(third_party/googltest)
    enable_testing()

    file(GLOB WORKQUEUE_TEST_SRC ${CMAKE_SOURCE_DIR}/tests/*.cc)

    add_executable(
        workqueue_test
        ${WORKQUEUE_TEST_SRC}
    )
    target_link_libraries(
        workqueue_test
        workqueue
        GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(workqueue_test)
endif()

