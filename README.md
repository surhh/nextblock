# NextBlock - next sibling of classic game


# How to build 



- Just build in Qt Creator. 
- It should be also possible to build using CMake from the command line. Run the following cmake commands:

        cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS_RELEASE="-O3 -DNDEBUG" -B build -S .
  
        cmake --build build

  The nextblock execuatble will be located in the build directory.

----------------------------------------------------------------
  *Note: Compiled binary packages for both Linux and Windows will be available later.*
