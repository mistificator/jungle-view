#ifndef __JSTORAGEPRIVATE_H__
#define __JSTORAGEPRIVATE_H__

#include "jdefs.h"

// ------------------------------------------------------------------------

template <class T, class TX>
jStorage<T, TX>::jStorage(): jStorageInterface() 
{
	seek_pos = 0;
	ch_count = 1;
	setProcessedItemsHint();
	setSegmentFunc();
	setSegmentSize();
	setLessFunc();
	setGreaterFunc();

	handler = new jStorageHandler(this);
	thread = new jStorageThread(this);
}

template <class T, class TX>
jStorage<T, TX>::~jStorage() 
{
	delete thread;
	delete handler;
}

template <class T, class TX>
int jStorage<T, TX>::itemSize() const
{
	return sizeof(T);
}

template <class T, class TX>
QVector< QMap< int, QVector<T> > > jStorage<T, TX>::processedItems(quint64 _start_item, quint64 _end_item, QVector<TX> * _x) const
{
	return thread->items(_start_item, _end_item ? _end_item : storageSize() / channels(), _x);
}

template <class T, class TX>
T jStorage<T, TX>::item(quint64 _item_index, int _channel) const
{
	QVector<T> _items = processedItems(_item_index, _item_index + 1)[_channel][Maximums];
	return _items.count() ? _items[0] : 0;
}

template <class T, class TX>
QVector< QMap< int, QByteArray> > jStorage<T, TX>::processedArray(quint64 _start_item, quint64 _end_item, QByteArray * _x) const
{
	QVector<TX> _vx;
	QVector< QMap< int, QVector<T> > > _items = thread->items(_start_item, _end_item ? _end_item : storageSize() / channels(), _x ? &_vx : 0);
	QVector< QMap< int, QByteArray> > _array;
	_array.resize(_items.count());
	for (int _idx = 0; _idx < _items.count(); _idx++)
	{
		foreach(int _type, _items[_idx].keys())
		{
			_array[_idx][_type] = QByteArray::fromRawData(reinterpret_cast<char *>(_items[_idx][_type].data()), _items[_idx][_type].count() * itemSize());
		}
	}
	if (_x)
	{
		* _x = QByteArray::fromRawData(reinterpret_cast<char *>(_vx.data()), _vx.count() * sizeof(TX));
	}
	return _array;
}

template <class T, class TX>
jStorage<T, TX> & jStorage<T, TX>::setPosition(quint64 _item_position)
{
	seek_pos = (_item_position < storageSize()) ? _item_position : storageSize();
	return * this;
}

template <class T, class TX>
quint64 jStorage<T, TX>::position() const
{
	return seek_pos;
}

template <class T, class TX>
QVector< QVector<T> > jStorage<T, TX>::defaultSegmentProcessing(const QVector<T> & _items, jStorage<T, TX> * _storage)
{
	QVector< QVector<T> > _result;
	const int _channels = _storage->channels();
	_result.resize(_channels);
	if (_result.count() == 1)
	{
		_result[0] = _items;
	}
	else		
	{
		for (int _channel = 0; _channel < _channels; _channel++)
		{
			QVector<T> & _channel_result = _result[_channel];
			_channel_result.resize(_items.count() / _channels);
			T * _channel_result_data = _channel_result.data();
			const T * _items_data = _items.constData();
			const T * _items_data_end = _items_data + _items.count();
			_items_data += _channel;
			for (; _items_data < _items_data_end; _items_data += _channels, _channel_result_data++)
			{
				* _channel_result_data = * _items_data;
			}
		}
	}
	return _result;
}

template <class T, class TX>
jStorageInterface & jStorage<T, TX>::setSegmentSize(quint64 _size)
{
	seg_size = _size;
	return * this;
}

template <class T, class TX>
quint64 jStorage<T, TX>::segmentSize() const
{
	return seg_size;
}

template <class T, class TX>
jStorage<T, TX> & jStorage<T, TX>::setSegmentFunc(segment_func _segment_func)
{
	seg_func = _segment_func;
	return * this;
}

template <class T, class TX>
typename jStorage<T, TX>::segment_func jStorage<T, TX>::segmentFunc() const
{
	return (segment_func)seg_func;
}

