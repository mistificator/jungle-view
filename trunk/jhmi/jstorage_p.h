#ifndef __JSTORAGEPRIVATE_H__
#define __JSTORAGEPRIVATE_H__

#include "jdefs.h"

// ------------------------------------------------------------------------

template <class T>
jStorage<T>::jStorage(): jStorageInterface() 
{
	seek_pos = 0;
	setProcessedItemsHint();
	setSegmentFunc();
	setSegmentSize();
	setLessFunc();
	setGreaterFunc();

	handler = new jStorageHandler(this);
	thread = new jStorageThread(this, & rw_lock);
}

template <class T>
jStorage<T>::~jStorage() 
{
	delete thread;
	delete handler;
}

template <class T>
int jStorage<T>::itemSize() const
{
	return sizeof(T);
}

template <class T>
QMap<int, QVector<T> > jStorage<T>::processedItems(quint64 _start_item, quint64 _end_item) const
{
	return thread->items(_start_item, _end_item ? _end_item : storageSize());
}

template <class T>
QMap<int, QByteArray> jStorage<T>::processedArray(quint64 _start_item, quint64 _end_item) const
{
	QMap<int, QVector<T> > _items = thread->items(_start_item, _end_item ? _end_item : storageSize());
	QMap<int, QByteArray> _array;
	foreach(int _type, _items.keys())
	{
		_array[_type] = QByteArray::fromRawData(reinterpret_cast<const char *>(_items[_type].constData()), _items[_type].count() * sizeof(T));
	}
	return _array;
}

template <class T>
jStorage<T> & jStorage<T>::setPosition(quint64 _item_position)
{
	SAFE_SET(seek_pos, (_item_position < storageSize()) ? _item_position : storageSize());
	return * this;
}

template <class T>
quint64 jStorage<T>::position() const
{
	return seek_pos;
}

template <class T>
QVector<T> jStorage<T>::defaultSegmentProcessing(const QVector<T> & _src)
{
	return _src;
}

template <class T>
jStorageInterface & jStorage<T>::setSegmentSize(quint64 _size)
{
	SAFE_SET(seg_size, _size);
	return * this;
}

template <class T>
quint64 jStorage<T>::segmentSize() const
{
	return seg_size;
}

template <class T>
jStorage<T> & jStorage<T>::setSegmentFunc(segment_func _segment_func)
{
	SAFE_SET(seg_func, _segment_func);
	return * this;
}

template <class T>
typename jStorage<T>::segment_func jStorage<T>::segmentFunc() const
{
	return (segment_func)seg_func;
}

template <class T>
bool jStorage<T>::defaultLess(const T & _op1, const T & _op2)
{
	return _op1 < _op2;
}

template <class T>
jStorage<T> & jStorage<T>::setLessFunc(less_func _less_func = & defaultLess)
{
	SAFE_SET(l_func, _less_func);
	return * this;
}

template <class T>
typename jStorage<T>::less_func jStorage<T>::lessFunc() const
{
	return l_func;
}

template <class T>
bool jStorage<T>::defaultGreater(const T & _op1, const T & _op2)
{
	return _op1 > _op2;
}

template <class T>
jStorage<T> & jStorage<T>::setGreaterFunc(greater_func _greater_func = & defaultGreater)
{
	SAFE_SET(g_func, _greater_func);
	return * this;
}

template <class T>
typename jStorage<T>::greater_func jStorage<T>::greaterFunc() const
{
	return g_func;
}

template <class T>
jStorageInterface & jStorage<T>::setProcessedItemsHint(quint64 _count)
{
	SAFE_SET(hint, _count ? _count : 1);
	return * this;
}

template <class T>
quint64 jStorage<T>::processedItemsHint() const
{
	return hint;
}

template <class T>
void jStorage<T>::startProcessing(int _priority)
{
	thread->start((QThread::Priority)_priority);
}

template <class T>
void jStorage<T>::stopProcessing()
{
	thread->stop();
}

template <class T>
bool jStorage<T>::isProcessingFinished() const
{
	return thread->isProcessingFinished();
}

template <class T>
quint64 jStorage<T>::itemsProcessed() const
{
	return thread->itemsProcessed();
}

template <class T>
jStorageHandler * jStorage<T>::storageControl() const
{
	return handler;
}

template <class T>
QByteArray jStorage<T>::exportLayers() const
{
	return thread->exportLayers();
}

template <class T>
bool jStorage<T>::importLayers(const QByteArray & _saved_layers)
{
	return thread->importLayers(_saved_layers);
}
// ------------------------------------------------------------------------

