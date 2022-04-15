#pragma once

#include <boost/thread.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/asio.hpp>

class StreamThread
{
protected:
	static boost::mutex			 mThreadMutex;
	static boost::thread		 mGlobalThread;
	static int					 mLockCount;

	static boost::asio::io_service		 mIO;

public:
	StreamThread( void );

	virtual ~StreamThread( void );

	void operator()( void );

	boost::asio::io_service &getIO( void )
	{
		return( mIO );
	}
};
