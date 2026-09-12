set(src_dirs
            LED
            MYIIC
            XL9555
            MYSPI
            SPILCD
            KEY)

set(include_dirs
            LED
            MYIIC
            XL9555
            MYSPI
            SPILCD
            KEY)

set(requires
            driver
            esp_lcd)

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires})

component_compile_options(-ffast-math -O3 -Wno-error=format=-Wno-format)