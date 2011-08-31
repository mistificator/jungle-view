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
	thread = new jStorageThread(this, & rw_lock);
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
			_array[_idx][_type] = QByteArray::fromRawData(reinterpret_cast<char *>(_items[_idx][_type].data()), _items[_idx][_type].count() * sizeof(T));
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
//	SAFE_SET(seek_pos, (_item_position < storageSize()) ? _item_position : storageSize());
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
			quint64 _ch_idx = 0;
			for (int _idx = _channel; _idx < _items.count(); _idx += _channels, _ch_idx++)
			{
				_channel_result[_ch_idx] = _items[_idx];
			}
		}
	}
	return _result;
}

template <class T, class TX>
jStorageInterface & jStorage<T, TX>::setSegmentSize(quint64 _size)
{
	SAFE_SET(seg_size, _size);
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
	SAFE_SET(seg_func, _segment_func);
	return * this;
}

template <class T, class TX>
typename jStorage<T, TX>::segment_func jStorage<T, TX>::segmentFunc() const
{
	return (segment_func)seg_func;
}

template <class T, class TX>
bool jStorage<T, TX>::defaultLess(const T & _op1, const T & _op2)
{
	return _op1 < _op2;
}

template <class T, class TX>
jStorage<T, TX> & jStorage<T, TX>::setLessFunc(less_func _less_func = & defaultLess)
{
	SAFE_SET(l_func, _less_func);
	return * this;
}

template <class T, class TX>
typename jStorage<T, TX>::less_func jStorage<T, TX>::lessFunc() const
{
	return l_func;
}

template <class T, class TX>
bool jStorage<T, TX>::defaultGreater(const T & _op1, const T & _op2)
{
	return _op1 > _op2;
}