template <class T>
jMemoryStorage<T>::jMemoryStorage(): jStorage<T>()
{
	items = 0;
}

template <class T>
jMemoryStorage<T>::jMemoryStorage(T * _items, quint64 _items_count, bool _deep_copy): jStorage<T>()
{
	items = 0;
	setStorageBuffer(_items, _items_count, _deep_copy);
}

template <class T>
jMemoryStorage<T>::~jMemoryStorage()
{
	stopProcessing();
	if (deep_copy && items)
	{
		delete [] items;
	}
}

template <class T>
quint64 jMemoryStorage<T>::readItems(T * & _items, quint64 _items_count)
{
	THREAD_SAFE(Read)
	_items = items + position();
	_items_count = (_items_count + position() > items_count) ? items_count - position() : _items_count;
	THREAD_UNSAFE
	return _items_count;
}

template <class T>
quint64 jMemoryStorage<T>::storageSize() const
{
	return items_count;
}

template <class T>
jMemoryStorage<T> & jMemoryStorage<T>::setStorageBuffer(T * _items, quint64 _items_count, bool _deep_copy)
{
	stopProcessing();
	THREAD_SAFE(Write)
	if (deep_copy && items)
	{
		delete [] items;
	}
	deep_copy = _deep_copy;
	items_count = _items_count;
	if (deep_copy)
	{
		items = new T[items_count];
		::memcpy(items, _items, items_count * sizeof(T));
	}
	else
	{
		items = _items;
	}
	THREAD_UNSAFE
	return * this;
}

template <class T>
bool jMemoryStorage<T>::isDeepCopy() const
{
	return deep_copy;
}

// ------------------------------------------------------------------------

template <class T>
jFileStorage<T>::jFileStorage(): jStorage<T>()
{
	setOffset(0);
}

template <class T>
jFileStorage<T>::jFileStorage(const QString & _file_name, quint64 _offset): jStorage<T>()
{
	setOffset(_offset);
	setStorageFile(_file_name);
}

template <class T>
jFileStorage<T>::~jFileStorage()
{
	stopProcessing();
	if (file.isOpen())
	{
		file.close();
	}
}

template <class T>
jFileStorage<T> & jFileStorage<T>::setStorageFile(const QString & _file_name)
{
	stopProcessing();
	THREAD_SAFE(Write);
	if (file.isOpen())
	{
		file.close();
	}
	file.setFileName(_file_name);
	file.setPermissions(QFile::ReadOther);
	file.open(QFile::ReadOnly);
	THREAD_UNSAFE
	return * this;
}

template <class T>
quint64 jFileStorage<T>::storageSize() const
{
	THREAD_SAFE(Read)
	quint64 _size = (file.size() - offs)/ sizeof(T);
	THREAD_UNSAFE
	return _size;
}

template <class T>
quint64 jFileStorage<T>::readItems(T * & _items, quint64 _items_count)
{
	THREAD_SAFE(Write)
	file.seek((position() + offs) / sizeof(T));
	items = file.read(_items_count * sizeof(T));
	_items = reinterpret_cast<T *>(const_cast<char *>(items.constData()));
	THREAD_UNSAFE
	return SAFE_GET(items.count() / sizeof(T));
}

template <class T>
jFileStorage<T> & jFileStorage<T>::setOffset(quint64 _offset)
{
	stopProcessing();
	SAFE_SET(offs, _offset);
	return * this;
}

template <class T>
quint64 jFileStorage<T>::offset() const
{
	return offs;
}

// ------------------------------------------------------------------------

template <class T>
class jStorage<T>::jStorageThread : public QThread
{
	DECL_MUTEX
public:
	jStorageThread(jStorage<T> * _storage, QReadWriteLock * _rw_lock);
	~jStorageThread();
	QMap<int, QVector<T> > items(quint64 _lo, quint64 _hi) const;
	void stop();
	bool isProcessingFinished() const;
	QByteArray exportLayers() const;
	bool importLayers(const QByteArray & _saved_layers);
	quint64 itemsProcessed() const;
protected:
	void run();
private:
	jStorage<T> * storage;
	QReadWriteLock * storage_rw_lock;
	bool finished;
	bool stop_thread;
	quint64 items_processed;

	struct Layer
	{
		qreal seg_count;
		QVector<bool> processed;
		QVector<T> min, max, x;
	};
	QVector<Layer> layers;
	void splitStorage();
	void segmentState(int _layer_number, quint64 _segment_number, quint64 & _lo, quint64 & _hi, bool & _processed) const;
	void prevLayerRange(int _layer_number, quint64 _segment_number, quint64 & _lo_segment, quint64 & _hi_segment);
	bool adjustLayers();
	int selectLayer(quint64 _lo, quint64 _hi) const;
};

