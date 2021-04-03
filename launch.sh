#!/bin/bash


usage()
{
    echo "usage: launch.sh [-b[=cmake_toolchain]] [-e[=path_to_qemu]] [-d] [-h] [-c]"
    echo "\t\t-b, --build=PATH_TO_TOOLCHAIN buid with optional toolchain cmake-file"
    echo "\t\t\t-d enable debug-build (release by-default)"
    echo "\t\t-e, --execute=PATH_TO_QEMU    execute on qemu with optional path to binary"
    echo "\t\t-c, --clean                   clean build directory"
    echo "\t\t-h, --help                    show this help"
    exit 1
}

TOOLCHAIN=''
QEMU_PATH=''

CLEAN='false'
BUILD='true'
EXECUTE='true'
DEBUG='false'
FREE_RTOS='false'


for i in "$@"
do
case $i in
    -c|--clean)
        CLEAN='true'
    ;;
    -b|--build)
        BUILD='true'
    ;;
    -e|--execute)
        EXECUTE='true'
    ;;
    -b=*|--build=*)
        BUILD='true'
        TOOLCHAIN="${i#*=}"
        shift # past argument=value
    ;;
    -e=*|--execute=*)
        EXECUTE='true'
        QEMU_PATH="${i#*=}"
        shift # past argument=value
    ;;
    -d)
        DEBUG='true'
    ;;

    *)
        usage
        # unknown option
    ;;
esac
done

echo "length size = ${#TOOLCHAIN}"

if ${CLEAN}; then
    rm -rf build
fi

if ${BUILD}; then
    echo "Build"
    mkdir -p build && cd build
    options=" -DCMAKE_CROSSCOMPILE=true "
    if [ ${#TOOLCHAIN} -ne 0 ]; then
        echo "Set toolchain \"$TOOLCHAIN\""
        options+=" -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN "
    else
        options+=" -DCMAKE_C_COMPILER=riscv64-linux-gnu-gcc -DCMAKE_CXX_COMPILER=riscv64-linux-gnu-g++ "
    fi

    if ${DEBUG}; then
        options+=" -DCMAKE_BUILD_TYPE=Debug "
    else
        options+=" -DCMAKE_BUILD_TYPE=Release "
    fi
    if ${FREE_RTOS}; then
        echo "FREE_RTOS unsuported"
        exit 1
    else
        options+=" -DNO_FREERTOS=true "
    fi
    echo "options \"$options\""

    cmake ../ $options
    cmake --build .
    cd ..
fi

if ${EXECUTE}; then
    echo "Execute"
    cd build

    QEMU_EXEC_LIB=/usr/riscv64-linux-gnu/
    if [ ${#QEMU_PATH} -ne  0 ]; then
        QEMU_BIN=$QEMU_PATH/qemu-riscv64-static
    else
        QEMU_BIN=qemu-riscv64-static
    fi

    $QEMU_BIN -L $QEMU_EXEC_LIB mem/double_linked_list_test
    $QEMU_BIN -L $QEMU_EXEC_LIB mem/memory_test
    
    $QEMU_BIN -L $QEMU_EXEC_LIB tiny_vm
    cd ..
fi




