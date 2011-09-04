#ifndef __JCACHEDITEMS_H__
#define __JCACHEDITEMS_H__

#include "jitems.h"
#include "jstorage.h"

template <class T, class TX = T>
class jCachedItem1D : public jItem1D<T, TX>
{
	COPY_FBD(jCachedItem1D)
public:
	jCachedItem1D(jStorageInterface * _storage = 0, int _channel = 0, int _line_style = jItem1D<T, TX>::Lines, qreal _bar_width = 1.0);
	~jCachedItem1D();

	jCachedItem1D<T, TX> & setChannel(int _channel = 0);
	int channel() const;

	jCachedItem1D<T, TX> & setStorage(jStorageInterface * _storage);
	jStorage<T, TX> * storage() const;

	void updateViewport(const QRectF & _rect);
	QRectF boundingRect(const jAxis * _x_axis = 0, const jAxis * _y_axis = 0) const;

	void render(QPainter & _painter, const QRectF & _dst_rect, const QRectF & _src_rect, const jAxis * _x_axis = 0, const jAxis * _y_axis = 0);
	void renderPreview(QPainter & _painter, const QRectF & _dst_rect, const QRectF & _src_rect, const jAxis * _x_axis = 0, const jAxis * _y_axis = 0);
private:
	jStorage<T, TX> * strg;
	QVector<T> items;
	QVector<TX> x_data;
	int ch;
	jMutex local_mtx;
protected:
	jItem1D<T, TX> & setData(typename jItem1D<T, TX>::Flat *, unsigned int, bool)			{ return * this; }
	jItem1D<T, TX> & setData(typename jItem1D<T, TX>::Flat *, TX *, unsigned int, bool)		{ return * this; }
	jItem1D<T, TX> & setData(typename jItem1D<T, TX>::Point * , unsigned int, bool)			{ return * this; }
	jItem1D<T, TX> & setData(typename jItem1D<T, TX>::Radial *, unsigned int, bool)			{ return * this; }
};

// ------------------------------------------------------------------------

template <class T, class TX>
jCachedItem1D<T, TX>::jCachedItem1D(jStorageInterface * _storage, int _channel, int _line_style, qreal _bar_width) :
	jItem1D<T, TX>(_line_style, _bar_width), strg(dynamic_cast<jStorage<T, TX> *>(_storage))
{
	ch = _channel;
}

template <class T, class TX>
jCachedItem1D<T, TX>::~jCachedItem1D()
{

}

template <class T, class TX>
jCachedItem1D<T, TX> & jCachedItem1D<T, TX>::setChannel(int _channel)
{
	if (_channel < 0)
	{
		_channel = 0;
	}
	if (strg && (_channel >= strg->channels()))
	{
		_channel = strg->channels() - 1;
	}
	SAFE_SET(ch, _channel);
	return * this;
}

template <class T, class TX>
int jCachedItem1D<T, TX>::channel() const
{
	return ch;
}

template <class T, class TX>
jCachedItem1D<T, TX> & jCachedItem1D<T, TX>::setStorage(jStorageInterface * _storage)
{
	THREAD_SAFE(Write)
	strg = dynamic_cast<jStorage<T, TX> *>(_storage);
	THREAD_UNSAFE
	return * this;
}

template <class T, class TX>
void jCachedItem1D<T, TX>::updateViewport(const QRectF & _rect)
{
//	JTIME_START
	if (strg == 0)
	{
		return;
	}
	QPointF _origin = origin();
	THREAD_SAFE(Write)
	quint64 _lo = qMax<qreal>(_rect.left() + _origin.x(), 0);
	quint64 _hi = qMax<qreal>(_rect.right() + _origin.x(), 0);
	if ((_lo == _hi) || (_hi == 0))
	{
		items.clear();
		THREAD_UNSAFE;
		return;
	}

	QVector<TX> _vx;
	QMap<int, QVector<T> > _items = strg->processedItems(_lo, _hi, &_vx).at(ch);

//	JTIME_DIFF("getting items")

	QVector<T> & _min = _items[jStorageInterface::Minimums];
	QVector<T> & _max = _items[jStorageInterface::Maximums];
	x_data.resize(2 * qMin<int>(_min.count(), _max.count()));
	items.resize(x_data.count());
	for (int _idx = 0; _idx < items.count(); _idx += 2)
	{
		items[_idx] = _min[_idx / 2];
		items[_idx + 1] = _max[_idx / 2];
		x_data[_idx] = _vx[_idx / 2];
		x_data[_idx + 1] = _vx[_idx / 2];
	}

	THREAD_UNSAFE

//	JTIME_DIFF("reshape arrays")

	local_mtx.lockForWrite();
	jItem1D<T, TX>::setData(items.data(), x_data.data(), items.count());
	local_mtx.unlock();

//	JTIME_DIFF("setting data")
//	JTIME_ELAPSED("total")
}