template <class T, class TX>
bool jStorage<T, TX>::defaultLess(const T & _op1, const T & _op2, quint32 _count)
{
	if (_count <= 1)
	{
		return _op1 < _op2;
	}
	quint32 _stsfy = 0;
	for (quint32 _idx = 0; _idx < _count; _idx++)
	{
		if (*(&_op1 + _idx) < *(&_op2 + _idx))
		{
			_stsfy++;
		}
	}
	return _stsfy > (_count / 2);
}

template <class T, class TX>
jStorage<T, TX> & jStorage<T, TX>::setLessFunc(less_func _less_func)
{
	l_func = _less_func;
	return * this;
}

template <class T, class TX>
typename jStorage<T, TX>::less_func jStorage<T, TX>::lessFunc() const
{
	return l_func;
}

template <class T, class TX>
bool jStorage<T, TX>::defaultGreater(const T & _op1, const T & _op2, quint32 _count)
{
	if (_count <= 1)
	{
		return _op1 > _op2;
	}
	quint32 _stsfy = 0;
	for (quint32 _idx = 0; _idx < _count; _idx++)
	{
		if (*(&_op1 + _idx) > *(&_op2 + _idx))
		{
			_stsfy++;
		}
	}
	return _stsfy > (_count / 2);
}

template <class T, class TX>
jStorage<T, TX> & jStorage<T, TX>::setGreaterFunc(greater_func _greater_func)
{
	g_func = _greater_func;
	return * this;
}

template <class T, class TX>
typename jStorage<T, TX>::greater_func jStorage<T, TX>::greaterFunc() const
{
	return g_func;
}

template <class T, class TX>
jStorageInterface & jStorage<T, TX>::setProcessedItemsHint(quint64 _count)
{
	hint = _count ? _count : 1;
	return * this;
}

template <class T, class TX>
quint64 jStorage<T, TX>::processedItemsHint() const
{
	return hint;
}

template <class T, class TX>
void jStorage<T, TX>::startProcessing(int _priority)
{
	thread->stop();
	thread->start((QThread::Priority)_priority);
}

template <class T, class TX>
void jStorage<T, TX>::stopProcessing()
{
	thread->stop();
}

template <class T, class TX>
bool jStorage<T, TX>::isProcessingFinished() const
{
	return thread->isProcessingFinished();
}

template <class T, class TX>
quint64 jStorage<T, TX>::itemsProcessed() const
{
	return thread->itemsProcessed();
}

template <class T, class TX>
jStorageInterface & jStorage<T, TX>::setChannels(int _count)
{
	stopProcessing();
	if (_count < 1)
	{
		_count = 1;
	}
	ch_count = _count;
	return * this;
}

template <class T, class TX>
int jStorage<T, TX>::channels() const
{
	return ch_count;
}

template <class T, class TX>
jStorageHandler * jStorage<T, TX>::storageControl() const
{
	return handler;
}

template <class T, class TX>
QByteArray jStorage<T, TX>::exportLayers() const
{
	return thread->exportLayers();
}

template <class T, class TX>
bool jStorage<T, TX>::importLayers(const QByteArray & _saved_layers)
{
	return thread->importLayers(_saved_layers);
}
// ------------------------------------------------------------------------

template <class T, class TX>
jMemoryStorage<T, TX>::jMemoryStorage(): jStorage<T, TX>()
{
	items = 0;
}

template <class T, class TX>
jMemoryStorage<T, TX>::jMemoryStorage(T * _items, quint64 _items_count, bool _deep_copy): jStorage<T, TX>()
{
	items = 0;
	setStorageBuffer(_items, _items_count, _deep_copy);
}

template <class T, class TX>
jMemoryStorage<T, TX>::~jMemoryStorage()
{
	stopProcessing();
	if (deep_copy && items)
	{
		delete [] items;
	}
}

template <class T, class TX>
QVector<T> jMemoryStorage<T, TX>::readItems(quint64 _items_count)
{
	T * _items = items + (position() / channels()) * channels();
	if (_items_count + position() > items_count)
	{
		if (position() > items_count)
		{
			return QVector<T>();
		}
		_items_count = items_count - position();
	}
	QVector<T> _result;
	_result.resize(_items_count);
    ::memcpy(_result.data(), _items, _items_count * itemSize());
	return _result;
}

