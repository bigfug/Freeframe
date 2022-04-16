
find_program( XXD xxd )

find_package( OpenGL REQUIRED )

include_directories( ${OPENGL_INCLUDE_DIRS} )

link_libraries( ${OPENGL_LIBRARIES} )

add_subdirectory( fugFeedbackGL )
add_subdirectory( fugGlitchGL )
# add_subdirectory( fugCompilerGL ) # windows only?
add_subdirectory( fugWrapperGL )
add_subdirectory( fugScopeGL )
add_subdirectory( fugPowerGL )
