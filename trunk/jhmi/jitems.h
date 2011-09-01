#ifndef _JITEMS_H_
#define _JITEMS_H_

#include "jview.h"
#ifdef __GNUC__
	#include <typeinfo>
#endif

// ------------------------------------------------------------------------

template <class T>
class jStack
{
public:
	jStack();
	jStack(const jStack & _other);
	const jStack<T> & operator = (const jStack & _other);
	~jStack();
	void push(const T & _value);
	T pop(const T & _default);
	T top(const T & _default);
private:
	QStack<T> stack;
};

template <class T, class TX = T>
class jItem1D: public jItem
{
	COPY_FBD(jItem1D)
public:
	enum {Lines = 1, Ticks = 2, Bars = 3, Dots = 4};
	enum {FlatData = 1, PointData = 2, RadialData = 3};
	struct Point
	{
		T x;
		TX y;
	};
	struct Radial
	{
		T v;
		TX t;
	};
	typedef T Flat;

	jItem1D(int _line_style = Lines, qreal _bar_width = 1.0);
	~jItem1D();

	void render(QPainter & _painter, const QRectF & _dst_rect, const QRectF & _src_rect, const jAxis * _x_axis = 0, const jAxis * _y_axis = 0);

	int dataModel() const;

	jItem1D<T, TX> & setLineStyle(int _style);
	int lineStyle() const;

	jItem1D<T, TX> & setBarWidth(qreal _width);
	qreal barWidth() const;

	bool intersects(const QRectF & _rect, const jAxis * _x_axis = 0, const jAxis * _y_axis = 0) const;
	QRectF boundingRect(const jAxis * _x_axis = 0, const jAxis * _y_axis = 0) const;

	virtual jItem1D<T, TX> & setData(Flat * _data, unsigned int _width, bool _deep_copy = false);
	virtual jItem1D<T, TX> & setData(Flat * _data, TX * _x, unsigned int _width, bool _deep_copy = false);
	virtual jItem1D<T, TX> & setData(Point * _data, unsigned int _width, bool _deep_copy = false);
	virtual jItem1D<T, TX> & setData(Radial * _data, unsigned int _width, bool _deep_copy = false);

	const TX * x() const;
private:
	int data_model, line_style;
	qreal bar_width;
	TX * x_data;
protected:
	virtual jItem1D<T, TX> & setDataModel(int _model);
};

template <class T>
class jItem2D: public jItem
{
	COPY_FBD(jItem2D)
public:
	jItem2D(int _conversion_flags = Qt::AutoColor | Qt::ThresholdDither, int _format = QImage::Format_RGB32);
	~jItem2D();

	void render(QPainter & _painter, const QRectF & _dst_rect, const QRectF & _src_rect, const jAxis * _x_axis = 0, const jAxis * _y_axis = 0);

	jItem2D<T> & setImageConversionFlags(int _conversion);
	int imageConversionFlags() const;

	jItem2D<T> & setImageFormat(int _format);
	int imageFormat() const;

	typedef qint32 (*converter_func)(unsigned int /* index */, const T & /* value */, jItem2D<T> *);
	jItem2D<T> & setConverter(converter_func _convert_func);
	converter_func converterFunc() const;

	bool intersects(const QRectF & _rect, const jAxis * _x_axis = 0, const jAxis * _y_axis = 0) const;

	jItem2D<T> & setData(T * _data, unsigned int _width, unsigned int _height, bool _deep_copy = false);
private:
	qint32 * convertToI32(void * _data_ptr, const QSize & _size);
	int conversion_flags, format;
	converter_func converter;
};

// ------------------------------------------------------------------------

template <class T>
jStack<T>::jStack()
{

}

template <class T>
jStack<T>::jStack(const jStack & _other)
{
	stack = _other.stack;
}

template <class T>
const jStack<T> & jStack<T>::operator = (const jStack & _other)
{
	stack = _other.stack;
	return * this;
}

template <class T>
jStack<T>::~jStack()
{

}

template <class T>
void jStack<T>::push(const T & _value)
{
	stack.push(_value);
}

template <class T>
T jStack<T>::pop(const T & _default)
{
	return (stack.isEmpty()) ? _default : stack.pop();
}

template <class T>
T jStack<T>::top(const T & _default)
{
	return (stack.isEmpty()) ? _default : stack.top();
}