template <class T, class TX>
jStorage<T, TX> & jStorage<T, TX>::setGreaterFunc(greater_func _greater_func = & defaultGreater)
{
	SAFE_SET(g_func, _greater_func);
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
	SAFE_SET(hint, _count ? _count : 1);
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
	SAFE_SET(ch_count, _count);
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
	THREAD_SAFE(Read)
	T * _items = items + (position() / channels()) * channels();
	if (_items_count + position() > items_count)
	{
		if (position() > items_count)
		{
			THREAD_UNSAFE
			return QVector<T>();
		}
		_items_count = items_count - position();
	}
	QVector<T> _result;
	_result.resize(_items_count);
	::memcpy(_result.data(), _items, _items_count * sizeof(T));
	THREAD_UNSAFE
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

template <class T, class TX>
bool jMemoryStorage<T, TX>::isDeepCopy() const
{
	return deep_copy;
}

// ------------------------------------------------------------------------

template <class T, class TX>
jFileStorage<T, TX>::jFileStorage(): jStorage<T, TX>()
{
	setOffset(0);
}

template <class T, class TX>
jFileStorage<T, TX>::jFileStorage(const QString & _file_name, quint64 _offset): jStorage<T, TX>()
{
	setOffset(_offset);
	setStorageFile(_file_name);
}

template <class T, class TX>
jFileStorage<T, TX>::~jFileStorage()
{
	stopProcessing();
	if (file.isOpen())
	{
		file.close();
	}
}

template <class T, class TX>
jFileStorage<T, TX> & jFileStorage<T, TX>::setStorageFile(const QString & _file_name)
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

template <class T, class TX>
quint64 jFileStorage<T, TX>::storageSize() const
{
	THREAD_SAFE(Read)
	quint64 _size = (file.size() - offs) / sizeof(T);
	THREAD_UNSAFE
	return _size;
}

template <class T, class TX>
QVector<T> jFileStorage<T, TX>::readItems(quint64 _items_count)
{
	THREAD_SAFE(Write)
	file.seek(offs + position() * sizeof(T) * channels());
	items = file.read(_items_count * sizeof(T));
	items.resize(_items_count * sizeof(T));
	QVector<T> _result;
	_result.resize(items.count() / sizeof(T));
	::memcpy(_result.data(), items.data(), items.count());
	THREAD_UNSAFE
	return _result;
}

template <class T, class TX>
jFileStorage<T, TX> & jFileStorage<T, TX>::setOffset(quint64 _offset)
{
	stopProcessing();
	SAFE_SET(offs, _offset);
	return * this;
}

template <class T, class TX>
quint64 jFileStorage<T, TX>::offset() const
{
	return offs;
}

// ------------------------------------------------------------------------

template <class T, class TX>
class jStorage<T, TX>::jStorageThread : public QThread
{
	DECL_MUTEX
public:
	jStorageThread(jStorage<T, TX> * _storage, QReadWriteLock * _rw_lock);
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
	QReadWriteLock * storage_rw_lock;
	bool finished;
	bool stop_thread;
	quint64 items_processed;

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
jStorage<T, TX>::jStorageThread::jStorageThread(jStorage<T, TX> * _storage, QReadWriteLock * _rw_lock) : QThread()
{
	storage = _storage;
	storage_rw_lock = _rw_lock;
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
	THREAD_SAFE(Write)
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
	THREAD_UNSAFE
}

template <class T, class TX>
void jStorage<T, TX>::jStorageThread::segmentState(int _layer_number, quint64 _segment_number, quint64 & _lo_item, quint64 & _hi_item, bool & _processed) const
{
	if ((_layer_number < 0) || (layers.count() == 0) || (_layer_number >= layers[0].count()))
	{
		_lo_item = 0;
		_hi_item = 0;
		return;
	}
	Layer _layer = layers[0][_layer_number];
	if (_segment_number >= ::ceil(_layer.seg_count))
	{
		_lo_item = 0;
		_hi_item = 0;
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
	THREAD_SAFE(Read)
	if ((layers.count() == 0) || finished)
	{
		THREAD_UNSAFE
		return true;
	}
	const int _layers_count = layers[0].count();
	if (_layers_count == 0)
	{
		THREAD_UNSAFE
		return true;
	}
	jStorage<T, TX>::less_func _less_func = storage->lessFunc();
	jStorage<T, TX>::greater_func _greater_func = storage->greaterFunc();
	const quint64 _seg_count = (layers[0][0].seg_count);
	THREAD_UNSAFE
	if (_seg_count < storage->processedItemsHint())
	{
		finished = true;
		return true;
	}
	bool _layer0_finished = true;
	quint64 _step = _layers_count * 2;
	const quint64 _seg_size0 = storage->segmentSize();
	const int _channels = storage->channels();

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
		if (_hi < _lo)
		{
			_hi = _lo;
		}
		THREAD_SAFE(Read)
		QVector< QVector<T> > _items = preprocessedReadItems(_lo, _hi);
		THREAD_UNSAFE
		for (int _channel = 0; _channel < _items.count(); _channel++)
		{
			QVector<T> & _items_data = _items[_channel];
			const quint64 _items_count = _items[_channel].count();
			if (_items_count)
			{
				T _min = _items_data[0];
				T _max = _items_data[0];
				for (quint64 _items_idx = 1; _items_idx < _items_count; _items_idx++)
				{
					if (_less_func(_items_data[_items_idx], _min))
					{
						_min = _items_data[_items_idx];
					}
					if (_greater_func(_items_data[_items_idx], _max))
					{
						_max = _items_data[_items_idx];
					}
				}
				THREAD_SAFE(Write)
				items_processed += _items_count;
				const quint64 _seg_idx_sub_end = qMin<quint64>(_seg_count, _seg_idx + _step);
				Layer & _layer0 = layers[_channel][0];
				for (quint64 _seg_idx_sub = _seg_idx; _seg_idx_sub < _seg_idx_sub_end; _seg_idx_sub++)
				{
					if (_layer0.processed[_seg_idx_sub] == false)
					{
						_layer0.min[_seg_idx_sub] = _min;
						_layer0.max[_seg_idx_sub] = _max;
						_layer0.x[_seg_idx_sub] = (qreal)(_seg_size0 * _seg_idx_sub) / _channels;
					}
				}
				_layer0.processed[_seg_idx] = true;
				THREAD_UNSAFE
			}
		}
		_seg_idx += _step;
		_layer0_finished = false;
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
			THREAD_SAFE(Write)
			Layer & _layer = layers[_channel][_layer_idx];
			const Layer & _prev_layer = layers[_channel][_layer_idx - 1];
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
				_layer.x[_seg_idx] = (qreal)(_seg_idx * _seg_size) / _channels;
				_layer.processed[_seg_idx] = true;
				if (stop_thread)
				{
					return false;
				}
			}
			THREAD_UNSAFE
		}

	}

	SAFE_SET(finished, _layer0_finished);
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
	THREAD_SAFE(Read)
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

			QVector<T> & _items_data = _items[_channel];

			if (_x)
			{
				_x->resize(_arr_sz);
				const quint64 _items_count_x = _x->count();
				for (quint64 _idx = 0; _idx < _items_count_x; _idx++)
				{
					(* _x)[_idx] = (qreal)(_lo_item + (_idx * _seg_size)) / _channels;
				}
			}

			for (qreal _idx = 0; _idx < _items_count; _idx += _seg_size)
			{
				const quint64 _index = (quint64)(_idx / _seg_size);
				T & _min_item = _min[_index];
				T & _max_item = _max[_index];

				_min_item = _items_data[(quint64)_idx];
				_max_item = _items_data[(quint64)_idx];
			}
			for (quint64 _idx = 0; _idx < _items_count; _idx++)
			{
				const quint64 _index = (quint64)(_idx / _seg_size);
				T & _min_item = _min[_index];
				if (_less_func(_items_data[_idx], _min_item))
				{
					_min_item = _items_data[_idx];
				}

				T & _max_item = _max[_index];
				if (_greater_func(_items_data[_idx], _max_item))
				{
					_max_item = _items_data[_idx];
				}
			}

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

	}
	THREAD_UNSAFE
	return _result;
}

