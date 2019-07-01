project(test-mocks)

add_library(test-mocks INTERFACE)
#     test-mocks.cpp
#     test-mocks.h
#     test-mocks_Namespace.h
#     test-mocks_Defs.h
#     test-mocks_SerialMock.cpp
#     test-mocks_SerialMock.hpp
#     test-mocks_SerialMock.h
# )

target_include_directories(test-mocks INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
