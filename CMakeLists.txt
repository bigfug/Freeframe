cmake_minimum_required( VERSION 3.1 )

project( freeframe )

set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS ON )

set( CMAKE_INCLUDE_CURRENT_DIR OFF )

set( PKG_CONFIG_USE_CMAKE_PREFIX_PATH ON )

#----------------------------------------------------------------------------

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    message(STATUS "*** Have SIZEOF void * = 8, so 64-bit")
    set( IS_64_BIT 1 )
else ()
    message(STATUS "*** SIZEOF void * != 8, so not 64-bit")
endif ()

if( IS_64_BIT )
  set( FF_BITS 64 )
else()
  set( FF_BITS 32 )
endif()

#------------------------------------------------------------------------------

set( PATH_APP . )

if( WIN32 )
	set( PATH_APP bin )
	set( CMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD 1 )
endif( WIN32 )

if( UNIX AND NOT APPLE )
	set( PATH_APP		bin )
endif()

if( WIN32 )
	add_definitions( -D_CRT_SECURE_NO_WARNINGS -DTARGET_OS_WIN )
elseif( APPLE )
	add_definitions( -DTARGET_OS_MAC )
endif()

#------------------------------------------------------------------------------
# Enable Unit Testing

include( CTest )

set( CTEST_OUTPUT_ON_FAILURE ON )

#----------------------------------------------------------------------------

#find_package( Boost COMPONENTS
#	date_time)

#if( Boost_FOUND )
#	message( ${Boost_LIBRARIES} )

#	include_directories( ${Boost_INCLUDE_DIR} )

#	link_libraries( ${Boost_LIBRARIES})
#endif( Boost_FOUND )

#----------------------------------------------------------------------------

add_subdirectory( FreeFrame )
#add_subdirectory( tester )

