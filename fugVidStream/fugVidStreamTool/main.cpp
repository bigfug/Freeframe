#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication::setApplicationName( "fugStream" );
	QApplication::setOrganizationName( "bigfug" );
	QApplication::setOrganizationDomain( "bigfug.com" );

	QApplication a(argc, argv);

	MainWindow w;

	w.show();
	
	return a.exec();
}
