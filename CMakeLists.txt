if(ESP_PLATFORM)
    idf_component_register(SRCS "src/RS485.c"
                        INCLUDE_DIRS "include"
                        REQUIRES driver)
    return()

endif()