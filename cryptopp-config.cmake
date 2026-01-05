if(BUILD_SHARED)
	include("${CMAKE_CURRENT_LIST_DIR}/cryptopp-targets-shared.cmake")
endif()

if(BUILD_STATIC)
	include("${CMAKE_CURRENT_LIST_DIR}/cryptopp-targets-static.cmake")
endif()