template <class T, class TX>
quint64 jMemoryStorage<T, TX>::storageSize() const
{
	return items_count;
}

template <class T, class TX>
jMemoryStorage<T, TX> & jMemoryStorage<T, TX>::setStorageBuffer(T * _items, quint64 _items_count, bool _deep_copy)
{
	stopProcessing();
	if (deep_copy && items)
	{
		delete [] items;
	}
	deep_copy = _deep_copy;
	items_count = _items_count;
	if (deep_copy)
	{
		items = new T[items_count];
        ::memcpy(items, _items, items_count * itemSize());
	}
	else
	{
		items = _items;
	}
	return * this;
}

template <class T, class TX>
bool jMemoryStorage<T, TX>::isDeepCopy() const
{
	return deep_copy;
}

// ------------------------------------------------------------------------

template <class T, class TX>
jIODeviceStorage<T, TX>::jIODeviceStorage(): jStorage<T, TX>(), io_device(0)
{
	setOffset(0);
	setCacheSize(0);
	cache_start = 0;
	cache_end = 0;
}

template <class T, class TX>
jIODeviceStorage<T, TX>::jIODeviceStorage(QIODevice * _io_device, quint64 _offset): jStorage<T, TX>(), io_device(0)
{
	setOffset(_offset);
	setCacheSize(0);
	cache_start = 0;
	cache_end = 0;
	setStorageIODevice(_io_device);
}

template <class T, class TX>
jIODeviceStorage<T, TX>::~jIODeviceStorage()
{
	stopProcessing();
	if (io_device && io_device->isOpen())
	{
		io_device->close();
	}
}

template <class T, class TX>
jIODeviceStorage<T, TX> & jIODeviceStorage<T, TX>::setStorageIODevice(QIODevice * _io_device)
{
	stopProcessing();
	if (io_device && io_device->isOpen())
	{
		io_device->close();
	}
	io_device = _io_device;
	if (io_device && (!io_device->isOpen()))
	{
		io_device->open(QIODevice::ReadOnly);
	}
	return * this;
}

template <class T, class TX>
quint64 jIODeviceStorage<T, TX>::storageSize() const
{
	quint64 _size = io_device ? (io_device->size() - offs) / itemSize() : 0;
	return _size;
}

template <class T, class TX>
QVector<T> jIODeviceStorage<T, TX>::readItems(quint64 _items_count)
{
	if (!io_device)
	{
		return QVector<T>();
	}
	const qint64 _file_size = io_device->size();
	const qint64 _bytes_to_read = _items_count * itemSize();
	qint64 _pos_start = offs + (position() * itemSize() / channels()) * channels();
	qint64 _pos_end = _pos_start + _bytes_to_read;
	_pos_start = qMax<qint64>(offs, _pos_start);
	_pos_end = qMin<qint64>(_file_size, _pos_start + _bytes_to_read);
	if ((_pos_start < (qint64)cache_start) || (_pos_end > (qint64)cache_end))
	{
		if (_items_count > segmentSize())
		{
			cache_start = qMin<qint64>(qMax<qint64>(_pos_start - (cache_size / 2), offs), _file_size);
			cache_end = qMax<qint64>(qMin<qint64>(_pos_end + (cache_size / 2), _file_size), offs);
		}
		else
		{
			cache_start = _pos_start;
			cache_end = _pos_end;
		}
		const qint64 _bytes_to_physical_read = cache_end - cache_start;
		if (items.count() < _bytes_to_physical_read)
		{
			items.resize(_bytes_to_physical_read);
		}
		io_device->seek(cache_start);
		io_device->read(items.data(), _bytes_to_physical_read);
	}
	
	if (cache_start > cache_end)
	{
		cache_start = cache_end;
	}
	if (_pos_start > _pos_end)
	{
		_pos_start = _pos_end;
	}
	const qint64 _count = _pos_end - _pos_start;
	QVector<T> _result;
	_result.resize(_count / itemSize());
	if (!_result.isEmpty() && items.count() >= (_pos_start - cache_start) + _count)
	{
        ::memcpy(_result.data(), items.constData() + (_pos_start - cache_start), _count);
	}
	return _result;
}

template <class T, class TX>
jIODeviceStorage<T, TX> & jIODeviceStorage<T, TX>::setOffset(quint64 _offset)
{
	stopProcessing();
	offs = _offset;
	return * this;
}

