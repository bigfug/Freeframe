
#include "StreamThread.h"
#include <iostream>

boost::mutex			 StreamThread::mThreadMutex;
signed					 StreamThread::mLockCount = 0;
boost::thread			 StreamThread::mGlobalThread;
boost::asio::io_service	 StreamThread::mIO;

StreamThread::StreamThread( void )
{
	boost::lock_guard<boost::mutex>		ThreadLock( mThreadMutex );

	if( mGlobalThread.get_id() == boost::thread::id() )
	{
		try
		{
			mGlobalThread = boost::thread( boost::ref( *this ) );
		}
		catch( std::exception &e )
		{
			std::cout << e.what() << "\n";
		}
	}

	if( mGlobalThread.get_id() != boost::thread::id() )
	{
		mLockCount++;
	}
}

StreamThread::~StreamThread( void )
{
	boost::lock_guard<boost::mutex>		ThreadLock( mThreadMutex );

	assert( mLockCount > 0 );

	if( --mLockCount == 0 )
	{
		try
		{
			mIO.stop();

			mGlobalThread.interrupt();

			mGlobalThread.join();
		}
		catch( std::exception &e )
		{
			std::cout << e.what() << "\n";
		}

		assert( mGlobalThread.get_id() == boost::thread::id() );
	}
}

void StreamThread::operator()( void )
{
	boost::asio::io_service::work		work( mIO );

	while( !boost::this_thread::interruption_requested() )
	{
		try
		{
			mIO.run();
		}
		catch( ... )
		{
		}
	}
}
