#include "perf_test.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	perf_test w;
	w.show();
	return a.exec();
}