template <class T, class TX>
quint64 jIODeviceStorage<T, TX>::offset() const
{
	return offs;
}

template <class T, class TX>
jIODeviceStorage<T, TX> &  jIODeviceStorage<T, TX>::setCacheSize(quint64 _size)
{
	cache_size = _size;
	return * this;
}

template <class T, class TX>
quint64  jIODeviceStorage<T, TX>::cacheSize() const
{
	return cache_size;
}

template <class T, class TX>
QIODevice * jIODeviceStorage<T, TX>::IODevice() const
{
	return io_device;
}

// ------------------------------------------------------------------------

template <class T, class TX>
jFileStorage<T, TX>::jFileStorage(): jIODeviceStorage<T, TX>()
{
	setStorageIODevice(&file);
}

template <class T, class TX>
jFileStorage<T, TX>::jFileStorage(const QString & _file_name, quint64 _offset): jIODeviceStorage<T, TX>()
{
	setOffset(_offset);
	setStorageFile(_file_name);
}

template <class T, class TX>
jFileStorage<T, TX> & jFileStorage<T, TX>::setStorageFile(const QString & _file_name)
{
	if (file.isOpen())
	{
		file.close();
	}
	file.setFileName(_file_name);
	file.setPermissions(QFile::ReadOther);
	setStorageIODevice(&file);
	return * this;
}

// ------------------------------------------------------------------------

template <class T, class TX>
class jStorage<T, TX>::jStorageThread : public QThread
{
public:
    jStorageThread(jStorage<T, TX> * _storage);
	~jStorageThread();
	QVector< QMap<int, QVector<T> > > items(quint64 _lo, quint64 _hi, QVector<TX> * _x) const;
	void stop();
	bool isProcessingFinished() const;
	QByteArray exportLayers() const;
	bool importLayers(const QByteArray & _saved_layers);
	quint64 itemsProcessed() const;
protected:
	void run();
private:
	jStorage<T, TX> * storage;
	bool finished;
	bool stop_thread;
	quint64 items_processed;
	bool first_run;

	struct Layer
	{
		qreal seg_count;
		QVector<bool> processed;
		QVector<T> min, max;
		QVector<TX> x;
	};
	QVector< QVector<Layer> > layers;
	void splitStorage();
	void segmentState(int _layer_number, quint64 _segment_number, quint64 & _lo_item, quint64 & _hi_item, bool & _processed) const;
	void prevLayerRange(int _layer_number, quint64 _segment_number, quint64 & _lo_segment_item, quint64 & _hi_segment_item);
	bool adjustLayers();
	int selectLayer(quint64 _lo_item, quint64 _hi_item) const;
	QVector< QVector<T> > preprocessedReadItems(quint64 _lo_item, quint64 _hi_item) const;
};

template <class T, class TX>
jStorage<T, TX>::jStorageThread::jStorageThread(jStorage<T, TX> * _storage) : QThread()
{
	storage = _storage;
	finished = true;
	stop_thread = false;
	items_processed = 0;
}

template <class T, class TX>
jStorage<T, TX>::jStorageThread::~jStorageThread()
{
	stop();
}

template <class T, class TX>
void jStorage<T, TX>::jStorageThread::run()
{
	first_run = true;
	splitStorage();
	QDateTime _time_stamp = QDateTime::currentDateTime();
	while ((!stop_thread) && (!finished))
	{
		adjustLayers();
		QMetaObject::invokeMethod(storage->storageControl(), "layersAdjusted", Qt::BlockingQueuedConnection);
		msleep(1);
	}
	quint64 _msecs = _time_stamp.msecsTo(QDateTime::currentDateTime());
//	JDEBUG("thread finished" << _msecs);

	if (finished)
	{
		items_processed = storage->storageSize();
		QMetaObject::invokeMethod(storage->storageControl(), "finished", Qt::QueuedConnection, Q_ARG(quint64, _msecs));
	}
	if (stop_thread)
	{
		QMetaObject::invokeMethod(storage->storageControl(), "stopped", Qt::QueuedConnection);
	}

	stop_thread = false;
}

template <class T, class TX>
void jStorage<T, TX>::jStorageThread::stop()
{
	stop_thread = true;
	wait();
	stop_thread = false;
}

