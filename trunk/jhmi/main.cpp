#include "mainwindow.h"

int main(int argc, char *argv[])
{
#ifdef JUNGLE_USES_OPENGL
	QGL::setPreferredPaintEngine(QPaintEngine::OpenGL2);
#endif
	QApplication a(argc, argv);
	Mainwindow w;
	w.show();
	return a.exec();
}
