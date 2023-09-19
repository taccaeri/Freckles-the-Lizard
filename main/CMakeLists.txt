idf_component_register(SRCS "wifi.c" "humidity.c" "color.c" "main.c"
                    INCLUDE_DIRS "")

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
