# Create combined_test executable with specified source files
add_executable(combined_test
    main.cpp
)

target_link_libraries(combined_test
    pico_i2c_slave
    hardware_i2c
    pico_stdlib
    )

# enable usb output, disable uart output
pico_enable_stdio_usb(combined_test 1)
pico_enable_stdio_uart(combined_test 0)

# If monitor requires vsync uncomment to set the vsync pin
#add_compile_definitions(VGA_GPIO_VSYNC=9)

# Add PicoVGA to the target
add_picovga(combined_test)

# for vga_config.h include
target_include_directories(combined_test PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

# Generate the uf2 output
pico_add_extra_outputs(combined_test)