// ------------------------------------------------------------------------

template <class T, class TX>
jItem1D<T, TX>::jItem1D(int _line_style, qreal _bar_width): jItem()
{
	x_data = 0;
	this->
		setDataModel(FlatData).
		setLineStyle(_line_style).
		setBarWidth(_bar_width);
}

template <class T, class TX>
jItem1D<T, TX>::~jItem1D()
{
	if (x_data && isDeepCopy())
	{
		delete [] x_data;
	}
}

template <class T, class TX>
jItem1D<T, TX> & jItem1D<T, TX>::setLineStyle(int _style)
{
	SAFE_SET(line_style, _style);
	return * this;
}

template <class T, class TX>
int jItem1D<T, TX>::lineStyle() const
{
	return line_style;
}

template <class T, class TX>
jItem1D<T, TX> & jItem1D<T, TX>::setBarWidth(qreal _width)
{
	SAFE_SET(bar_width, _width);
	return * this;
}

template <class T, class TX>
qreal jItem1D<T, TX>::barWidth() const
{
	return bar_width;
}

template <class T, class TX>
jItem1D<T, TX> & jItem1D<T, TX>::setDataModel(int _model)
{
	SAFE_SET(data_model, _model);
	switch (data_model)
	{
	case FlatData:
		{
			setBytesPerItem(sizeof(jItem1D<T>::Flat));
			break;
		}
	case PointData:
		{
			setBytesPerItem(sizeof(jItem1D<T>::Point));
			break;
		}
	case RadialData:
		{
			setBytesPerItem(sizeof(jItem1D<T>::Radial));
			break;
		}
	default:
		{
			SAFE_SET(data_model, (int)FlatData);
			setBytesPerItem(sizeof(jItem1D<T>::Flat));
			break;
		}
	}
	return * this;
}

template <class T, class TX>
int jItem1D<T, TX>::dataModel() const
{
	return data_model;
}

static bool radialSort(const QPointF & _p1, const QPointF & _p2)
{
	return (_p1.y() < _p2.y());
}