template <class T, class TX>
bool jStorage<T, TX>::jStorageThread::isProcessingFinished() const
{
	return finished;
}

template <class T, class TX>
void jStorage<T, TX>::jStorageThread::splitStorage()
{
	const int _channels = storage->channels();
	finished = false;
	layers.clear();
	layers.resize(_channels);
	items_processed = 0;
	qreal _seg_count = (qreal)storage->storageSize() / storage->segmentSize();
	do 
	{
		for (int _channel = 0; _channel < _channels; _channel++)
		{
			Layer _layer;
			_layer.seg_count = _seg_count;
			_layer.processed.resize(::ceil(_seg_count));
			_layer.processed.fill(false, ::ceil(_seg_count));
			_layer.min.resize(::ceil(_seg_count));
			_layer.max.resize(::ceil(_seg_count));
			_layer.x.resize(::ceil(_seg_count));
			layers[_channel] << _layer;
		}
		_seg_count = _seg_count / 2;
	} 
	while (_seg_count > storage->processedItemsHint());
}

template <class T, class TX>
void jStorage<T, TX>::jStorageThread::segmentState(int _layer_number, quint64 _segment_number, quint64 & _lo_item, quint64 & _hi_item, bool & _processed) const
{
	if ((_layer_number < 0) || (layers.count() == 0) || (_layer_number >= layers[0].count()))
	{
		_lo_item = 0;
		_hi_item = 0;
		_processed = false;
		return;
	}
	const Layer & _layer = layers[0][_layer_number];
	if (_segment_number >= ::ceil(_layer.seg_count))
	{
		_lo_item = 0;
		_hi_item = 0;
		_processed = false;
		return;
	}
	_processed = _layer.processed[_segment_number];
	_lo_item = storage->storageSize() * _segment_number / _layer.seg_count;
	_hi_item = storage->storageSize() * (_segment_number + 1) / _layer.seg_count;
}

template <class T, class TX>
void jStorage<T, TX>::jStorageThread::prevLayerRange(int _layer_number, quint64 _segment_number, quint64 & _lo_segment_item, quint64 & _hi_segment_item)
{
	if ((_layer_number < 1) || (layers.count() == 0) || (_layer_number >= layers[0].count()))
	{
		_lo_segment_item = 0;
		_hi_segment_item = 0;
		return;
	}
	Layer _layer = layers[0][_layer_number];
	if (_segment_number >= ::ceil(_layer.seg_count))
	{
		_lo_segment_item = 0;
		_hi_segment_item = 0;
		return;
	}
	Layer _prev_layer = layers[0][_layer_number - 1];
	_lo_segment_item = _prev_layer.seg_count * _segment_number / _layer.seg_count;
	_hi_segment_item = _prev_layer.seg_count * (_segment_number + 1) / _layer.seg_count;
	if (_hi_segment_item > _prev_layer.seg_count)
	{
		_hi_segment_item = _prev_layer.seg_count;
	}
}


