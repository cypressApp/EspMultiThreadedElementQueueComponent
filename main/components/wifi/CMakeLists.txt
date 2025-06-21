file(GLOB_RECURSE SOURCES "*.cpp")

set(CMAKE_CXX_STANDARD 20)

idf_component_register(
    SRCS ${SOURCES}
    INCLUDE_DIRS "."
)