template <class T, class TX>
void jItem1D<T, TX>::render(QPainter & _painter, const QRectF & _dst_rect, const QRectF & _src_rect, const jAxis * _x_axis, const jAxis * _y_axis)
{
	THREAD_SAFE(Read)
	if (isVisible() == false)
	{
		THREAD_UNSAFE
		return;
	}
	if (data() == 0)
	{
		THREAD_UNSAFE
		return;
	}
	QPolygonF _points;
	QVector<QRectF> _rects;
	const QPointF _origin = origin();
	const qreal _offset_x = _origin.x();
	const qreal _offset_y = _src_rect.top() + _src_rect.bottom() - _origin.y();
	const QSize _size = size();
	const unsigned int _width = _size.width();
	switch (data_model)
	{
	case FlatData:
		{
			const Flat * const _y_data = (const Flat * const)data();
			if (x_data == 0)
			{
				qreal _fleft =
					(_x_axis && _x_axis->isLog10ScaleEnabled()) ?
					_x_axis->fromLog10(_src_rect.left() - _origin.x()) :
					_src_rect.left() - _origin.x();
				qreal _fright =
					(_x_axis && _x_axis->isLog10ScaleEnabled()) ?
					_x_axis->fromLog10(_src_rect.right() - _origin.x()) :
					_src_rect.right() - _origin.x();

				if (_fright > _width)
				{
					_fright = _width;
				}
				const qint32 _left = ((qint32)_fleft) < 0 ? 0 : _fleft;
				const qint32 _right = ((qint32)_fright) < 0 ? 0 : _fright;
				const qreal _bar_width =
					(_x_axis && _x_axis->isLog10ScaleEnabled()) ?
					_x_axis->fromLog10(bar_width) :
					bar_width;
				switch (line_style)
				{
				case Dots:
				case Lines:
					{
						_points.reserve(_right - _left);
						for (int _x = _left; _x < _right; _x++)
						{
							_points << QPointF(_x, -_y_data[_x]);
						}
						break;
					}
				case Ticks:
					{
						_points.reserve((_right - _left) * 2);
						for (int _x = _left; _x < _right; _x++)
						{
							_points << QPointF(_x, -_y_data[_x]);
							_points << QPointF( _x, 0);
						}
						break;
					}
				case Bars:
					{
						_rects.reserve(_right - _left);
						for (int _x = _left; _x < _right; _x++)
						{
							_rects << QRectF(QPointF(_x - (_bar_width / 2.0), - _y_data[_x]), QSizeF(_bar_width, _y_data[_x]));
						}
					}
				}
			}
			else
			{
				const qreal _bar_width =
					(_x_axis && _x_axis->isLog10ScaleEnabled()) ?
					_x_axis->fromLog10(bar_width) :
					bar_width;
				switch (line_style)
				{
				case Dots:
				case Lines:
					{
						_points.reserve(_width);
						for (unsigned int _idx = 0; _idx < _width; _idx++)
						{
							_points << QPointF(x_data[_idx], -_y_data[_idx]);
						}
						break;
					}
				case Ticks:
					{
						_points.reserve(_width * 2);
						for (unsigned int _idx = 0; _idx < _width; _idx++)
						{
							_points << QPointF(x_data[_idx], -_y_data[_idx]);
							_points << QPointF(x_data[_idx], 0);
						}
						break;
					}
				case Bars:
					{
						_rects.reserve(_width);
						for (unsigned int _idx = 0; _idx < _width; _idx++)
						{
							_rects << QRectF(QPointF(x_data[_idx] - (_bar_width / 2.0), -_y_data[_idx]), QSizeF(_bar_width, _y_data[_idx]));
						}
						break;
					}
				}

			}
			break;
		}
	case PointData:
		{
			const Point * const _data = (const Point * const)data();
			const qreal _bar_width =
				(_x_axis && _x_axis->isLog10ScaleEnabled()) ?
				_x_axis->fromLog10(bar_width) :
				bar_width;
			switch (line_style)
			{
			case Dots:
			case Lines:
				{
					_points.reserve(_width);
					for (unsigned int _idx = 0; _idx < _width; _idx++)
					{
						_points << QPointF(_data[_idx].x, -_data[_idx].y);
					}
					break;
				}
			case Ticks:
				{
					_points.reserve(_width * 2);
					for (unsigned int _idx = 0; _idx < _width; _idx++)
					{
						_points << QPointF(_data[_idx].x, -_data[_idx].y);
						_points << QPointF(_data[_idx].x, 0);
					}
					break;
				}
			case Bars:
				{
					_rects.reserve(_width);
					for (unsigned int _idx = 0; _idx < _width; _idx++)
					{
						_rects << QRectF(QPointF(_data[_idx].x - (_bar_width / 2.0), -_data[_idx].y), QSizeF(_bar_width, _data[_idx].y));
					}
					break;
				}
			}
			break;
		}
	case RadialData:
		{
			const Radial * const _data = (const Radial * const)data();
			switch (line_style)
			{
			case Dots:
				{
					_points.reserve(_width);
					for (unsigned int _idx = 0; _idx < _width; _idx++)
					{
						_points << QPointF(_data[_idx].v * ::cosf(_data[_idx].t), -_data[_idx].v * ::sinf(_data[_idx].t));
					}
					break;
				}
			case Lines:
				{
					_points.reserve(_width + 1);
					for (unsigned int _idx = 0; _idx < _width; _idx++)
					{
						_points << QPointF(_data[_idx].v, _data[_idx].t);
					}
					::qSort(_points.begin(), _points.end(), &radialSort);
					_points << _points[0];
					for (unsigned int _idx = 0; _idx < _width + 1; _idx++)
					{
						const qreal & _v = _points[_idx].x();
						const qreal & _t = _points[_idx].y();
						_points[_idx] = QPointF(_v * ::cosf(_t), -_v * ::sinf(_t));
					}
					break;
				}
			case Ticks:
				{
					_points.reserve(_width * 2);
					for (unsigned int _idx = 0; _idx < _width; _idx++)
					{
						_points << QPointF(_data[_idx].v * ::cosf(_data[_idx].t), -_data[_idx].v * ::sinf(_data[_idx].t));
						_points << QPointF(0, 0);
					}
					break;
				}
			}
			break;
		}
	}

	if (_x_axis && _x_axis->isLog10ScaleEnabled())
	{
		const qreal _minimum = _src_rect.left();
		for (int _idx = 0; _idx < _points.count(); _idx++)
		{
			_points[_idx].setX(_x_axis->toLog10(_points[_idx].x(), _minimum));
		}
	}
	if (_y_axis && _y_axis->isLog10ScaleEnabled())
	{
		const qreal _minimum = -_src_rect.bottom();
		for (int _idx = 0; _idx < _points.count(); _idx++)
		{
			_points[_idx].setY(-_y_axis->toLog10(-_points[_idx].y(), _minimum));
		}
	}
	THREAD_UNSAFE

	QTransform _transform;
	if (::jQuadToQuad(QRectF(QPointF(_src_rect.left() - _offset_x, _src_rect.top() - _offset_y), _src_rect.size()), _dst_rect, _transform))
	{
		_painter.setPen(pen());
		_painter.setBrush(brush());
		switch (line_style)
		{
		case Dots:
			{
				_painter.drawPoints(_transform.map(_points));
				break;
			}
		case Lines:
			{
				_painter.drawPolyline(_transform.map(_points));
				break;
			}
		case Ticks:
			{
				_painter.drawLines(_transform.map(_points));
				break;
			}
		case Bars:
			{
				foreach (const QRectF & _rect, _rects)
				{
					_painter.drawRect(_transform.mapRect(_rect));
				}
				break;
			}
		}
	}
	addCounter(_points.count() + _rects.count());
}