template <class T>
jStorage<T>::jStorageThread::jStorageThread(jStorage<T> * _storage, QReadWriteLock * _rw_lock) : QThread()
{
	storage = _storage;
	storage_rw_lock = _rw_lock;
	finished = true;
	stop_thread = false;
	items_processed = 0;
}

template <class T>
jStorage<T>::jStorageThread::~jStorageThread()
{
	stop();
}

template <class T>
void jStorage<T>::jStorageThread::run()
{
	splitStorage();
	QDateTime _time_stamp = QDateTime::currentDateTime();
	while ((!stop_thread) && (!finished))
	{
		adjustLayers();
		storage->storageControl()->emitLayersAdjusted();
		msleep(1);
	}
	quint64 _msecs = _time_stamp.msecsTo(QDateTime::currentDateTime());
	JDEBUG("thread finished" << _msecs);

	if (finished)
	{
		storage->storageControl()->emitFinished(_msecs);
	}
	if (stop_thread)
	{
		storage->storageControl()->emitStopped();
	}

	stop_thread = false;
}

template <class T>
void jStorage<T>::jStorageThread::stop()
{
	stop_thread = true;
	wait();
	stop_thread = false;
}

template <class T>
bool jStorage<T>::jStorageThread::isProcessingFinished() const
{
	return finished;
}

template <class T>
void jStorage<T>::jStorageThread::splitStorage()
{
	THREAD_SAFE(Write)
	finished = false;
	layers.clear();
	items_processed = 0;
	qreal _seg_count = (qreal)storage->storageSize() / storage->segmentSize();
	do 
	{
		Layer _layer;
		_layer.seg_count = _seg_count;
		_layer.processed.resize(::ceil(_seg_count));
		_layer.processed.fill(false, ::ceil(_seg_count));
		_layer.min.resize(::ceil(_seg_count));
		_layer.max.resize(::ceil(_seg_count));
		_layer.x.resize(::ceil(_seg_count));
		layers << _layer;
		_seg_count = _seg_count / 2;
	} 
	while (_seg_count > storage->processedItemsHint());
	THREAD_UNSAFE
}

template <class T>
void jStorage<T>::jStorageThread::segmentState(int _layer_number, quint64 _segment_number, quint64 & _lo, quint64 & _hi, bool & _processed) const
{
	if ((_layer_number < 0) || (_layer_number >= layers.count()))
	{
		_lo = 0;
		_hi = 0;
		return;
	}
	Layer _layer = layers[_layer_number];
	if (_segment_number >= ::ceil(_layer.seg_count))
	{
		_lo = 0;
		_hi = 0;
		return;
	}
	_processed = _layer.processed[_segment_number];
	_lo = storage->storageSize() * _segment_number / _layer.seg_count;
	_hi = storage->storageSize() * (_segment_number + 1) / _layer.seg_count;
}

template <class T>
void jStorage<T>::jStorageThread::prevLayerRange(int _layer_number, quint64 _segment_number, quint64 & _lo_segment, quint64 & _hi_segment)
{
	if ((_layer_number < 1) || (_layer_number >= layers.count()))
	{
		_lo_segment = 0;
		_hi_segment = 0;
		return;
	}
	Layer _layer = layers[_layer_number];
	if (_segment_number >= ::ceil(_layer.seg_count))
	{
		_lo_segment = 0;
		_hi_segment = 0;
		return;
	}
	Layer _prev_layer = layers[_layer_number - 1];
	_lo_segment = _prev_layer.seg_count * _segment_number / _layer.seg_count;
	_hi_segment = _prev_layer.seg_count * (_segment_number + 1) / _layer.seg_count;
	if (_hi_segment > _prev_layer.seg_count)
	{
		_hi_segment = _prev_layer.seg_count;
	}
}

