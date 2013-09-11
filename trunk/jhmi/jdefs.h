#ifndef _JDEFS_H_
#define _JDEFS_H_

#include <QtCore>
#include <QtGui>
#include <QtOpenGL/QtOpenGL>
#include "./../thread_utils/recursive_locker.h"
#include "./../thread_utils/memory_routines.h"

#ifdef JUNGLE_USES_OPENGL
	#define JUNGLE_WIDGET_CLASS QGLWidget
#else
	#define JUNGLE_WIDGET_CLASS QWidget
#endif

#define CTOR_FBD(CLASS_NAME) private: CLASS_NAME(const CLASS_NAME &) {}
#define ASSIGN_FBD(CLASS_NAME) private: CLASS_NAME & operator = (const CLASS_NAME &) { return * this; }
#define COPY_FBD(CLASS_NAME) CTOR_FBD(CLASS_NAME); ASSIGN_FBD(CLASS_NAME);
#define PDATA private: struct Data; Data * d;

#ifdef _DEBUG
	class jDbgTime
	{
	public:
		jDbgTime(int _line, const QString & _file, const QString & _func): line(_line), file(_file), func(_func)
		{
			dt = QDateTime::currentDateTime();
		}
		~jDbgTime()
		{
			qDebug() << QFileInfo(file).fileName() << "\t" << func << "\ttime elapsed from \tline" << line << "is\t" << dt.msecsTo(QDateTime::currentDateTime()) << "ms";	
		}
	private:
		int line;
		QString file, func;
		QDateTime dt;
	};

	#define JDEBUG(str) qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << QFileInfo(__FILE__).fileName() << "\t" << __LINE__ << "\t" << __FUNCTION__ << "\t" << str;
	#define JTIME_START		int _line_start = __LINE__; int _line_prev = _line_start; QTime _time_start = QTime::currentTime();  QTime _time_prev = _time_start;
	#define JTIME_DIFF(comment)	qDebug() << QFileInfo(__FILE__).fileName() << "\t" << __FUNCTION__ << "\ttime diff from \tline" << _line_prev << "to" << __LINE__ << "is\t" << _time_prev.msecsTo(QTime::currentTime()) << "ms, " << comment; _time_prev = QTime::currentTime(); _line_prev = __LINE__;
	#define JTIME_ELAPSED(comment)  qDebug() << QFileInfo(__FILE__).fileName() << "\t" << __FUNCTION__ << "\ttime elapsed from \tline" << _line_start << "to" << __LINE__ << "is\t" << _time_start.msecsTo(QTime::currentTime()) << "ms, " << comment;
	#define JTIME_AUTO jDbgTime __dbg_time_##__FUNCTION__(__LINE__, __FILE__, __FUNCTION__);
#else
	#define JDEBUG(str)
	#define JTIME_START
	#define JTIME_DIFF(comment)
	#define JTIME_ELAPSED(comment)
	#define JTIME_AUTO
#endif

#endif
