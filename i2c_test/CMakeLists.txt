add_executable(i2c_test
    i2c_test.c
    )
target_link_libraries(i2c_test
    pico_i2c_slave
    hardware_i2c
    pico_stdlib
    )

# enable usb output, disable uart output
pico_enable_stdio_usb(i2c_test 1)
pico_enable_stdio_uart(i2c_test 0)

pico_add_extra_outputs(i2c_test)
#example_auto_set_url(i2c_test)


