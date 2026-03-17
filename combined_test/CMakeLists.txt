# Create combined_test executable with specified source files
add_executable(combined_test
    main.cpp
)

# enable usb output, disable uart output
pico_enable_stdio_usb(combined_test 1)
pico_enable_stdio_uart(combined_test 0)

# If monitor requires vsync uncomment to set the vsync pin
#add_compile_definitions(VGA_GPIO_VSYNC=9)

# Add PicoVGA to the target
add_picovga(combined_test)

# Generate the uf2 output
pico_add_extra_outputs(combined_test)