template <class T, class TX>
bool jItem1D<T, TX>::intersects(const QRectF & _rect, const jAxis * _x_axis, const jAxis * _y_axis) const
{
	THREAD_SAFE(Read)
	const unsigned int _width = size().width();
	const qreal _offset_x = origin().x();
	const qreal _offset_y = origin().y();
	QRectF _adj_rect = QRectF(_rect.left() - _offset_x, _rect.top() - _offset_y, _rect.width(), _rect.height());
	if (_x_axis && _x_axis->isLog10ScaleEnabled())
	{
		_adj_rect.setLeft(_x_axis->fromLog10(_adj_rect.left()));
		_adj_rect.setRight(_x_axis->fromLog10(_adj_rect.right()));
	}
	if (_y_axis && _y_axis->isLog10ScaleEnabled())
	{
		_adj_rect.setTop(_y_axis->fromLog10(_adj_rect.top()));
		_adj_rect.setBottom(_y_axis->fromLog10(_adj_rect.bottom()));
	}
	switch (data_model)
	{
	case FlatData:
		{
			const Flat * const _y_data = (const Flat * const)data();
			if (x_data == 0)
			{
				switch (line_style)
				{
				case Dots:
				case Lines:
					{
						for (unsigned int _x = 0; _x < _width; _x++)
						{
							if (_adj_rect.contains(QPointF(_x, _y_data[_x])))
							{
								THREAD_UNSAFE
								return true;
							}
						}
						break;
					}
				case Ticks:
				case Bars:
					{
						for (unsigned int _x = 0; _x < _width; _x++)
						{
							if (_adj_rect.intersects(QRectF(QPointF((int)_x - 1, _y_data[_x]), QPointF(_x + 1, 0))))
							{
								THREAD_UNSAFE
								return true;
							}
						}
						break;
					}
				}
			}
			else
			{
				switch (line_style)
				{
				case Dots:
				case Lines:
					{
						for (unsigned int _idx = 0; _idx < _width; _idx++)
						{
							if (_adj_rect.contains(QPointF(x_data[_idx], _y_data[_idx])))
							{
								THREAD_UNSAFE
								return true;
							}
						}
						break;
					}
				case Ticks:
				case Bars:
					{
						for (unsigned int _idx = 0; _idx < _width; _idx++)
						{
							if (_adj_rect.intersects(QRectF(QPointF((qreal)x_data[_idx] - 1, _y_data[_idx]), QPointF(x_data[_idx] + 1, 0))))
							{
								THREAD_UNSAFE
								return true;
							}
						}
						break;
					}
				}
			}
			break;
		}
	case PointData:
		{
			const Point * const _data = (const Point * const)data();
			switch (line_style)
			{
			case Dots:
			case Lines:
				{
					for (unsigned int _idx = 0; _idx < _width; _idx++)
					{
						if (_adj_rect.contains(QPointF(_data[_idx].x, _data[_idx].y)))
						{
							THREAD_UNSAFE
							return true;
						}
					}
					break;
				}
			case Ticks:
			case Bars:
				{
					for (unsigned int _idx = 0; _idx < _width; _idx++)
					{
						if (_adj_rect.intersects(QRectF(QPointF((qreal)_data[_idx].x - 1, _data[_idx].y), QPointF(_data[_idx].x + 1, 0))))
						{
							THREAD_UNSAFE
							return true;
						}
					}
					break;
				}
			}
			break;
		}
	case RadialData:
		{
			const Radial * const _data = (const Radial * const)data();
			switch (line_style)
			{
			case Dots:
			case Lines:
				{
					for (unsigned int _idx = 0; _idx < _width; _idx++)
					{
						if (_adj_rect.contains(QPointF(_data[_idx].v * ::cosf(_data[_idx].t), _data[_idx].v * ::sinf(_data[_idx].t))))
						{
							THREAD_UNSAFE
							return true;
						}
					}					
					break;
				}
			case Ticks:
			case Bars:
				{
					const qreal _mid_x = _adj_rect.x() + _adj_rect.width() / 2;
					const qreal _mid_y = _adj_rect.y() + _adj_rect.height() / 2;
					const qreal _mid_x_2 = _mid_x * _mid_x;
					const qreal _mid_y_2 = _mid_y * _mid_y;
					const qreal _v = ::qSqrt(_mid_x_2 + _mid_y_2 + 0.0001);
					for (unsigned int _idx = 0; _idx < _width; _idx++)
					{
						if (_v < _data[_idx].v)
						{
							THREAD_UNSAFE
							return true;
						}
					}					
					break;
				}
			}
			break;
		}
	}
	THREAD_UNSAFE
	return false;
}

