if [[ $(uname) == 'Darwin' ]]; then
    DYLIB_EXT=dylib
    CC=clang
    CXX=clang++
    CXXFLAGS="-I${PREFIX}/include -stdlib=libc++"
else
    DYLIB_EXT=so
    CXXFLAGS="-I${PREFIX}/include"
fi

BUILD_DIR=${BUILD_DIR-build}

CMAKE_GENERATOR=${CMAKE_GENERATOR-Unix Makefiles}
CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE-Release}

export MACOSX_DEPLOYMENT_TARGET=10.9

mkdir -p "${BUILD_DIR}" # Using -p here is convenient for calling this script outside of conda.
cd "${BUILD_DIR}"
cmake ..\
    -G "${CMAKE_GENERATOR}" \
    -DCMAKE_C_COMPILER=${CC} \
    -DCMAKE_CXX_COMPILER=${CXX} \
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} \
    -DCMAKE_INSTALL_PREFIX="${PREFIX}" \
    -DCMAKE_PREFIX_PATH="${PREFIX}" \
    -DCMAKE_CXX_FLAGS="${CXXFLAGS}" \
    -DCMAKE_OSX_SYSROOT="${CONDA_BUILD_SYSROOT}" \
    -DCMAKE_OSX_DEPLOYMENT_TARGET="${MACOSX_DEPLOYMENT_TARGET}" \
    -DCMAKE_SHARED_LINKER_FLAGS="-Wl,-rpath,${PREFIX}/lib -L${PREFIX}/lib" \
    -DCMAKE_EXE_LINKER_FLAGS="-Wl,-rpath,${PREFIX}/lib -L${PREFIX}/lib" \
    -DCMAKE_MACOSX_RPATH=ON

make -j${CPU_COUNT}
make test
make install
