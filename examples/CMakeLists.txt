file(GLOB ExampleSources
    *.c
)

foreach (example_src ${ExampleSources})
    get_filename_component(example_name ${example_src} NAME_WE)
    add_executable(${example_name} ${example_src})
endforeach ()