template <class T, class TX>
QRectF jItem1D<T, TX>::boundingRect(const jAxis * _x_axis, const jAxis * _y_axis) const
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
	TX _left, _right;
	T _top, _bottom;
	switch (data_model)
	{
	case FlatData:
		{
			const Flat * const _y_data = (const Flat * const)data();
			_top = _y_data[0];
			_bottom = _y_data[0];
			if (x_data == 0)
			{
				_left = 0; _right = _width;
				for (unsigned int _x = 0; _x < _width; _x++)
				{
					if (_y_data[_x] < _top)
					{
						_top = _y_data[_x];
					}
					if (_y_data[_x] > _bottom)
					{
						_bottom = _y_data[_x];
					}
				}
			}
			else
			{
				_left = x_data[0];
				_right = x_data[0];
				for (unsigned int _idx = 0; _idx < _width; _idx++)
				{
					const qreal & _x = x_data[_idx];
					const qreal & _y = _y_data[_idx];
					if (_x < _left)
					{
						_left = _x;
					}
					if (_x > _right)
					{
						_right = _x;
					}
					if (_y < _top)
					{
						_top = _y;
					}
					if (_y > _bottom)
					{
						_bottom = _y;
					}
				}
			}
			break;
		}
	case PointData:
		{
			const Point * const _data = (const Point * const)data();
			_left = _data[0].x;
			_right = _data[0].x;
			_top = _data[0].y;
			_bottom = _data[0].y;
			for (unsigned int _idx = 0; _idx < _width; _idx++)
			{
				const qreal & _x = _data[_idx].x;
				const qreal & _y = _data[_idx].y;
				if (_x < _left)
				{
					_left = _x;
				}
				if (_x > _right)
				{
					_right = _x;
				}
				if (_y < _top)
				{
					_top = _y;
				}
				if (_y > _bottom)
				{
					_bottom = _y;
				}
			}
			break;
		}
	case RadialData:
		{
			const Radial * const _data = (const Radial * const)data();
			_left = _data[0].v * ::cosf(_data[0].t);
			_right = _data[0].v * ::cosf(_data[0].t);
			_top = _data[0].v * ::sinf(_data[0].t);
			_bottom = _data[0].v * ::sinf(_data[0].t);
			for (unsigned int _idx = 0; _idx < _width; _idx++)
			{
				const qreal & _x = _data[_idx].v * ::cosf(_data[_idx].t);
				const qreal & _y = _data[_idx].v * ::sinf(_data[_idx].t);
				if (_x < _left)
				{
					_left = _x;
				}
				if (_x > _right)
				{
					_right = _x;
				}
				if (_y < _top)
				{
					_top = _y;
				}
				if (_y > _bottom)
				{
					_bottom = _y;
				}
			}
			break;
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
const TX * jItem1D<T, TX>::x() const
{
	return x_data;
}

template <class T, class TX>
jItem1D<T, TX> & jItem1D<T, TX>::setData(typename jItem1D<T, TX>::Flat * _data, unsigned int _width, bool _deep_copy)
{
	if (x_data && isDeepCopy())
	{
		delete [] x_data;
		x_data = 0;
	}
	setDataModel(jItem1D<T, TX>::FlatData);
	jItem::setData(_data, _width, 1, _deep_copy);
	return * this;
}

template <class T, class TX>
jItem1D<T, TX> & jItem1D<T, TX>::setData(typename jItem1D<T, TX>::Flat * _data, TX * _x, unsigned int _width, bool _deep_copy = false)
{
	if (x_data && isDeepCopy())
	{
		delete [] x_data;
	}
	if (_deep_copy)
	{
		x_data = new TX[_width];
		::memcpy(x_data, _x, _width * sizeof(TX));
	}
	else
	{
		x_data = _x;
	}
	setDataModel(jItem1D<T, TX>::FlatData);
	jItem::setData(_data, _width, 1, _deep_copy);
	return * this;
}

template <class T, class TX>
jItem1D<T, TX> & jItem1D<T, TX>::setData(typename jItem1D<T, TX>::Point * _data, unsigned int _width, bool _deep_copy)
{
	if (x_data && isDeepCopy())
	{
		delete [] x_data;
		x_data = 0;
	}
	setDataModel(jItem1D<T, TX>::PointData);
	jItem::setData(_data, _width, 1, _deep_copy);
	return * this;
}

template <class T, class TX>
jItem1D<T, TX> & jItem1D<T, TX>::setData(typename jItem1D<T, TX>::Radial * _data, unsigned int _width, bool _deep_copy)
{
	if (x_data && isDeepCopy())
	{
		delete [] x_data;
		x_data = 0;
	}
	setDataModel(jItem1D<T, TX>::RadialData);
	jItem::setData(_data, _width, 1, _deep_copy);
	return * this;
}

// ------------------------------------------------------------------------

template <class T>
jItem2D<T>::jItem2D(int _conversion_flags, int _format): jItem()
{
	this->
		setImageConversionFlags(_conversion_flags).
		setImageFormat(_format).
		setConverter(0).
		setBytesPerItem(sizeof(T));
}

template <class T>
jItem2D<T>::~jItem2D()
{

}

template <class T>
void jItem2D<T>::render(QPainter & _painter, const QRectF & _dst_rect, const QRectF & _src_rect, const jAxis * _x_axis, const jAxis * _y_axis)
{
	THREAD_SAFE(Read)
	if (isVisible() == false)
	{
		THREAD_UNSAFE
		return;
	}
	if (data() == 0)
	{
		THREAD_UNSAFE
		return;
	}
	const QSize _size = size();
	unsigned int _w = _size.width();
	unsigned int _h = _size.height();
	const QPointF _origin = origin();
	void * _data_ptr = (void *)data();
	qint32 * _buf_i32 = convertToI32(_data_ptr, _size);
	bool _need_to_delete = _buf_i32 ? true : false;
	_buf_i32 = _buf_i32 ? _buf_i32 : (qint32 *)_data_ptr;
	QImage _image = QImage((const uchar *)_buf_i32, _w, _h, (QImage::Format)format);
	if (_x_axis && _x_axis->isLog10ScaleEnabled())
	{
		const unsigned int _log_w = _x_axis->toLog10(_w);
		QImage _result(_log_w, _h, (QImage::Format)format);
		QPainter _painter(&_result);
		for (unsigned int _x = 0; _x < _log_w - 1; _x++)
		{
			_painter.drawImage(
				QRectF(QPointF(_x, 0), QPointF(_x + 1, _h)), 
				_image, 
				QRectF(QPointF(_x_axis->fromLog10(_x), 0), QPointF(_x_axis->fromLog10(_x + 1), _h)));
		}
		_image = _result;
		_w = _log_w;
	}
	if (_y_axis && _y_axis->isLog10ScaleEnabled())
	{
		const unsigned int _log_h = _y_axis->toLog10(_h);
		QImage _result(_w, _log_h, (QImage::Format)format);
		QPainter _painter(&_result);
		for (unsigned int _y = 0; _y < _log_h - 1; _y++)
		{
			_painter.drawImage(
				QRectF(QPointF(0, _y), QPointF(_w, _y + 1)), 
				_image, 
				QRectF(QPointF(0, _h - _y_axis->fromLog10(_log_h - _y - 1)), QPointF(_w, _h - _y_axis->fromLog10(_log_h - _y))));
		}
		_image = _result;
		_h = _log_h;
	}
	QTransform _transform;
	if (::jQuadToQuad(_src_rect, _dst_rect, _transform))
	{
		QRectF _rect = _transform.mapRect(QRectF(
			_origin.x(), 
			_src_rect.top() + _src_rect.bottom() - _h - _origin.y(), 
			_w, 
			_h
			));
		QRectF _rect_img = QRectF(
			0, 
			0, 
			_w, 
			_h
			);	
		_painter.drawImage(
			_rect, 
			_image,
			_rect_img,
			(Qt::ImageConversionFlags)conversion_flags);
	}
	if (_need_to_delete)
	{
		delete [] _buf_i32;
	}
	THREAD_UNSAFE
	addCounter(_w * _h);
}

template <class T>
qint32 * jItem2D<T>::convertToI32(void * _data_ptr, const QSize & _size)
{
	if (converter == 0)
	{
		if (
			(typeid(T) == typeid(qint32)) ||
			(typeid(T) == typeid(quint32)) ||
			(typeid(T) == typeid(int)) || 
			(typeid(T) == typeid(unsigned int)) || 
			(typeid(T) == typeid(long)) || 
			(typeid(T) == typeid(unsigned long))
			)
		{
			if (sizeof(T) == 4)
			{
				return 0;
			}
		}
	}
	const unsigned int _buf_i32_sz = _size.width() * _size.height();
	qint32 * _buf_i32 = new qint32[_buf_i32_sz];
	const T * const _data = (const T * const)_data_ptr;
	if (converter == 0)
	{
		for (unsigned int _idx = 0; _idx < _buf_i32_sz; _idx++)
		{
			_buf_i32[_idx] = _data[_idx];
		}
	}
	else
	{
		for (unsigned int _idx = 0; _idx < _buf_i32_sz; _idx++)
		{
			_buf_i32[_idx] = converter(_idx, _data[_idx], this);
		}
	}
	if (format == QImage::Format_RGB32)
	{
		for (unsigned int _idx = 0; _idx < _buf_i32_sz; _idx++)
		{
			_buf_i32[_idx] |= 0xff000000;
		}
	}
	return _buf_i32;
}

template <class T>
jItem2D<T> & jItem2D<T>::setImageConversionFlags(int _conversion)
{
	SAFE_SET(conversion_flags, _conversion);
	return * this;
}

template <class T>
int jItem2D<T>::imageConversionFlags() const
{
	return conversion_flags;
}

template <class T>
jItem2D<T> & jItem2D<T>::setImageFormat(int _format)
{
	SAFE_SET(format, _format);
	return * this;
}

template <class T>
int jItem2D<T>::imageFormat() const
{
	return format;
}

template <class T>
jItem2D<T> & jItem2D<T>::setConverter(typename jItem2D<T>::converter_func _convert_func)
{
	SAFE_SET(converter, _convert_func);
	return * this;
}

template <class T>
typename jItem2D<T>::converter_func jItem2D<T>::converterFunc() const
{
	return SAFE_GET(converter);
}

template <class T>
bool jItem2D<T>::intersects(const QRectF & _rect, const jAxis * _x_axis, const jAxis * _y_axis) const
{
	if (isVisible() == false)
	{
		return false;
	}
	return _rect.intersects(boundingRect(_x_axis, _y_axis));
}

template <class T>
jItem2D<T> & jItem2D<T>::setData(T * _data, unsigned int _width, unsigned int _height, bool _deep_copy)
{
	jItem::setData(_data, _width, _height, _deep_copy);
	return * this;
}

#endif
