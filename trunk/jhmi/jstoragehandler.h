#ifndef __JSTORAGEHANDLER_H__
#define __JSTORAGEHANDLER_H__

// ------------------------------------------------------------------------

#include <QObject>
#include <QThread>

class jStorageInterface;

class jStorageHandler : public QObject
{
	Q_OBJECT
public:
	jStorageHandler(jStorageInterface * _storage);
	~jStorageHandler();
signals:
	void finished(quint64);
	void layersAdjusted();
	void stopped();
public slots:
	void startProcessing(int _priority = QThread::LowestPriority);
	void stopProcessing();
private:
	jStorageInterface * storage;
};

// ------------------------------------------------------------------------

#endif