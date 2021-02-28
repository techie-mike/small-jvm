cmake_minimum_required()

project (tiny_vm)

set(CMAKE_CXX_STANDARD 17)

if (NOT DEFINED NO_ASM)
    set(NO_ASM false)
endif ()

set(SOURCE_LIST
    main.c
    test.c
    vm.c
)

add_subdirectory (mem)


add_executable(tiny_vm ${SOURCE_LIST})

if(NO_FREERTOS)
    target_compile_options(tiny_vm PUBLIC "-DNO_FREERTOS")
endif()

if(CMAKE_CROSSCOMPILE AND NOT NO_ASM)
    target_compile_options(tiny_vm PUBLIC "-DASM")
    message("!! Build assembly-version")
endif()