template <class T, class TX>
jStorage<T, TX> * jCachedItem1D<T, TX>::storage() const
{
	return strg;
}


template <class T, class TX>
QRectF jCachedItem1D<T, TX>::boundingRect(const jAxis * _x_axis, const jAxis * _y_axis) const
{
	THREAD_SAFE(Read)

	const qreal _offset_x = origin().x();
	const qreal _offset_y = origin().y();
	TX _left, _right;
	T _top, _bottom;

	QVector<TX> _vx;
	QMap<int, QVector<T> > _items = strg->processedItems(0, 0, &_vx).at(ch);

	if (_vx.count())
	{
		_left = _vx[0];
		_right = _vx[0];
	}
	if (_items[jStorageInterface::Minimums].count())
	{
		_top = _items[jStorageInterface::Minimums][0];
	}
	if (_items[jStorageInterface::Maximums].count())
	{
		_bottom = _items[jStorageInterface::Maximums][0];
	}

	foreach (const TX & _x, _vx)
	{
		if (_x < _left)
		{
			_left = _x;
		}
		if (_x > _right)
		{
			_right = _x;
		}
	}
	foreach (const T & _item, _items[jStorageInterface::Minimums])
	{
		if (_item < _top)
		{
			_top = _item;
		}
	}
	foreach (const T & _item, _items[jStorageInterface::Maximums])
	{
		if (_item > _bottom)
		{
			_bottom = _item;
		}
	}

	if (_x_axis && _x_axis->isLog10ScaleEnabled())
	{
		_left = _x_axis->toLog10(_left);
		_right = _x_axis->toLog10(_right);
	}
	if (_y_axis && _y_axis->isLog10ScaleEnabled())
	{
		_top = _y_axis->toLog10(_top);
		_bottom = _y_axis->toLog10(_bottom);
	}
	_left += _offset_x;
	_right += _offset_x;
	_top += _offset_y;
	_bottom += _offset_y;
	if (_x_axis && _x_axis->isLog10ScaleEnabled())
	{
		if (_left - _offset_x == 0)
		{
			_left = _x_axis->lo();
		}
	}
	if (_y_axis && _y_axis->isLog10ScaleEnabled())
	{
		if (_bottom - _offset_y == 0)
		{
			_bottom = _y_axis->lo();
		}
	}
	if (_left > _right)
	{
		::qSwap(_left, _right);
	}
	if (_top > _bottom)
	{
		::qSwap(_top, _bottom);
	}
	THREAD_UNSAFE
	return (_top == _bottom) ? QRectF(QPointF(_left, _top > 0 ? _offset_y : _top), QPointF(_right, _top > 0 ? _top : _offset_y)) : QRectF(QPointF(_left, _top), QPointF(_right, _bottom));
}

template <class T, class TX>
void jCachedItem1D<T, TX>::render(QPainter & _painter, const QRectF & _dst_rect, const QRectF & _src_rect, const jAxis * _x_axis = 0, const jAxis * _y_axis = 0)
{
	local_mtx.lockForWrite();
	jItem1D<T, TX>::render(_painter, _dst_rect, _src_rect, _x_axis, _y_axis);
	local_mtx.unlock();
}

template <class T, class TX>
void jCachedItem1D<T, TX>::renderPreview(QPainter & _painter, const QRectF & _dst_rect, const QRectF & _src_rect, const jAxis * _x_axis = 0, const jAxis * _y_axis = 0)
{
	QVector<TX> _vx;
	QMap<int, QVector<T> > _items = strg->processedItems(0, 0, &_vx).at(ch);

	QVector<T> & _min = _items[jStorageInterface::Minimums];
	QVector<T> & _max = _items[jStorageInterface::Maximums];
	QVector<TX> _pv_x_data;
	_pv_x_data.resize(2 * qMin<int>(_min.count(), _max.count()));
	QVector<T> _pv_items;
	_pv_items.resize(_pv_x_data.count());
	for (int _idx = 0; _idx < _pv_items.count(); _idx += 2)
	{
		_pv_items[_idx] = _min[_idx / 2];
		_pv_items[_idx + 1] = _max[_idx / 2];
		_pv_x_data[_idx] = _vx[_idx / 2];
		_pv_x_data[_idx + 1] = _vx[_idx / 2];
	}

	local_mtx.lockForWrite();
	jItem1D<T, TX>::setData(_pv_items.data(), _pv_x_data.data(), _pv_items.count());
	jItem1D<T, TX>::render(_painter, _dst_rect, _src_rect, _x_axis, _y_axis); 
	// return back data
	jItem1D<T, TX>::setData(items.data(), x_data.data(), items.count());
	local_mtx.unlock();
}

#endif
