#include "qastle.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	Qastle w;
	w.show();
	return a.exec();
}
