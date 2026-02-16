# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\View_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\View_autogen.dir\\ParseCache.txt"
  "View_autogen"
  )
endif()
