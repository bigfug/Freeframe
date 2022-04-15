
#include <QCoreApplication>
#include <QTest>

#include "teststream.h"

int main( int argc, char *argv[] )
{
	QCoreApplication app(argc, argv);

//	QTEST_DISABLE_KEYPAD_NAVIGATION;

	TestStream		Test1;

	return QTest::qExec( &Test1, argc, argv );
}
