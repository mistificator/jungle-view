#ifndef _JDEFS_H_
#define _JDEFS_H_

#include <QtCore>
#include <QtGui>
#include <QtOpenGL/QtOpenGL>

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
	#define JDEBUG(str) qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << QFileInfo(__FILE__).fileName() << "\t" << __LINE__ << "\t" << __FUNCTION__ << "\t" << str;
#else
	#define JDEBUG(str)
#endif

class jMutex: public QReadWriteLock
{
public:
	jMutex(): QReadWriteLock(QReadWriteLock::Recursive) {}
};

#define DECL_MUTEX protected: mutable jMutex rw_lock;
#define THREAD_SAFE(_TYPE_)  /*JDEBUG("mutex lock");*/ rw_lock.lockFor##_TYPE_();
#define THREAD_UNSAFE rw_lock.unlock(); /*JDEBUG("mutex unlock");*/

template <class T> void jSafeProperty(T & _property, const T & _value, QReadWriteLock & _lock)
{
	_lock.lockForWrite();
	_property = _value;
	_lock.unlock();
}

template <class T> T jSafeProperty(const T & _property, QReadWriteLock & _lock)
{
	T _result;
	_lock.lockForRead();
	_result = _property;
	_lock.unlock();
	return _result;
}

#define SAFE_SET(_PROPERTY_, _VALUE_) ::jSafeProperty(_PROPERTY_, _VALUE_, rw_lock);
#define SAFE_GET(_PROPERTY_) ::jSafeProperty(_PROPERTY_, rw_lock)

#endif
