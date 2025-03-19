# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\tempusUI_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\tempusUI_autogen.dir\\ParseCache.txt"
  "tempusUI_autogen"
  )
endif()
