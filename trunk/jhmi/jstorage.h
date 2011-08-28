#ifndef __JSTORAGE_H__
#define __JSTORAGE_H__

#include "jview.h"
#include "jstoragehandler.h"

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

class jStorageInterface
{
	COPY_FBD(jStorageInterface)
	DECL_MUTEX
public:
	jStorageInterface() {}
	virtual ~jStorageInterface() {}

	enum ProcessedItemType {Minimums = 0, Maximums = 1, X = 2};
	virtual QMap<int, QByteArray> processedArray(quint64 _start_item = 0, quint64 _end_item = 0) const = 0;

	virtual jStorageInterface & setSegmentSize(quint64 _size = 1024) = 0;
	virtual quint64 segmentSize() const = 0;

	virtual int itemSize() const = 0; // in bytes
	virtual quint64 storageSize() const = 0;

	virtual jStorageInterface & setProcessedItemsHint(quint64 _count = 1024) = 0;
	virtual quint64 processedItemsHint() const = 0;

	virtual void startProcessing(int _priority = QThread::LowestPriority) = 0;
	virtual void stopProcessing() = 0;
	virtual bool isProcessingFinished() const = 0;
	virtual quint64 itemsProcessed() const = 0;

	virtual jStorageHandler * storageControl() const = 0;

	virtual QByteArray exportLayers() const = 0;
	virtual bool importLayers(const QByteArray & _saved_layers) = 0;
};

// ------------------------------------------------------------------------

template <class T>
class jStorage : public jStorageInterface
{
	COPY_FBD(jStorage)
public:

	typedef QVector<T> (*segment_func)(const T *, quint64, jStorage<T> *);
	static QVector<T> defaultSegmentProcessing(const T *, quint64, jStorage<T> *);
	typedef bool (*less_func)(const T &, const T &);
	static bool defaultLess(const T &, const T &);
	typedef bool (*greater_func)(const T &, const T &);
	static bool defaultGreater(const T &, const T &);

	jStorage();
	~jStorage();

	int itemSize() const; // in bytes

	jStorageInterface & setSegmentSize(quint64 _size = 1024);
	quint64 segmentSize() const;

	jStorage & setSegmentFunc(segment_func _segment_func = & defaultSegmentProcessing);
	segment_func segmentFunc() const;

	jStorage & setLessFunc(less_func _less_func = & defaultLess);
	less_func lessFunc() const;

	jStorage & setGreaterFunc(greater_func _greater_func = & defaultGreater);
	greater_func greaterFunc() const;

	QMap<int, QVector<T> > processedItems(quint64 _start_item = 0, quint64 _end_item = 0) const;
	QMap<int, QByteArray> processedArray(quint64 _start_item = 0, quint64 _end_item = 0) const;

	jStorageInterface & setProcessedItemsHint(quint64 _count = 1024);
	quint64 processedItemsHint() const;

	void startProcessing(int _priority = QThread::LowestPriority);
	void stopProcessing();
	bool isProcessingFinished() const;
	quint64 itemsProcessed() const;

	jStorageHandler * storageControl() const;

	QByteArray exportLayers() const;
	bool importLayers(const QByteArray & _saved_layers);
protected:
	jStorage & setPosition(quint64 _item_position);
	quint64 position() const;
	virtual quint64 readItems(T * & _items, quint64 _items_count) = 0;
private:
	quint64 seek_pos, seg_size, hint;
	segment_func seg_func;
	less_func l_func;
	greater_func g_func;
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
	jMemoryStorage(T * _items, quint64 _items_count, bool _deep_copy = false);
	~jMemoryStorage();

	virtual jMemoryStorage & setStorageBuffer(T * _items, quint64 _items_count, bool _deep_copy = false);
	virtual quint64 storageSize() const;
	virtual bool isDeepCopy() const;
protected:
	quint64 readItems(T * & _items, quint64 _items_count);
private:
	T * items;
	quint64 items_count;
	bool deep_copy;
};

// ------------------------------------------------------------------------

template <class T>
class jFileStorage : public jStorage<T>
{
	COPY_FBD(jFileStorage)
public:
	jFileStorage();
	jFileStorage(const QString & _file_name, quint64 _offset = 0);
	~jFileStorage();

	virtual jFileStorage & setStorageFile(const QString & _file_name);
	quint64 storageSize() const;

	virtual jFileStorage & setOffset(quint64 _offset); // in bytes
	virtual quint64 offset() const;
protected:
	quint64 readItems(T * & _items, quint64 _items_count);
	quint64 offs;
	QFile file;
	QByteArray items;
};

// ------------------------------------------------------------------------
/*
class jWaveFile : public jFileStorage<qint64>
{
	COPY_FBD(jWaveFile)
public:
	static QVector<qint64> waveSegmentProcessing(const qint64 *, quint64, jStorage<qint64> *);

	jWaveFile();
	jWaveFile(const QString & _file_name);

	jFileStorage & setStorageFile(const QString & _file_name);
	quint64 storageSize() const;

	jFileStorage & setOffset(quint64 _offset = 0); // ignored

	qint16 channels() const;
	qint16 bits() const;
	int sampleRate() const;
protected:
	qint16 ch_count;
	qint16 bits_per_sample;
	int sample_rate;
};
*/
// ------------------------------------------------------------------------

#include "jstorage_p.h"

#endif