template <class T, class TX>
QByteArray jStorage<T, TX>::jStorageThread::exportLayers() const
{
	QByteArray _exported;
	QDataStream ds(&_exported, QIODevice::WriteOnly);
	THREAD_SAFE(Read)
	ds << storage->storageSize();
	ds << layers.count();
	for (int _channel = 0; _channel < storage->channels(); _channel++)
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
	THREAD_UNSAFE
	return qCompress(_exported);
}

template <class T, class TX>
bool jStorage<T, TX>::jStorageThread::importLayers(const QByteArray & _saved_layers)
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
	quint64 _channels;
	ds >> _channels;
	if (_channels != storage->channels())
	{
		THREAD_UNSAFE
		JDEBUG("import failed, wrong channels number");
		return false;
	}
	layers.clear();
	layers.resize(_channels);
	for (int _channel = 0; _channel < _channels; _channel++)
	{
		int _layers_count;
		ds >> _layers_count;
		layers[_channel].resize(_layers_count);
		for (int _idx = 0; _idx < _layers_count; _idx++)
		{
			Layer & _layer = layers[_channel][_idx];
			ds >> _layer.seg_count;
			ds >> _layer.processed;
			ds >> _layer.min;
			ds >> _layer.max;
			ds >> _layer.x;
		}
	}
	finished = true;
	THREAD_UNSAFE
	quint64 _msecs = _time_stamp.msecsTo(QDateTime::currentDateTime());
	JDEBUG("import finished" << _msecs);
	storage->storageControl()->emitLayersAdjusted();
	storage->storageControl()->emitFinished(_msecs);
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
