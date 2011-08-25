#ifndef __JSTORAGE_H__
#define __JSTORAGE_H__

#include "jview.h"

/*

Memory storage.
Features:
 - insert data;
 - remove data;
 - seekable.

File storage.
Features:
 - append data;
 - seekable.

*/

// ------------------------------------------------------------------------

class jStorageHandler;

class jStorageInterface
{
	COPY_FBD(jStorageInterface)
	DECL_MUTEX
public:
	jStorageInterface() {}
	virtual ~jStorageInterface() {}

	virtual jStorageInterface & setSegmentSize(quint64 _size = 4096) = 0;
	virtual quint64 segmentSize() const = 0;

	virtual quint64 storageSize() const = 0;

	virtual jStorageInterface & setProcessedItemsHint(quint64 _count = 4096) = 0;
	virtual quint64 processedItemsHint() const = 0;

	virtual void startProcessing(int _priority = QThread::LowestPriority) = 0;
	virtual void stopProcessing() = 0;
	virtual bool isProcessingFinished() const = 0;

	virtual jStorageHandler * storageControl() const = 0;
};

// ------------------------------------------------------------------------

template <class T>
class jStorage : public jStorageInterface
{
	COPY_FBD(jStorage)
public:

	typedef QVector<T> (*segment_func)(const QVector<T> &);
	static QVector<T> defaultSegmentProcessing(const QVector<T> &);

	jStorage();
	~jStorage();

	jStorageInterface & setSegmentSize(quint64 _size = 4096);
	quint64 segmentSize() const;

	jStorage & setSegmentFunc(segment_func _segment_func = & defaultSegmentProcessing);
	segment_func segmentFunc() const;

	enum ProcessingType {MinOnly = 0, MaxOnly = 1, MaxThenMin = 2, MinThenMax = 3, MinMaxInterleaved = 4};
	QVector<T> processedItems(quint64 _start_item = 0, quint64 _end_item = 0, int _type = MinMaxInterleaved) const;

	jStorageInterface & setProcessedItemsHint(quint64 _count = 4096);
	quint64 processedItemsHint() const;

	void startProcessing(int _priority = QThread::LowestPriority);
	void stopProcessing();
	bool isProcessingFinished() const;

	jStorageHandler * storageControl() const;
protected:
	jStorage & setPosition(quint64 _item_position);
	quint64 position() const;
	virtual quint64 readItems(T * & _items, quint64 _items_count) = 0;
private:
	quint64 seek_pos, seg_size, hint;
	segment_func seg_func;
	class jStorageThread;
	jStorageThread * thread;
	jStorageHandler * handler;
};

// ------------------------------------------------------------------------

template <class T>
class jMemoryStorage : public jStorage<T>
{
	COPY_FBD(jMemoryStorage)
public:
	jMemoryStorage();
	~jMemoryStorage();

	jMemoryStorage & setStorageBuffer(T * _items, quint64 _items_count);
	quint64 storageSize() const;
protected:
	quint64 readItems(T * & _items, quint64 _items_count);
private:
	T * items;
	quint64 items_count;
};

// ------------------------------------------------------------------------

class jStorageHandler : public QObject
{
	Q_OBJECT
public:
	jStorageHandler(jStorageInterface * _storage): QObject(), storage(_storage) {}
	~jStorageHandler() {}
	void emitFinished(quint64 _msecs) { emit finished(_msecs); }
	void emitLayersAdjusted()	  { emit layersAdjusted(); }
signals:
	void finished(quint64);
	void layersAdjusted();
public slots:
	void startProcessing(int _priority = QThread::LowestPriority)	{ storage->startProcessing(_priority); }
	void stopProcessing()						{ storage->stopProcessing(); }
private:
	jStorageInterface * storage;
};

// ------------------------------------------------------------------------

#include "jstorage_p.h"

#endif
