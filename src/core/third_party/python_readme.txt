To use Python, the Python source must be added under Python/<version>.
Use an unmodified version of the Python sources,
not the one from Wargaming.
If Python is not present, Python projects will be disabled.

Example to use Python 2.7.12:
1. Download Python-2.7.12.
2. Add it to ngt/src/core/third_party/python/python-2.7.12.
3. Double-check your folder structure matches the PYTHON_DIR in the build script
   ngt/build/cmake/FindCorePython.cmake.

Compatibility Notes
- To build with VS2015, Python sources must be patched or taken from the latest.
- Python plugins are not supported for use with MacOS or Maya.

