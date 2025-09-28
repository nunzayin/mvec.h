file(GLOB TestSources
    *.c
)

foreach (test_src ${TestSources})
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${test_src})
    add_test(
        NAME ${test_name} COMMAND ${test_name}
    )
endforeach ()
