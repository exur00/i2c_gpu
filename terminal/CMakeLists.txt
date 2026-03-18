# Create terminal executable with specified source files
add_executable(terminal
    main.cpp
)

# enable usb output, disable uart output
pico_enable_stdio_usb(terminal 1)
pico_enable_stdio_uart(terminal 0)

# If monitor requires vsync uncomment to set the vsync pin
#add_compile_definitions(VGA_GPIO_VSYNC=9)

# Add PicoVGA to the target
add_picovga(terminal)

# Generate the uf2 output
pico_add_extra_outputs(terminal)