template <class T>
bool jStorage<T>::jStorageThread::adjustLayers()
{
	THREAD_SAFE(Read)
	const int _layers_count = layers.count();
	if ((_layers_count == 0) || finished)
	{
		THREAD_UNSAFE
		return true;
	}
	Layer & _layer0 = layers[0];
	const quint64 _seg_count = ::ceil(_layer0.seg_count);
	jStorage<T>::less_func _less_func = storage->lessFunc();
	jStorage<T>::greater_func _greater_func = storage->greaterFunc();
	THREAD_UNSAFE
	if (_seg_count < storage->processedItemsHint())
	{
		finished = true;
		return true;
	}
	T * _items;
	bool _layer0_finished = true;
	quint64 _step = _layers_count * 2;
	const quint64 _seg_size0 = storage->segmentSize();
	for (quint64 _seg_idx = 0; _seg_idx < _seg_count; )
	{
		bool _processed;
		quint64 _lo, _hi;
		THREAD_SAFE(Read)
		segmentState(0, _seg_idx, _lo, _hi, _processed);
		THREAD_UNSAFE
		if (_processed)
		{
			_seg_idx++;
			continue;
		}
		storage->setPosition(_lo);
		const quint64 _items_count = storage->readItems(_items, _hi - _lo);
		if (_items_count && _items)
		{
			T _min = _items[0];
			T _max = _items[0];
			for (quint64 _items_idx = 1; _items_idx < _items_count; _items_idx++)
			{
				if (_less_func(_items[_items_idx], _min))
				{
					_min = _items[_items_idx];
				}
				if (_greater_func(_items[_items_idx], _max))
				{
					_max = _items[_items_idx];
				}
			}
			THREAD_SAFE(Write)
			items_processed += _items_count;
			const quint64 _seg_idx_sub_end = qMin<quint64>(_seg_count, _seg_idx + _step);
			for (quint64 _seg_idx_sub = _seg_idx; _seg_idx_sub < _seg_idx_sub_end; _seg_idx_sub++)
			{
				if (_layer0.processed[_seg_idx_sub] == false)
				{
					_layer0.min[_seg_idx_sub] = _min;
					_layer0.max[_seg_idx_sub] = _max;
					_layer0.x[_seg_idx_sub] = _seg_size0 * _seg_idx_sub;
				}
			}
			_layer0.processed[_seg_idx] = true;
			THREAD_UNSAFE
		}
		_seg_idx += _step;
		_layer0_finished = false;
		if (stop_thread)
		{
			return false;
		}
	}

	for (int _layer_idx = 1; _layer_idx < _layers_count; _layer_idx++)
	{
		quint64 _lo_segment, _hi_segment;
		THREAD_SAFE(Write)
		Layer & _layer = layers[_layer_idx];
		const Layer & _prev_layer = layers[_layer_idx - 1];
		const quint64 _seg_count = ::ceil(_layer.seg_count);
		const qreal _seg_size = storage->storageSize() / _layer.seg_count;
		for (quint64 _seg_idx = 0; _seg_idx < _seg_count; _seg_idx++)
		{
			prevLayerRange(_layer_idx, _seg_idx, _lo_segment, _hi_segment);
			T _min = _prev_layer.min[_lo_segment];
			T _max = _prev_layer.max[_lo_segment];
			for (quint64 _prev_seg_idx = _lo_segment; _prev_seg_idx < _hi_segment; _prev_seg_idx++)
			{
				if (_prev_layer.processed[_prev_seg_idx] == false)
				{
					break;
				}
				if (_less_func(_prev_layer.min[_prev_seg_idx], _min))
				{
					_min = _prev_layer.min[_prev_seg_idx];
				}
				if (_greater_func(_prev_layer.max[_prev_seg_idx], _max))
				{
					_max = _prev_layer.max[_prev_seg_idx];
				}					
			}
			_layer.min[_seg_idx] = _min;
			_layer.max[_seg_idx] = _max;
			_layer.x[_seg_idx] = _seg_idx * _seg_size;
			_layer.processed[_seg_idx] = true;
			if (stop_thread)
			{
				return false;
			}
		}
		THREAD_UNSAFE
	}
	SAFE_SET(finished, _layer0_finished);
	return finished;
}

template <class T>
int jStorage<T>::jStorageThread::selectLayer(quint64 _lo, quint64 _hi) const
{
	int _layer_idx = 0;
	for (; _layer_idx < layers.count(); _layer_idx++)
	{
		const quint64 _seg_size = storage->storageSize() / layers[_layer_idx].seg_count;
		const qreal _selected_seg_count = (qreal)(_hi - _lo) / _seg_size;
		if (_selected_seg_count < storage->processedItemsHint())
		{
			break;
		}
	}
	_layer_idx--;
	return _layer_idx;
}

