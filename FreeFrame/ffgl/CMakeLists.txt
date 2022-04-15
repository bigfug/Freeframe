
find_program( XXD xxd )

find_package( OpenGL REQUIRED )

include_directories( ${OPENGL_INCLUDE_DIRS} )

link_libraries( ${OPENGL_LIBRARIES} )

add_subdirectory( fugFeedbackGL )
