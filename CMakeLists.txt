cmake_minimum_required(VERSION 3.2)

project(diploma)

set(proj_name diploma)
set(test_name tests)
set(perf_name performance)
set(src ${CMAKE_BINARY_DIR}/../src)
set(incl ${CMAKE_BINARY_DIR}/../include)
set(test ${CMAKE_BINARY_DIR}/../test)
set(perf ${CMAKE_BINARY_DIR}/../performance)

# packages
find_package(GTest)
find_package(benchmark)
find_package(Threads REQUIRED)

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
    Threads::Threads
)

set_target_properties(
    ${proj_name} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

include_directories(
    ${incl}
)

if(MSVC)
    target_compile_options(${proj_name} PRIVATE
        /W4
    )
else()
    target_compile_options(${proj_name} PRIVATE
        -Wall -Wextra -Werror -pedantic
    )
endif()

# tests
if(GTEST_FOUND)
    add_executable(
        ${test_name}
        ${test}/creation.cpp
        ${test}/execution.cpp
        ${test}/test_helpers.hpp
        ${test}/test_helpers.cpp
        ${incl}/IFunctionCaller.hpp
        ${incl}/Buffer.hpp
        ${incl}/Producer.hpp
        ${incl}/Consumer.hpp
        ${incl}/PBCController.hpp
    )
    include_directories(
        ${GTEST_INCLUDE_DIR}
    )
    
    set_target_properties(
        ${test_name} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
    )

    if(MSVC)
        target_link_libraries(
            ${test_name} PRIVATE
            ${GTEST_LIBRARIES}
            Threads::Threads
            GTest::gtest
            GTest::gtest_main
        )
        target_compile_options(${test_name} PRIVATE
            /W4
        )    
    else()
        target_link_libraries(
            ${test_name} PRIVATE
            ${GTEST_LIBRARIES}
            Threads::Threads
            gtest
            gtest_main
        )
        target_compile_options(${test_name} PRIVATE
            -Wall -Wextra -Werror -pedantic
        )
    endif()

    enable_testing()
    add_test(${test_name} ${test_name})
else()
    message("Tests are disabled. GTest was not found.")
endif()

# performance test
if(benchmark_FOUND)
    add_executable(
        ${perf_name}
        ${perf}/performance_testing.cpp
        ${test}/test_helpers.hpp
        ${test}/test_helpers.cpp
        ${incl}/IFunctionCaller.hpp
        ${incl}/Buffer.hpp
        ${incl}/Producer.hpp
        ${incl}/Consumer.hpp
        ${incl}/PBCController.hpp
    )

    target_link_libraries(
        ${perf_name} PRIVATE
        Threads::Threads
        benchmark::benchmark
        benchmark::benchmark_main
    )
    set_target_properties(
       ${perf_name}  PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
    )

    # compilator
    if(MSVC)
        target_compile_options(${perf_name} PRIVATE
            /W4
        )
    else()
        target_link_libraries(
            ${perf_name} PRIVATE
            stdc++fs
        )
        target_compile_options(${perf_name} PRIVATE
            -Wall -Wextra -Werror -pedantic
        )
    endif()
else()
    message("Performance tests are disabled. Benchmark was not found.")
endif()