template <class T, class TX>
bool jStorage<T, TX>::jStorageThread::adjustLayers()
{
//	JTIME_START
	if ((layers.count() == 0) || finished)
	{
		return true;
	}
	const int _layers_count = layers[0].count();
	if (_layers_count == 0)
	{
		return true;
	}
	jStorage<T, TX>::less_func _less_func = storage->lessFunc();
	jStorage<T, TX>::greater_func _greater_func = storage->greaterFunc();
	const quint64 _seg_count = (layers[0][0].seg_count);
	if (_seg_count < storage->processedItemsHint())
	{
		finished = true;
		return true;
	}
	bool _layer0_finished = true;
	quint64 _step = ::pow(2.0f, _layers_count);
	const quint64 _seg_size0 = storage->segmentSize();
	const int _channels = storage->channels();

	bool * _processed_copy = layers[0][0].processed.data();

//	JTIME_DIFF("starting layer0 cycle")
	for (quint64 _seg_idx = 0; _seg_idx < _seg_count; )
	{
		if (_processed_copy[_seg_idx])
		{
			_seg_idx++;
			continue;
		}
		bool _processed;
		quint64 _lo, _hi;
		segmentState(0, _seg_idx, _lo, _hi, _processed);
		if (_hi < _lo)
		{
			_hi = _lo;
		}
		const QVector< QVector<T> > & _items = preprocessedReadItems(_lo, _hi);
		for (int _channel = 0; _channel < _items.count(); _channel++)
		{
			const T * _items_data = _items[_channel].constData();
			const quint64 _items_count = _items[_channel].count();
			T _min = * _items_data;
			T _max = * _items_data;
			const T * _items_data_end = _items_data + _items_count;
			_items_data++;
			for (; _items_data < _items_data_end; _items_data++)
			{
				if (_less_func(* _items_data, _min, 1))
				{
					_min = * _items_data;
				}
				else
				{
					if (_greater_func(* _items_data, _max, 1))
					{
						_max = * _items_data;
					}
				}
			}
			items_processed += _items_count;
			const quint64 _seg_idx_sub_end = qMin<quint64>(_seg_count, _seg_idx + _step);
			Layer & _layer0 = layers[_channel][0];
			_layer0.min[_seg_idx] = _min;
			_layer0.max[_seg_idx] = _max;
			_layer0.x[_seg_idx] = (qreal)(_seg_size0 * _seg_idx) / _channels;
			_layer0.processed[_seg_idx] = true;
			if (first_run)
			{
				for (quint64 _seg_idx_sub = _seg_idx + 1; _seg_idx_sub < _seg_idx_sub_end; _seg_idx_sub++)
				{
					if (_layer0.processed[_seg_idx_sub] == false)
					{
						_layer0.min[_seg_idx_sub] = _min;
						_layer0.max[_seg_idx_sub] = _max;
						_layer0.x[_seg_idx_sub] = (qreal)(_seg_size0 * _seg_idx_sub) / _channels;
					}
				}
			}
		}
		_seg_idx += _step;
		_layer0_finished = false;
	}
//	JTIME_DIFF("layer0 iteration ended")

	if (first_run)
	{
		first_run = false;
	}

	if (stop_thread)
	{
		return false;
	}

	for (int _channel = 0; _channel < storage->channels(); _channel++)
	{
		for (int _layer_idx = 1; _layer_idx < _layers_count; _layer_idx++)
		{
			quint64 _lo_segment, _hi_segment;
			Layer & _layer = layers[_channel][_layer_idx];
			const Layer & _prev_layer = layers[_channel][_layer_idx - 1];
			const quint64 _seg_count = ::ceil(_layer.seg_count);
			const qreal _seg_size = storage->storageSize() / _layer.seg_count;
			for (quint64 _seg_idx = 0; _seg_idx < _seg_count; _seg_idx++)
			{
				prevLayerRange(_layer_idx, _seg_idx, _lo_segment, _hi_segment);
				const bool * _pl_processed = _prev_layer.processed.constData();
				const T * _pl_min = _prev_layer.min.constData() + _lo_segment;
				const T * _pl_max = _prev_layer.max.constData() + _lo_segment;
				const T * _pl_max_end = _pl_max + (_hi_segment - _lo_segment);
				T _min = * _pl_min;
				T _max = * _pl_max;
				for (; _pl_max < _pl_max_end; _pl_processed++, _pl_min++, _pl_max++)
				{
					if (* _pl_processed == false)
					{
						break;
					}
					if (_less_func(* _pl_min, _min, 1))
					{
						_min = * _pl_min;
					}
					else
					{
						if (_greater_func(* _pl_max, _max, 1))
						{
							_max = * _pl_max;
						}
					}
				}
				_layer.min[_seg_idx] = _min;
				_layer.max[_seg_idx] = _max;
				_layer.x[_seg_idx] = (qreal)(_seg_idx * _seg_size) / _channels;
				_layer.processed[_seg_idx] = true;
				if (stop_thread)
				{
					return false;
				}
			}
		}

	}

//	JTIME_DIFF("adjusting iteration ended")
//	JTIME_ELAPSED("total")

	finished = _layer0_finished;
	return finished;
}

template <class T, class TX>
int jStorage<T, TX>::jStorageThread::selectLayer(quint64 _lo_item, quint64 _hi_item) const
{
	if (layers.count() == 0)
	{
		return -1;
	}
	int _layer_idx = 0;
	for (; _layer_idx < layers[0].count(); _layer_idx++)
	{
		const quint64 _seg_size = storage->storageSize() / layers[0][_layer_idx].seg_count;
		const qreal _selected_seg_count = (qreal)(_hi_item - _lo_item) / _seg_size;
		if (_selected_seg_count < storage->processedItemsHint())
		{
			break;
		}
	}
	_layer_idx--;
	return _layer_idx;
}

