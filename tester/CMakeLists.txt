
project( freeframe-tester )

enable_testing( true )

set( CMAKE_AUTOMOC ON )

find_package( Qt5 REQUIRED COMPONENTS Core OpenGL Test )

link_libraries(
	Qt5::Core Qt5::OpenGL Qt5::Test
)

foreach( FFNAME fugFeedback )
	foreach( FFTYPE params render )
		foreach( FFVER 16 20 )
			add_executable( test_${FFTYPE}_FF${FFVER}_${FFNAME}
				freeframe_${FFTYPE}_tester.cpp
				freeframe_${FFTYPE}_tester.h
			)

			target_include_directories( test_${FFTYPE}_FF${FFVER}_${FFNAME} PRIVATE
				${CMAKE_SOURCE_DIR}/Freeframe/include/ff${FFVER}
				)

			target_compile_definitions( test_${FFTYPE}_FF${FFVER}_${FFNAME} PRIVATE "FF_VERSION_${FFVER}" )

			if( WIN32 )
				target_compile_definitions( test_${FFTYPE}_FF${FFVER}_${FFNAME} PRIVATE
					"PLUGIN_FILENAME=${FFNAME}_FF${FFVER}_x${FF_BITS}.dll"
					"PLUGIN_PATH=../freeframe/ffgl/${FFNAME}GL_FF${FFVER}_x${FF_BITS}"
					)
			elseif( APPLE )
				target_compile_definitions( test_${FFTYPE}_FF${FFVER}_fugFeedback PRIVATE
					"PLUGIN_FILENAME=${FFNAME}_FF${FFVER}_x${FF_BITS}"
					"PLUGIN_PATH=../freeframe/ffgl/${FFNAME}GL/${FFNAME}_FF${FFVER}_x${FF_BITS}.bundle/Contents/MacOS"
					)
			elseif( UNIX )
				target_compile_definitions( test_${FFTYPE}_FF${FFVER}_${FFNAME} PRIVATE
					"PLUGIN_FILENAME=${FFNAME}_FF${FFVER}_x${FF_BITS}.so"
					"PLUGIN_PATH=../freeframe/ffgl/${FFNAME}GL"
					)
			endif()

			add_test( test_${FFTYPE}_FF${FFVER}_${FFNAME} test_${FFTYPE}_FF${FFVER}_${FFNAME} )
		endforeach()
	endforeach()
endforeach()
