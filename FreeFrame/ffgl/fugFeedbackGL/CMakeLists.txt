
project( fugFeedbackGL )

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
	)

file( TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR} _SHADER_BASE_PATH )

foreach( FFNAME fugFeedback )
	foreach( FFVER 16 20 )

		set( FFLIB ${FFNAME}_FF${FFVER}_x${FF_BITS} )

		set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "")
		
		add_library( ${FFLIB} MODULE
			${SOURCES}
			${FFNAME}_FF${FFVER}.vert
			${FFNAME}_FF${FFVER}.frag
			${CMAKE_SOURCE_DIR}/glad/ff${FFVER}/src/glad.c
		)

		set_target_properties( ${FFLIB} PROPERTIES
		  BUNDLE TRUE
		)

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
			"FF_SHADER_VERT=${FFNAME}_FF${FFVER}_vert"
			"FF_SHADER_FRAG=${FFNAME}_FF${FFVER}_frag"
			"FF_SHADER_VERT_H=${FFNAME}_FF${FFVER}_vert.h"
			"FF_SHADER_FRAG_H=${FFNAME}_FF${FFVER}_frag.h"
			"FF_SHADER_VERT_LEN=${FFNAME}_FF${FFVER}_vert_len"
			"FF_SHADER_FRAG_LEN=${FFNAME}_FF${FFVER}_frag_len"
			SHADER_BASE_PATH="${_SHADER_BASE_PATH}"
			)

		#------------------------------------------------------------------------------

		add_custom_command(
			OUTPUT ${FFNAME}_FF${FFVER}_vert.h
			COMMAND ${XXD} -i ${FFNAME}_FF${FFVER}.vert ${CMAKE_CURRENT_BINARY_DIR}/${FFNAME}_FF${FFVER}_vert.h
			MAIN_DEPENDENCY ${FFNAME}_FF${FFVER}.vert
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			VERBATIM
			COMMENT "Compiling ${CMAKE_CURRENT_SOURCE_DIR}/${FFNAME}_FF${FFVER}.vert"
			)

		add_custom_command(
			OUTPUT ${FFNAME}_FF${FFVER}_frag.h
			COMMAND ${XXD} -i ${FFNAME}_FF${FFVER}.frag ${CMAKE_CURRENT_BINARY_DIR}/${FFNAME}_FF${FFVER}_frag.h
			MAIN_DEPENDENCY ${FFNAME}_FF${FFVER}.frag
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			VERBATIM
			COMMENT "Compiling ${CMAKE_CURRENT_SOURCE_DIR}/${FFNAME}_FF${FFVER}.frag"
			)

		target_sources( ${FFLIB} PRIVATE
			${CMAKE_CURRENT_BINARY_DIR}/${FFNAME}_FF${FFVER}_vert.h
			${CMAKE_CURRENT_BINARY_DIR}/${FFNAME}_FF${FFVER}_frag.h
			)

		#------------------------------------------------------------------------------

		target_include_directories( ${FFLIB} PRIVATE ${CMAKE_CURRENT_BINARY_DIR} )

	endforeach()
endforeach()
