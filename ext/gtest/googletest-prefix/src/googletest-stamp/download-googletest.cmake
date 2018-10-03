if(EXISTS "/home/bart/shell/cache/release-1.8.0.tar.gz")
  file("SHA1" "/home/bart/shell/cache/release-1.8.0.tar.gz" hash_value)
  if("x${hash_value}" STREQUAL "xe7e646a6204638fe8e87e165292b8dd9cd4c36ed")
    return()
  endif()
endif()
message(STATUS "downloading...
     src='https://github.com/google/googletest/archive/release-1.8.0.tar.gz'
     dst='/home/bart/shell/cache/release-1.8.0.tar.gz'
     timeout='none'")




file(DOWNLOAD
  "https://github.com/google/googletest/archive/release-1.8.0.tar.gz"
  "/home/bart/shell/cache/release-1.8.0.tar.gz"
  SHOW_PROGRESS
  # no TIMEOUT
  STATUS status
  LOG log)

list(GET status 0 status_code)
list(GET status 1 status_string)

if(NOT status_code EQUAL 0)
  message(FATAL_ERROR "error: downloading 'https://github.com/google/googletest/archive/release-1.8.0.tar.gz' failed
  status_code: ${status_code}
  status_string: ${status_string}
  log: ${log}
")
endif()

message(STATUS "downloading... done")
