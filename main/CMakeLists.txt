
file(GLOB_RECURSE WIFI_SOURCES components/wifi/*.cpp)
file(GLOB_RECURSE TCP_SOURCES components/tcp/*.cpp)

set(CMAKE_CXX_STANDARD 20)

idf_component_register(SRCS 

"main.cpp"
${WIFI_SOURCES}
${TCP_SOURCES}

INCLUDE_DIRS ".")
