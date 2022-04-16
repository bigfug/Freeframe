
project( fugCompilerGL )

set( SOURCES
	Plugin.cpp
	Plugin.h
	Instance.cpp
	Instance.h

	../../shared/PluginBase.h
	../../shared/InstanceBase.h
	../../shared/Shader.h
	../../shared/PluginBase.cpp
	../../shared/Library.cpp
	../../shared/InstanceBase.cpp
	../../shared/Shader.cpp
	../../shared/TexToBuf.cpp
	../../shared/TexToBuf.h
	)

file( TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR} _SHADER_BASE_PATH )

## set CMAKE_MODULE_PATH so we can find FindJPEGTURBO.cmake in which
## is located in the project directory
set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR} )

foreach( FFNAME fugCompiler )
	foreach( FFVER 16 20 )
		set( FFLIB ${FFNAME}_FF${FFVER}_x${FF_BITS} )

		set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "")
		
		add_library( ${FFLIB} MODULE
			${SOURCES}
			${CMAKE_SOURCE_DIR}/glad/ff${FFVER}/src/glad.c
		)

		set_target_properties( ${FFLIB} PROPERTIES
		  BUNDLE TRUE
		)

		find_package(GLEW REQUIRED)
		if (GLEW_FOUND)
			message(STATUS "FOUND GLEW Library")
			target_include_directories(${FFLIB} PRIVATE ${GLEW_INCLUDE_DIR})
			target_link_libraries(${FFLIB} debug ${GLEW_STATIC_LIBRARY_DEBUG} optimized ${GLEW_STATIC_LIBRARY_RELEASE})
		else()
			message(WARNING "GLEW library not found; you will need to set the paths manually") 
		endif()

		find_package(Boost REQUIRED)
		if (BOST_FOUND)
			message(STATUS "FOUND BOOST Library")
		else()
			message(WARNING "BOOST library not found; you will need to set the paths manually") 
		endif()
		
		target_include_directories( ${FFLIB} PRIVATE
			${CMAKE_SOURCE_DIR}/glad/ff${FFVER}/include
			${CMAKE_SOURCE_DIR}/Freeframe/include/ff${FFVER}
			${PROJECT_SOURCE_DIR}	## include source directory, not binary product directory
		)

		install( TARGETS ${FFLIB}
			LIBRARY DESTINATION ${PATH_APP}
			BUNDLE DESTINATION ${PATH_APP}
		)

		#------------------------------------------------------------------------------

		target_compile_definitions( ${FFLIB} PRIVATE
			"FF_VERSION_${FFVER}"
			"FF_VERSION=${FFVER}"
		)


	endforeach()
endforeach()
