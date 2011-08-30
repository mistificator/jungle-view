#ifndef __JCACHEDITEMS_H__
#define __JCACHEDITEMS_H__

#include "jitems.h"
#include "jstorage.h"

template <class T>
class jCachedItem1D : public jItem
{
	COPY_FBD(jCachedItem1D)
public:
	jCachedItem1D(jStorageInterface * _storage = 0, int _line_style = jItem1D<T>::Lines, qreal _bar_width = 1.0);
	~jCachedItem1D();

	jCachedItem1D<T> & setChannel(int _channel = 0);
	int channel() const;

	jItem & setData(void * _data, unsigned int _width, unsigned int _height = 1, bool _deep_copy = false);
	jCachedItem1D<T> & setStorage(jStorageInterface * _storage);
	jStorage<T> * storage() const;

	const void * data() const;
	QSize size() const;

	void updateViewport(const QRectF & _rect);
	void render(QPainter & _painter, const QRectF & _dst_rect, const QRectF & _src_rect, const jAxis * _x_axis = 0, const jAxis * _y_axis = 0);

	int dataModel() const;

	jCachedItem1D<T> & setLineStyle(int _style);
	int lineStyle() const;

	jCachedItem1D<T> & setBarWidth(qreal _width);
	qreal barWidth() const;

	bool intersects(const QRectF & _rect, const jAxis * _x_axis = 0, const jAxis * _y_axis = 0) const;
	QRectF boundingRect(const jAxis * _x_axis = 0, const jAxis * _y_axis = 0) const;

private:
	jItem1D<T> * item1d;
	jStorage<T> * strg;
	bool internal_strg;
	QVector<T> items;
	int width;
	int ch;
};

// ------------------------------------------------------------------------

template <class T>
jCachedItem1D<T>::jCachedItem1D(jStorageInterface * _storage, int _line_style, qreal _bar_width) :
	jItem(), item1d(new jItem1D<T>(_line_style, _bar_width)), strg(dynamic_cast<jStorage<T> *>(_storage))
{
	internal_strg = _storage ? false : true;
	width = 0;
	ch = 0;
}

template <class T>
jCachedItem1D<T>::~jCachedItem1D()
{
	if (internal_strg && strg)
	{
		delete strg;
	}
	delete item1d;
}

template <class T>
jCachedItem1D<T> & jCachedItem1D<T>::setChannel(int _channel)
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

template <class T>
int jCachedItem1D<T>::channel() const
{
	return ch;
}

template <class T>
jItem & jCachedItem1D<T>::setData(void * _data, unsigned int _width, unsigned int, bool _deep_copy = false)
{
	THREAD_SAFE(Write)
	if (internal_strg && strg)
	{
		delete strg;
	}
	strg = new jMemoryStorage<T>(reinterpret_cast<T *>(_data), _width, _deep_copy);
	internal_strg = true;
	THREAD_UNSAFE
	return * this;
}

template <class T>
jCachedItem1D<T> & jCachedItem1D<T>::setStorage(jStorageInterface * _storage)
{
	THREAD_SAFE(Write)
	if (internal_strg && strg)
	{
		delete strg;
	}
	strg = dynamic_cast<jStorage<T> *>(_storage);
	internal_strg = false;
	THREAD_UNSAFE
	return * this;
}


template <class T>
const void * jCachedItem1D<T>::data() const
{
	return SAFE_GET(items.constData());
}

template <class T>
QSize jCachedItem1D<T>::size() const
{
	if (strg == 0)
	{
		return QSize();
	}
	THREAD_SAFE(Read)                
	QSize _size(width, 1);
	THREAD_UNSAFE
	return _size;
}

template <class T>
void jCachedItem1D<T>::updateViewport(const QRectF & _rect)
{
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
		width = 0;
		THREAD_UNSAFE;
		return;
	}
	QMap<int, QVector<T> > _items = strg->processedItems(_lo, _hi).at(ch);
	const QVector<T> & _x = _items[jStorageInterface::X];
	const QVector<T> & _min = _items[jStorageInterface::Minimums];
	const QVector<T> & _max = _items[jStorageInterface::Maximums];
	const int _items_count = qMin<int>(_min.count(), _max.count());
	width = _items_count * 2;
	items.resize(_items_count * 4);
	for (int _idx = 0; _idx < _items_count; _idx++)
	{
		items[_idx * 4] = _x[_idx];
		items[_idx * 4 + 1] = _min[_idx];
		items[_idx * 4 + 2] = _x[_idx];
		items[_idx * 4 + 3] = _max[_idx];
	}
	THREAD_UNSAFE
	item1d->setData((jItem1D<T>::Point *)items.constData(), width);
}

template <class T>
jStorage<T> * jCachedItem1D<T>::storage() const
{
	return strg;
}


template <class T>
QRectF jCachedItem1D<T>::boundingRect(const jAxis * _x_axis, const jAxis * _y_axis) const
{
	THREAD_SAFE(Read)
	const unsigned int _width = size().width();
	if (_width == 0)
	{
		THREAD_UNSAFE
		return QRectF();
	}
	const qreal _offset_x = origin().x();
	const qreal _offset_y = origin().y();
	qreal _left, _right, _top, _bottom;
	_left = 1e+37; _right = -1e+37;
	_top = 1e+37; _bottom = -1e+37;

	QMap<int, QVector<T> > _items = strg->processedItems().at(ch);

	foreach (const T & _item, _items[jStorageInterface::X])
	{
		if (_item < _left)
		{
			_left = _item;
		}
		if (_item > _right)
		{
			_right = _item;
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

template <class T>
void jCachedItem1D<T>::render(QPainter & _painter, const QRectF & _dst_rect, const QRectF & _src_rect, const jAxis * _x_axis = 0, const jAxis * _y_axis = 0)
{
	if (isVisible() == false)
	{
		return;
	}
	item1d->render(_painter, _dst_rect, _src_rect, _x_axis, _y_axis);
}

template <class T>
int jCachedItem1D<T>::dataModel() const
{
	return item1d->dataModel();
}

template <class T>
jCachedItem1D<T> & jCachedItem1D<T>::setLineStyle(int _style)
{
	return item1d->setLineStyle(_style);
}

template <class T>
int jCachedItem1D<T>::lineStyle() const
{
	return item1d->lineStyle();
}

template <class T>
jCachedItem1D<T> & jCachedItem1D<T>::setBarWidth(qreal _width)
{
	return item1d->setBarWidth(_width);
}

template <class T>
qreal jCachedItem1D<T>::barWidth() const
{
	return item1d->barWidth();
}

template <class T>
bool jCachedItem1D<T>::intersects(const QRectF & _rect, const jAxis * _x_axis = 0, const jAxis * _y_axis = 0) const
{
	return item1d->intersects(_rect, _x_axis, _y_axis);
}

#endif