template <class T>
QMap<int, QVector<T> > jStorage<T>::jStorageThread::items(quint64 _lo, quint64 _hi) const
{
	THREAD_SAFE(Read)
	jStorage<T>::less_func _less_func = storage->lessFunc();
	jStorage<T>::greater_func _greater_func = storage->greaterFunc();
	QMap<int, QVector<T> > _result;
	int _selected_layer = selectLayer(_lo, _hi);
	JDEBUG("selected layer" << _selected_layer);
	if (_selected_layer == -1)
	{
		storage->setPosition(_lo);
		T * _items;
		const quint64 _items_count = storage->readItems(_items, _hi - _lo);

		qreal _seg_size = (qreal) _items_count / storage->processedItemsHint();

		QVector<T> & _min = _result[jStorage<T>::Minimums];
		QVector<T> & _max = _result[jStorage<T>::Maximums];
		QVector<T> & _x = _result[jStorage<T>::X];

		_min.resize(storage->processedItemsHint());
		_max.resize(storage->processedItemsHint());
		_x.resize(storage->processedItemsHint());

		T * _min_data = const_cast<T *>(_min.constData());
		T * _max_data = const_cast<T *>(_max.constData());
		T * _x_data = const_cast<T *>(_x.constData());

		const quint64 _items_count_x = storage->processedItemsHint();
		for (quint64 _idx = 0; _idx < _items_count_x; _idx++)
		{
			_x_data[_idx] = _lo + (_idx * _seg_size);
		}

		for (qreal _idx = 0; _idx < _items_count; _idx += _seg_size)
		{
			quint64 _index = (quint64)(_idx / _seg_size);
			T & _min_item = _min_data[_index];
			T & _max_item = _max_data[_index];

			_min_item = _items[(quint64)_idx];
			_max_item = _items[(quint64)_idx];
		}
		for (quint64 _idx = 0; _idx < _items_count; _idx++)
		{
			quint64 _index = (quint64)(_idx / _seg_size);
			T & _min_item = _min_data[_index];
			if (_less_func(_items[_idx], _min_item))
			{
				_min_item = _items[_idx];
			}

			T & _max_item = _max_data[_index];
			if (_greater_func(_items[_idx], _max_item))
			{
				_max_item = _items[_idx];
			}
		}
	}
	else
	{
		qreal _seg_lo = _lo * layers[_selected_layer].seg_count / storage->storageSize();
		qreal _seg_hi = _hi * layers[_selected_layer].seg_count / storage->storageSize();
		if (_seg_hi - _seg_lo < 1.0f)
		{
			_seg_hi += 1.0f;
		}
		_result[jStorage<T>::Minimums] = layers[_selected_layer].min.mid(_seg_lo, _seg_hi - _seg_lo);
		_result[jStorage<T>::Maximums] = layers[_selected_layer].max.mid(_seg_lo, _seg_hi - _seg_lo);
		_result[jStorage<T>::X] = layers[_selected_layer].x.mid(_seg_lo, _seg_hi - _seg_lo);
	}
	THREAD_UNSAFE
	return _result;
}

template <class T>
QByteArray jStorage<T>::jStorageThread::exportLayers() const
{
	QByteArray _exported;
	QDataStream ds(&_exported, QIODevice::WriteOnly);
	THREAD_SAFE(Read)
	ds << storage->storageSize();
	ds << layers.count();
	foreach (const Layer & _layer, layers)
	{
		ds << _layer.seg_count;
		ds << _layer.processed;
		ds << _layer.min;
		ds << _layer.max;
		ds << _layer.x;
	}
	THREAD_UNSAFE
	return qCompress(_exported);
}

template <class T>
bool jStorage<T>::jStorageThread::importLayers(const QByteArray & _saved_layers)
{
	QDateTime _time_stamp = QDateTime::currentDateTime();
	stop();
	THREAD_SAFE(Write)
	QDataStream ds(qUncompress(_saved_layers));
	quint64 _storage_size;
	ds >> _storage_size;
	if (_storage_size != storage->storageSize())
	{
		THREAD_UNSAFE
		JDEBUG("import failed, wrong size");
		return false;
	}
	layers.clear();
	int _layers_count;
	ds >> _layers_count;
	layers.resize(_layers_count);
	for (int _idx = 0; _idx < _layers_count; _idx++)
	{
		Layer & _layer = layers[_idx];
		ds >> _layer.seg_count;
		ds >> _layer.processed;
		ds >> _layer.min;
		ds >> _layer.max;
		ds >> _layer.x;
	}
	finished = true;
	THREAD_UNSAFE
	quint64 _msecs = _time_stamp.msecsTo(QDateTime::currentDateTime());
	JDEBUG("import finished" << _msecs);
	storage->storageControl()->emitLayersAdjusted();
	storage->storageControl()->emitFinished(_msecs);
	return true;
}

template <class T>
quint64 jStorage<T>::jStorageThread::itemsProcessed() const
{
	return items_processed;
}

// ------------------------------------------------------------------------

#endif
