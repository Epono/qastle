#include "../include/qastle.h"
#include <QApplication>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	QApplication::setApplicationName("qastledb");
	QApplication::setApplicationVersion("0.1");
	QApplication::setApplicationDisplayName("QastleDB");
	QApplication::setWindowIcon(QIcon("Resources/icon.png"));

	Qastle w;
	w.show();
	return a.exec();
}