template <class T, class TX>
QVector< QMap<int, QVector<T> > > jStorage<T, TX>::jStorageThread::items(quint64 _lo_item, quint64 _hi_item, QVector<TX> * _x) const
{
//	JTIME_START
	jStorage<T, TX>::less_func _less_func = storage->lessFunc();
	jStorage<T, TX>::greater_func _greater_func = storage->greaterFunc();
	QVector< QMap<int, QVector<T> > > _result;
	const int _channels = storage->channels();
	_result.resize(_channels);
	_lo_item *= _channels;
	_hi_item *= _channels;
	int _selected_layer = selectLayer(_lo_item, _hi_item);
	if (_selected_layer == -1)
	{
		QVector< QVector<T> > _items = preprocessedReadItems(_lo_item, _hi_item);
//		JTIME_DIFF("getting preprocessed items")

		for (int _channel = 0; _channel < _items.count(); _channel++)
		{
			qreal _seg_size = (qreal) _items[_channel].count() / storage->processedItemsHint();
			if (_seg_size < 1.0)
			{
				_seg_size = 1.0;
			}
			const quint64 _items_count = _items[_channel].count();

			QVector<T> & _min = _result[_channel][jStorage<T, TX>::Minimums];
			QVector<T> & _max = _result[_channel][jStorage<T, TX>::Maximums];

			const quint64 _arr_sz = _seg_size <= 1.0 ? _items_count : storage->processedItemsHint();

			_min.resize(_arr_sz);
			_max.resize(_arr_sz);

			const T * _items_data = _items[_channel].constData();
			T * _min_data = _min.data();
			T * _max_data = _max.data();

			const qreal _lo_seg = _lo_item / _channels;
			const qreal _lo_seg_stable = (quint64)(_lo_seg / _seg_size) * _seg_size;
			const qreal _lo_seg_div = _lo_seg_stable / _seg_size;
			const qreal _hi_seg = _lo_seg + _items_count;
			if (_x)
			{
				_x->resize(_arr_sz);
				TX * _x_data = _x->data();
				const TX * _x_data_end = _x_data + _arr_sz;
				qreal _value = _lo_seg_stable;
				for (; _x_data < _x_data_end; _x_data++, _value += _seg_size)
				{
					* _x_data = _value;
				}
//				JTIME_DIFF("reshaping array x channel" << _channel)
			}
			for (qreal _idx = _lo_seg_stable + _seg_size; _idx < _hi_seg; _idx += _seg_size)
			{
				const quint64 _index = (_idx / _seg_size) - _lo_seg_div;
				if (_index >= _arr_sz)
				{
					break;
				}
				const quint64 _idx_offs = _idx - _lo_seg;

				_min_data[_index] = _items_data[_idx_offs];
				_max_data[_index] = _items_data[_idx_offs];
			}
//			JTIME_DIFF("init min&max channel" << _channel)
			qreal _index_r = (_lo_seg / _seg_size) - _lo_seg_div;
			const T * _items_data_end = _items_data + (quint64)(_hi_seg - _lo_seg);
			quint64 _prev_index = _index_r;
			T * _min_item = _min_data;
			T * _max_item = _max_data;
			* _min_item = * _items_data;
			* _max_item = * _items_data;
			for (; _items_data < _items_data_end; _items_data++, _index_r += 1.0/_seg_size)
			{
				if ((quint64)_index_r != _prev_index)
				{
					_prev_index = _index_r;
					_min_item++;
					_max_item++;
				}
				if (_less_func(* _items_data, * _min_item, 1))
				{
					* _min_item = * _items_data;
				}
				else
				{
					if (_greater_func(* _items_data, * _max_item, 1))
					{
						* _max_item = * _items_data;
					}
				}
			}
//			JTIME_DIFF("reshaping arrays min&max channel" << _channel)
		}
	}
	else
	{
		for (int _channel = 0; _channel < storage->channels(); _channel++)
		{
			qreal _seg_lo = _lo_item * layers[_channel][_selected_layer].seg_count / storage->storageSize();
			qreal _seg_hi = _hi_item * layers[_channel][_selected_layer].seg_count / storage->storageSize();
			if (_seg_hi - _seg_lo < 1.0f)
			{
				_seg_hi += 1.0f;
			}
			_result[_channel][jStorage<T, TX>::Minimums] = layers[_channel][_selected_layer].min.mid(_seg_lo, _seg_hi - _seg_lo);
			_result[_channel][jStorage<T, TX>::Maximums] = layers[_channel][_selected_layer].max.mid(_seg_lo, _seg_hi - _seg_lo);

		}
		if (_x)
		{
			qreal _seg_lo = _lo_item * layers[0][_selected_layer].seg_count / storage->storageSize();
			qreal _seg_hi = _hi_item * layers[0][_selected_layer].seg_count / storage->storageSize();
			* _x = layers[0][_selected_layer].x.mid(_seg_lo, _seg_hi - _seg_lo);
		}

//		JTIME_DIFF("parsing layers")
	}
//	JTIME_ELAPSED("total")
	return _result;
}

