
file(GLOB_RECURSE WIFI_SOURCES components/wifi/*.cpp)

idf_component_register(SRCS 

# "adc_handler.c"
# "StationMode.c"
# "AccessPointMode.c"
# "TcpServer.c"
# "wifi_info_handler.c"
"main.cpp"
${WIFI_SOURCES}
# "params.c"
# "constants.c"
# "str_lib.c"
# "GPIO.c"
                       INCLUDE_DIRS ".")
