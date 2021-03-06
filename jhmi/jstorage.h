#ifndef __JSTORAGE_H__
#define __JSTORAGE_H__

#include "jdefs.h"
#include "jstoragehandler.h"

// ------------------------------------------------------------------------

class jStorageInterface
{
	COPY_FBD(jStorageInterface)
	DECL_PROPERTIES(jStorageInterface)
public:
	jStorageInterface() {}
	virtual ~jStorageInterface() {}

	enum ProcessedItemType {Minimums = 0, Maximums = 1};
	virtual QVector< QMap<int, QByteArray> > processedArray(quint64 _start_item = 0, quint64 _end_item = 0, QByteArray * _x = 0) const = 0;

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

	virtual jStorageInterface & setChannels(int _count = 1) = 0;
	virtual int channels() const = 0;

	virtual jStorageHandler * storageControl() const = 0;

	virtual QByteArray exportLayers() const = 0;
	virtual bool importLayers(const QByteArray & _saved_layers) = 0;
};

// ------------------------------------------------------------------------

template <class T, class TX = T>
class jStorage : public jStorageInterface
{
	COPY_FBD(jStorage)
public:

	typedef QVector< QVector<T> > (*segment_func)(const QVector<T> &, jStorage<T, TX> *);
	static QVector< QVector<T> > defaultSegmentProcessing(const QVector<T> &, jStorage<T, TX> *);
	typedef bool (*less_func)(const T &, const T &, quint32);
	inline static bool defaultLess(const T &, const T &, quint32 _count);
	typedef bool (*greater_func)(const T &, const T &, quint32);
	inline static bool defaultGreater(const T &, const T &, quint32 _count);

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

	QVector< QMap< int, QVector<T> > > processedItems(quint64 _start_item = 0, quint64 _end_item = 0, QVector<TX> * _x = 0) const;
	QVector< QMap< int, QByteArray> > processedArray(quint64 _start_item = 0, quint64 _end_item = 0, QByteArray * _x = 0) const;

	T item(quint64 _item_index, int _channel) const;

	jStorageInterface & setProcessedItemsHint(quint64 _count = 1024);
	quint64 processedItemsHint() const;

	void startProcessing(int _priority = QThread::LowestPriority);
	void stopProcessing();
	bool isProcessingFinished() const;
	quint64 itemsProcessed() const;

	jStorageInterface & setChannels(int _count = 1);
	int channels() const;

	jStorageHandler * storageControl() const;

	QByteArray exportLayers() const;
	bool importLayers(const QByteArray & _saved_layers);
protected:
	virtual jStorage & setPosition(quint64 _item_position);
	quint64 position() const;
	virtual QVector<T> readItems(quint64 _items_count) = 0;
private:
	quint64 seek_pos, seg_size, hint;
	int ch_count;
	segment_func seg_func;
	less_func l_func;
	greater_func g_func;
	class jStorageThread;
	jStorageThread * thread;
	jStorageHandler * handler;
};

// ------------------------------------------------------------------------

template <class T, class TX = T>
class jMemoryStorage : public jStorage<T, TX>
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
	QVector<T> readItems(quint64 _items_count);
private:
	T * items;
	quint64 items_count;
	bool deep_copy;
};

// ------------------------------------------------------------------------

template <class T, class TX = T>
class jIODeviceStorage : public jStorage<T, TX>
{
	COPY_FBD(jIODeviceStorage)
public:
	jIODeviceStorage();
	jIODeviceStorage(QIODevice * _io_device, quint64 _offset = 0);
	~jIODeviceStorage();

	virtual jIODeviceStorage & setStorageIODevice(QIODevice * _io_device);
	quint64 storageSize() const;

	virtual jIODeviceStorage & setOffset(quint64 _offset); // in bytes
	virtual quint64 offset() const;

	jIODeviceStorage & setCacheSize(quint64 _size = 0);
	quint64 cacheSize() const;

	QIODevice * IODevice() const;
protected:
	QVector<T> readItems(quint64 _items_count);
	quint64 offs, cache_start, cache_end, cache_size;
	QIODevice * io_device;
	QByteArray items;
};

// ------------------------------------------------------------------------

template <class T, class TX = T>
class jFileStorage : public jIODeviceStorage<T, TX>
{
	COPY_FBD(jFileStorage)
public:
	jFileStorage();
	jFileStorage(const QString & _file_name, quint64 _offset = 0);

	virtual jFileStorage & setStorageFile(const QString & _file_name);
protected:
	QFile file;
};

// ------------------------------------------------------------------------

#include "jstorage_p.h"

#endif
