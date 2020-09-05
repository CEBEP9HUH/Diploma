cmake_minimum_required(VERSION 3.2)

set(proj_name diploma)
set(test_name tests)
set(perf_name performance)
set(src ${CMAKE_BINARY_DIR}/../src)
set(incl ${CMAKE_BINARY_DIR}/../include)
set(test ${CMAKE_BINARY_DIR}/../test)
set(perf ${CMAKE_BINARY_DIR}/../performance)

# packages
find_package(GTest REQUIRED)
find_package(benchmark REQUIRED)

# project
add_executable(
    ${proj_name}
    ${src}/main.cpp
    ${incl}/IFunctionCaller.hpp
    ${incl}/Buffer.hpp
    ${incl}/Producer.hpp
    ${incl}/Consumer.hpp
    ${incl}/PBCController.hpp
)

target_link_libraries(
    ${proj_name} PRIVATE
    pthread
)

# tests
add_executable(
    ${test_name}
    ${incl}/IFunctionCaller.hpp
    ${test}/creation.cpp
    ${test}/execution.cpp
    ${incl}/Buffer.hpp
    ${incl}/Producer.hpp
    ${incl}/Consumer.hpp
    ${incl}/PBCController.hpp
)

target_link_libraries(
    ${test_name} PRIVATE
    ${GTEST_LIBRARIES}
    pthread
    gtest
    gtest_main
)

# performance test
add_executable(
    ${perf_name}
    ${perf}/performance_testing.cpp
    ${incl}/IFunctionCaller.hpp
    ${incl}/Buffer.hpp
    ${incl}/Producer.hpp
    ${incl}/Consumer.hpp
    ${incl}/PBCController.hpp
)

target_link_libraries(
    ${perf_name} PRIVATE
    pthread
    benchmark
    benchmark_main
)

# common
set_target_properties(
    ${proj_name} ${test_name} ${perf_name} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

include_directories(
    ${incl}
    ${GTEST_INCLUDE_DIR}
)

# compilator
if(MSVC)
    target_compile_options(${proj_name} PRIVATE
        /W4
    )
    target_compile_options(${test_name} PRIVATE
        /W4
    )
    target_compile_options(${perf_name} PRIVATE
        /W4
    )
else()
    target_compile_options(${proj_name} PRIVATE
        -Wall -Wextra -Werror -pedantic
    )
    target_compile_options(${test_name} PRIVATE
        -Wall -Wextra -Werror -pedantic
    )
    target_compile_options(${perf_name} PRIVATE
        -Wall -Wextra -Werror -pedantic
    )
endif()


# testing

enable_testing()
add_test(${test_name} ${test_name})
