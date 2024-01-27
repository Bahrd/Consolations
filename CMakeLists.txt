# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("Consolations")

# Include sub-projects.
add_subdirectory ("fft")
add_subdirectory ("VariadicAsyncT")
add_subdirectory ("MetaTemplates")
add_subdirectory ("SummAnonymous")
add_subdirectory("Barricade")
add_subdirectory("CClatchCC")
add_subdirectory("AtomicLoopInLoupe")
