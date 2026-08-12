cmake_minimum_required(VERSION 3.20)

project(ArgueWithCpp VERSION 0.1.0 LANGUAGES CXX)
option(BUILD_TESTS "Build tests" OFF)

add_library(ArgueWithCpp
    src/main.cpp
    src/Tokenizer.cpp
)

target_include_directories(ArgueWithCpp
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(ArgueWithCpp
    PUBLIC
        cxx_std_20
)

if(BUILD_TESTS)
    enable_testing()

    add_executable(ArgueWithCppTests
        test/test.cpp
    )

    target_link_libraries(ArgueWithCppTests
        PRIVATE
            ArgueWithCpp
    )

    add_test(NAME ArgueWithCppTests COMMAND ArgueWithCppTests)
endif()
