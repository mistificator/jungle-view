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
QVector<T> jStorage<T>::processedItems(quint64 _start_item, quint64 _end_item, int _type) const
{
	return thread->items(_start_item, _end_item ? _end_item : storageSize(), _type);
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
jStorageHandler * jStorage<T>::storageControl() const
{
	return handler;
}


// ------------------------------------------------------------------------

template <class T>
jMemoryStorage<T>::jMemoryStorage(): jStorage<T>()
{
}

template <class T>
jMemoryStorage<T>::~jMemoryStorage()
{
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
jMemoryStorage<T> & jMemoryStorage<T>::setStorageBuffer(T * _items, quint64 _items_count)
{
	THREAD_SAFE(Write)
	items = _items;
	items_count = _items_count;
	THREAD_UNSAFE
	return * this;
}

// ------------------------------------------------------------------------

template <class T>
class jStorage<T>::jStorageThread : public QThread
{
	DECL_MUTEX
public:
	jStorageThread(jStorage<T> * _storage, QReadWriteLock * _rw_lock);
	~jStorageThread();
	QVector<T> items(quint64 _lo, quint64 _hi, int _type) const;
	void stop();
	bool isProcessingFinished() const;
protected:
	void run();
private:
	jStorage<T> * storage;
	QReadWriteLock * storage_rw_lock;
	bool finished;
	bool stop_thread;

	struct Layer
	{
		qreal seg_count;
		QVector<bool> processed;
		QVector<T> min, max;
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
	JDEBUG("finished" << _msecs);
	storage->storageControl()->emitFinished(_msecs);

	stop_thread = false;
}

template <class T>
void jStorage<T>::jStorageThread::stop()
{
	stop_thread = true;
	wait();
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
	qreal _seg_count = (qreal)storage->storageSize() / storage->segmentSize();
	do 
	{
		Layer _layer;
		_layer.seg_count = _seg_count;
		_layer.processed.resize(::ceil(_seg_count));
		_layer.processed.fill(false, ::ceil(_seg_count));
		_layer.min.resize(::ceil(_seg_count));
		_layer.max.resize(::ceil(_seg_count));
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
	THREAD_UNSAFE
	if (_seg_count < storage->processedItemsHint())
	{
		finished = true;
		return true;
	}
	T * _items;
	bool _layer0_finished = true;
	quint64 _step = _layers_count * 2;
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
				if (_items[_items_idx] < _min)
				{
					_min = _items[_items_idx];
				}
				if (_items[_items_idx] > _max)
				{
					_max = _items[_items_idx];
				}
			}
			THREAD_SAFE(Write)
			_layer0.min[_seg_idx] = _min;
			_layer0.max[_seg_idx] = _max;
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
				if (_prev_layer.min[_prev_seg_idx] < _min)
				{
					_min = _prev_layer.min[_prev_seg_idx];
				}
				if (_prev_layer.max[_prev_seg_idx] > _max)
				{
					_max = _prev_layer.max[_prev_seg_idx];
				}					
			}
			_layer.min[_seg_idx] = _min;
			_layer.max[_seg_idx] = _max;

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
	qreal _selected_seg_count = (qreal)(_hi - _lo) / storage->segmentSize();
	for (int _layer_idx = layers.count() - 1; _layer_idx >= 0; _layer_idx--)
	{
		const quint64 _seg_size = storage->storageSize() / layers[_layer_idx].seg_count;
		if (_seg_size * _selected_seg_count > storage->processedItemsHint())
		{
			return _layer_idx;
		}
		_selected_seg_count = _selected_seg_count / 2;
	}
	return -1;
}

template <class T>
QVector<T> jStorage<T>::jStorageThread::items(quint64 _lo, quint64 _hi, int _type) const
{
	THREAD_SAFE(Read)
	QVector<T> _result;
	int _selected_layer = selectLayer(_lo, _hi);
	if (_selected_layer == -1)
	{
		storage->setPosition(_lo);
		T * _items;
		quint64 _items_count = storage->readItems(_items, _hi - _lo);
		_result.resize(_items_count);
		::memcpy((void *)_result.data(), _items, _items_count * sizeof(T));
	}
	else
	{
		qreal _seg_lo = _lo * layers[_selected_layer].seg_count / storage->storageSize();
		qreal _seg_hi = _hi * layers[_selected_layer].seg_count / storage->storageSize();
		switch (_type)
		{
		case jStorage<T>::MinOnly:
			_result = layers[_selected_layer].min.mid(_seg_lo, _seg_hi - _seg_lo);
			break;
		case jStorage<T>::MaxOnly:
			_result = layers[_selected_layer].max.mid(_seg_lo, _seg_hi - _seg_lo);
			break;
		case jStorage<T>::MaxThenMin:
			_result << layers[_selected_layer].max.mid(_seg_lo, _seg_hi - _seg_lo);
			_result << layers[_selected_layer].min.mid(_seg_lo, _seg_hi - _seg_lo);
			break;
		case jStorage<T>::MinThenMax:
			_result << layers[_selected_layer].min.mid(_seg_lo, _seg_hi - _seg_lo);
			_result << layers[_selected_layer].max.mid(_seg_lo, _seg_hi - _seg_lo);
			break;
		case jStorage<T>::MinMaxInterleaved:
			_result.resize((::ceil(_seg_hi) - _seg_lo) * 2);
			quint64 _result_idx = 0;
			for (quint64 _seg_index = _seg_lo; _seg_index < _seg_hi; _seg_index++, _result_idx += 2)
			{
				_result[_result_idx] = layers[_selected_layer].min[_seg_index];
				_result[_result_idx + 1] = layers[_selected_layer].max[_seg_index];
			}
			break;
		}

	}
	THREAD_UNSAFE
	return _result;
}

// ------------------------------------------------------------------------

#endif
