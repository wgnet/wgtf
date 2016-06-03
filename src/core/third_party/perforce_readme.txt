Clone the perforce repository into ngt/src/core/third_party/perforce.
Make sure the folder structure matches PERFORCE_DIR in the build script
ngt/build/cmake/FindPerforce.cmake.
The default path is ngt/src/core/third_party/perforce.r15.2/bin.ntx64/vs2015_dyn
or ngt/src/core/third_party/perforce.r15.2/bin.ntx64/vs2015_dyn_debug.
If Perforce is not present, Perforce projects will be disabled.
