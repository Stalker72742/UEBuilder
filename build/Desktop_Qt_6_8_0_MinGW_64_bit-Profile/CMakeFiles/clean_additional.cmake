# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "RelWithDebInfo")
  file(REMOVE_RECURSE
  "CMakeFiles\\UEBuilder_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\UEBuilder_autogen.dir\\ParseCache.txt"
  "UEBuilder_autogen"
  )
endif()
