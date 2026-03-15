# Create helloworld executable with specified source files
add_executable(helloworld
    main.cpp
)

# If monitor requires vsync uncomment to set the vsync pin
#add_compile_definitions(VGA_GPIO_VSYNC=9)

# Add PicoVGA to the target
add_picovga(helloworld)

# for vga_config.h include
target_include_directories(helloworld PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

# Generate the uf2 output
pico_add_extra_outputs(helloworld)