template <class T, class TX>
QByteArray jStorage<T, TX>::jStorageThread::exportLayers() const
{
	QByteArray _exported;
	QDataStream ds(&_exported, QIODevice::WriteOnly);
	ds << storage->storageSize();
	ds << layers.count();
	ds << storage->segmentSize();
	ds << storage->processedItemsHint();
	for (int _channel = 0; _channel < layers.count(); _channel++)
	{
		ds << layers[_channel].count();
		foreach (const Layer & _layer, layers[_channel])
		{
			ds << _layer.seg_count;
			ds << _layer.processed;
			ds << _layer.min;
			ds << _layer.max;
			ds << _layer.x;
		}
	}
	return qCompress(_exported);
}

template <class T, class TX>
bool jStorage<T, TX>::jStorageThread::importLayers(const QByteArray & _saved_layers)
{
	QDateTime _time_stamp = QDateTime::currentDateTime();
	stop();
	QDataStream ds(qUncompress(_saved_layers));
	quint64 _storage_size;
	ds >> _storage_size;
	if (_storage_size != storage->storageSize())
	{
		JDEBUG("import failed, wrong size");
		return false;
	}
	int _channels;
	ds >> _channels;
	if (_channels != storage->channels())
	{
		JDEBUG("import failed, wrong channels number");
		return false;
	}
	quint64 _segment_size;
	ds >> _segment_size;
	quint64 _hint;
	ds >> _hint;
	layers.clear();
	layers.resize(_channels);
	for (int _channel = 0; _channel < _channels; _channel++)
	{
		int _layers_count;
		ds >> _layers_count;
		layers[_channel].resize(_layers_count);
		for (int _idx = 0; _idx < _layers_count; _idx++)
		{
			JDEBUG("importing channel" << _channel + 1 << "/" << _channels << "layer" << _idx + 1 << "/" << _layers_count);
			Layer & _layer = layers[_channel][_idx];
			ds >> _layer.seg_count;
			ds >> _layer.processed;
			ds >> _layer.min;
			ds >> _layer.max;
			ds >> _layer.x;
		}
	}
	finished = true;
	items_processed = storage->storageSize();
	storage->setSegmentSize(_segment_size);
	storage->setProcessedItemsHint(_hint);
	quint64 _msecs = _time_stamp.msecsTo(QDateTime::currentDateTime());
	JDEBUG("import finished" << _msecs);
	QMetaObject::invokeMethod(storage->storageControl(), "layersAdjusted", Qt::QueuedConnection);
	QMetaObject::invokeMethod(storage->storageControl(), "finished", Qt::QueuedConnection, Q_ARG(quint64, _msecs));
	return true;
}

template <class T, class TX>
quint64 jStorage<T, TX>::jStorageThread::itemsProcessed() const
{
	return items_processed;
}

template <class T, class TX>
QVector< QVector<T> > jStorage<T, TX>::jStorageThread::preprocessedReadItems(quint64 _lo, quint64 _hi) const
{
	storage->setPosition(_lo);
	return storage->segmentFunc()(storage->readItems(_hi - _lo), storage);
}

// ------------------------------------------------------------------------

#endif
