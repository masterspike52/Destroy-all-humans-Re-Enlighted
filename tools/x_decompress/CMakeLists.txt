project("x_decompress")
set(CMAKE_CXX_STANDARD 17)

add_executable(x_decompress
    "x_decompress.cpp"
    "${POTF_RECOMP_TOOLS_ROOT}/XenonRecomp/thirdparty/libmspack/libmspack/mspack/lzxd.c"
)

target_include_directories(x_decompress
    PRIVATE "${POTF_RECOMP_TOOLS_ROOT}/XenonRecomp/thirdparty/libmspack/libmspack/mspack"
)
