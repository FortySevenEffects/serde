include(CMakeToolsHelpers OPTIONAL)

project(unit-tests)

include_directories(
    ${unit-tests_SOURCE_DIR}
    ${gtest_SOURCE_DIR}/include
    ${gmock_SOURCE_DIR}/include
)

add_executable(unit-tests

    unit-tests.cpp
    unit-tests.h
    unit-tests_Namespace.h

    tests/unit-tests_SerialMock.cpp
    tests/unit-tests_SerdeTx.cpp
    tests/unit-tests_SerdeRx.cpp
    tests/unit-tests_SerdeLoopback.cpp
    tests/unit-tests_SerdePacket.cpp
)

target_link_libraries(unit-tests
    gtest
    gmock
    serde
    test-mocks
)

add_test(unit-tests ${unit-tests_BINARY_DIR}/unit-tests --gtest_color=yes)
add_custom_target(build-and-run-unit-tests
    COMMAND ${CMAKE_CTEST_COMMAND} -V
    DEPENDS unit-tests
)
