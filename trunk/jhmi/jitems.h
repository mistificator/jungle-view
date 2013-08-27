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
		Point(): x(0), y(0) {}
		Point(TX _x, T _y): x(_x), y(_y) {}
		Point(const QPointF & _pt): x(_pt.x()), y(_pt.y()) {}
        TX x;
        T y;
    };
    struct Radial
    {
		Radial(): v(0), t(0) {}
		Radial(TX _v, T _t): v(_v), t(_t) {}
        TX v;
        T t;
    };
    typedef T Flat;

    jItem1D(int _line_style = Lines, double _bar_width = 1.0);
    ~jItem1D();

    void render(QPainter & _painter, const QRectF & _dst_rect, const QRectF & _src_rect, const jAxis * _x_axis = 0, const jAxis * _y_axis = 0);

    int dataModel() const;

    jItem1D<T, TX> & setLineStyle(int _style);
    int lineStyle() const;

    jItem1D<T, TX> & setBarWidth(double _width);
    double barWidth() const;

    bool intersects(const QRectF & _rect, const jAxis * _x_axis = 0, const jAxis * _y_axis = 0) const;
    QRectF boundingRect(const jAxis * _x_axis = 0, const jAxis * _y_axis = 0) const;

    virtual jItem1D<T, TX> & setData(Flat * _data, unsigned int _width, bool _deep_copy = false);
    virtual jItem1D<T, TX> & setData(Flat * _data, TX * _x, unsigned int _width, bool _deep_copy = false);
    virtual jItem1D<T, TX> & setData(Point * _data, unsigned int _width, bool _deep_copy = false);
    virtual jItem1D<T, TX> & setData(Radial * _data, unsigned int _width, bool _deep_copy = false);
    virtual jItem1D<T, TX> & setData(const QPointF & _point, int _data_model = PointData);
    virtual jItem1D<T, TX> & setData(const QRectF & _rect);
    virtual jItem1D<T, TX> & setData(const QLineF & _line);

    const TX * xData() const;
    TX x(int _idx = 0) const;
    T y(int _idx = 0) const;
private:
    int data_model, line_style;
    double bar_width;
    TX * x_data;
protected:
    virtual jItem1D<T, TX> & setDataModel(int _model);
};

template <class T, class TX = T>
class jFigureItem : public jItem1D<T, TX>
{
    COPY_FBD(jFigureItem)
public:
    jFigureItem();
    ~jFigureItem();

    jFigureItem<T, TX> & setArcSymbol(const QRectF & _rectangle, int _startAngle, int _spanAngle);
    jFigureItem<T, TX> & setChordSymbol(const QRectF & _rectangle, int _startAngle, int _spanAngle);
    jFigureItem<T, TX> & setEllipseSymbol(const QRectF & _rectangle);
    jFigureItem<T, TX> & setPieSymbol(const QRectF & _rectangle, int _startAngle, int _spanAngle);
    jFigureItem<T, TX> & setRectSymbol(const QRectF & _rectangle);
    jFigureItem<T, TX> & setRoundedRectSymbol(const QRectF & _rectangle, double _xRadius, double _yRadius, Qt::SizeMode _mode = Qt::AbsoluteSize);
private:
    QImage symbol_img;
    QPainter * createPainter(const QRectF & _rectangle);
    void deletePainter(QPainter * & _painter);
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

    typedef QImage (*scale_func)(const QImage & /*_src_image*/, const QRectF & /*_src_rect*/, const QSizeF & /*_dst_size*/, jItem2D<T> *);
    jItem2D<T> & setScaler(scale_func _scale_func);
    scale_func scaleFunc() const;
private:
    qint32 * convertToI32(void * _data_ptr, const QSize & _size);
    int conversion_flags, format;
    converter_func converter;
    scale_func scaler;
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
jItem1D<T, TX>::jItem1D(int _line_style, double _bar_width): jItem()
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
jItem1D<T, TX> & jItem1D<T, TX>::setBarWidth(double _width)
{
    SAFE_SET(bar_width, _width);
    return * this;
}

template <class T, class TX>
double jItem1D<T, TX>::barWidth() const
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
        setBytesPerItem(sizeof(typename jItem1D<T>::Flat));
        break;
    }
    case PointData:
    {
        setBytesPerItem(sizeof(typename jItem1D<T>::Point));
        break;
    }
    case RadialData:
    {
        setBytesPerItem(sizeof(typename jItem1D<T>::Radial));
        break;
    }
    default:
    {
        SAFE_SET(data_model, (int)FlatData);
        setBytesPerItem(sizeof(typename jItem1D<T>::Flat));
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

static inline bool radialSort(const QPointF & _p1, const QPointF & _p2)
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
    const double _offset_x = _origin.x();
    const double _offset_y = _src_rect.top() + _src_rect.bottom() - _origin.y();
    const QSize _size = size();
    const unsigned int _width = _size.width();
    QImage _symbol_img = symbol();
    switch (data_model)
    {
		case FlatData:
		{
			const Flat * const _y_data = (const Flat * const)data();
			if (x_data == 0)
			{
				double _fleft = jAxis::normalizeFromScale(_x_axis, _src_rect.left() - _origin.x());
//				double _fright = jAxis::normalizeFromScale(_x_axis, _src_rect.right() - _origin.x());
				double _fright = jAxis::normalizeFromScale(_x_axis, _src_rect.right() - _origin.x() + 1.5);
				const double _bar_width = jAxis::normalizeFromScale(_x_axis, bar_width);
				if (_fright > _width)
				{
					_fright = _width;
				}
				const qint32 _left = ((qint32)_fleft) < 0 ? 0 : _fleft;
				const qint32 _right = ((qint32)_fright) < 0 ? 0 : _fright;
				const double _ratio = (_right - _left) / _dst_rect.width();
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
					if (_ratio >= 2)
					{
						const int _i_ratio = _ratio;
						QVector<T> _ys(_i_ratio);
						T * _ys_ptr = _ys.data();
						_points.resize((_right - _left) * 2 / _i_ratio);
						for (int _x = _left, _j = 0; _x <= _right - _i_ratio; _x+= _i_ratio)
						{
							::qMemCopy(_ys_ptr, _y_data + _x, _i_ratio * sizeof(T));
							T _ys_max = _ys[0];
							for (int _i = 1; _i < _i_ratio; _i++)
							{
								if (_ys_max < _ys_ptr[_i])
								{
									_ys_max = _ys_ptr[_i];
								}
							}
							_points[_j++] = QPointF(_x, -_ys_max);
							_points[_j++] = QPointF(_x, 0); 
						}
					}
					else
					{
						_points.reserve((_right - _left) * 2);
						for (int _x = _left; _x < _right; _x++)
						{
							_points << QPointF(_x, -_y_data[_x]);
							_points << QPointF(_x, 0);
						}
					}
					break;
				}
				case Bars:
				{
					if (_ratio >= 2)
					{
						const int _i_ratio = _ratio;
						QVector<T> _ys(_i_ratio);
						T * _ys_ptr = _ys.data();
						_rects.resize((_right - _left) / _i_ratio);
						for (int _x = _left, _j = 0; _x <= _right - _i_ratio; _x+= _i_ratio)
						{
							::qMemCopy(_ys_ptr, _y_data + _x, _i_ratio * sizeof(T));
							T _ys_max = _ys[0];
							for (int _i = 1; _i < _i_ratio; _i++)
							{
								if (_ys_max < _ys_ptr[_i])
								{
									_ys_max = _ys_ptr[_i];
								}
							}
							_rects[_j++] = QRectF(QPointF(_x - (_bar_width / 2.0), - _ys.back()), QSizeF(_bar_width, _ys.back()));
						}
					}
					else
					{
						_rects.reserve(_right - _left);
						for (int _x = _left; _x < _right; _x++)
						{
							_rects << QRectF(QPointF(_x - (_bar_width / 2.0), - _y_data[_x]), QSizeF(_bar_width, _y_data[_x]));
						}
					}
				}
				}
			}
			else
			{
				const double _bar_width = jAxis::normalizeFromScale(_x_axis, bar_width);
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
			const double _bar_width = jAxis::normalizeFromScale(_x_axis, bar_width);
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
						_points << QPointF(_data[_idx].v * qCos(_data[_idx].t), _data[_idx].v * qSin(-_data[_idx].t));
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
						const double & _v = _points[_idx].x();
						const double & _t = _points[_idx].y();
						_points[_idx] = QPointF(_v * qCos(_t), _v * qSin(-_t));
					}
					break;
				}
				case Ticks:
				{
					_points.reserve(_width * 2);
					for (unsigned int _idx = 0; _idx < _width; _idx++)
					{
						_points << QPointF(_data[_idx].v * qCos(_data[_idx].t), _data[_idx].v * qSin(-_data[_idx].t));
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
        const double _minimum = _src_rect.left();
        if (line_style == Bars)
        {
            for (int _idx = 0; _idx < _rects.count(); _idx++)
            {
                _rects[_idx].setLeft(_x_axis->toLog10(_rects[_idx].left(), _minimum));
            }
        }
        else
		{
			for (int _idx = 0; _idx < _points.count(); _idx++)
			{
				_points[_idx].setX(_x_axis->toLog10(_points[_idx].x(), _minimum));
			}
		}
    }
    if (_y_axis && _y_axis->isLog10ScaleEnabled())
    {
        const double _minimum = -_src_rect.bottom();
		if (line_style == Bars)
		{
			for (int _idx = 0; _idx < _rects.count(); _idx++)
			{
				_rects[_idx].setTop(-_y_axis->toLog10(-_rects[_idx].top(), _minimum));
			}
		}
		else
		{
			for (int _idx = 0; _idx < _points.count(); _idx++)
			{
				_points[_idx].setY(-_y_axis->toLog10(-_points[_idx].y(), _minimum));
			}
		}
    }
    THREAD_UNSAFE

    QTransform _transform;
    QRectF _adj_src_rect = QRectF(QPointF(_src_rect.left() - _offset_x, _src_rect.top() - _offset_y), _src_rect.size());
    if (::jQuadToQuad(_adj_src_rect, _dst_rect, _transform))
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
        if ((_symbol_img.width() > 0) && (_symbol_img.height() > 0))
        {
            const double _img_offs_x = _symbol_img.width() / 2.0;
            const double _img_offs_y = _symbol_img.height() / 2.0;
            foreach (const QPointF & _pt, _transform.map(_points))
            {
                _painter.drawImage(QPointF(_pt.x() - _img_offs_x, _pt.y() - _img_offs_y), _symbol_img);
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
    const double _offset_x = origin().x();
    const double _offset_y = origin().y();
    QRectF _adj_rect = QRectF(_rect.left() - _offset_x, _rect.top() - _offset_y, _rect.width(), _rect.height());

	_adj_rect.setLeft(jAxis::normalizeFromScale(_x_axis, _adj_rect.left()));
    _adj_rect.setRight(jAxis::normalizeFromScale(_x_axis, _adj_rect.right()));
    _adj_rect.setTop(jAxis::normalizeFromScale(_y_axis, _adj_rect.top()));
    _adj_rect.setBottom(jAxis::normalizeFromScale(_y_axis, _adj_rect.bottom()));

	QPainterPath _pp;
    switch (data_model)
    {
		case FlatData:
		{
			const Flat * const _y_data = (const Flat * const)data();
			if (x_data == 0)
			{
				if (_width > 0)
				{
					_pp.moveTo(QPointF(0, _y_data[0]));
				}
				for (unsigned int _x = 1; _x < _width; _x++)
				{
					_pp.lineTo(QPointF(_x, _y_data[_x]));
				}
			}
			else
			{
				if (_width > 0)
				{
					_pp.moveTo(QPointF(x_data[0], _y_data[0]));
				}
				for (unsigned int _idx = 1; _idx < _width; _idx++)
				{
					_pp.lineTo(QPointF(x_data[_idx], _y_data[_idx]));
				}
			}
			break;
		}
		case PointData:
		{
			const Point * const _data = (const Point * const)data();
			if (_width > 0)
			{
				_pp.moveTo(QPointF(_data[0].x, _data[0].y));
			}
			for (unsigned int _idx = 1; _idx < _width; _idx++)
			{
				_pp.lineTo(QPointF(_data[_idx].x, _data[_idx].y));
			}
			break;
		}
		case RadialData:
		{
			const Radial * const _data = (const Radial * const)data();
			_pp.moveTo(QPointF(_offset_x, _offset_y));
			for (unsigned int _idx = 0; _idx < _width; _idx++)
			{
				_pp.lineTo(QPointF(_data[_idx].v * qCos(_data[_idx].t), _data[_idx].v * qSin(-_data[_idx].t)));
			}
			break;
		}
    }
    THREAD_UNSAFE
    return _pp.intersects(_adj_rect);
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
    const double _offset_x = origin().x();
    const double _offset_y = origin().y();
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
                const double & _x = x_data[_idx];
                const double & _y = _y_data[_idx];
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
            const double & _x = _data[_idx].x;
            const double & _y = _data[_idx].y;
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
        _left = _data[0].v * qCos(_data[0].t);
        _right = _data[0].v * qCos(_data[0].t);
        _top = _data[0].v * qSin(-_data[0].t);
        _bottom = _data[0].v * qSin(-_data[0].t);
        for (unsigned int _idx = 0; _idx < _width; _idx++)
        {
            const double & _x = _data[_idx].v * qCos(_data[_idx].t);
            const double & _y = _data[_idx].v * qSin(-_data[_idx].t);
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
	_left = jAxis::normalizeToScale(_x_axis, _left);
	_right = jAxis::normalizeToScale(_x_axis, _right);
	_top = jAxis::normalizeToScale(_y_axis, _top);
	_bottom = jAxis::normalizeToScale(_y_axis, _bottom);
    _left += _offset_x;
    _right += _offset_x;
    _top += _offset_y;
    _bottom += _offset_y;
    if (_x_axis && _x_axis->isLog10ScaleEnabled())
    {
        if (_left - _offset_x <= 0)
        {
            _left = _x_axis->lo();
        }
    }
    if (_y_axis && _y_axis->isLog10ScaleEnabled())
    {
        if (_bottom - _offset_y <= 0)
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
const TX * jItem1D<T, TX>::xData() const
{
    return x_data;
}

template <class T, class TX>
TX jItem1D<T, TX>::x(int _idx) const
{
    const int _width = size().width();
    if (_width <= 0)
    {
        return (0);
    }
    _idx = qMin<int>(qMax<int>(0, _idx), _width - 1);
    switch (data_model)
    {
        case FlatData:
        {
            if (x_data == 0)
            {
                return (_idx);
            }
            else
            {
                return (x_data[_idx]);
            }
        }
        case PointData:
        {
            return ((const Point * const)data())[_idx].x;
        }
    }
    return (0);
}

template <class T, class TX>
T jItem1D<T, TX>::y(int _idx) const
{
    const int _width = size().width();
    if (_width <= 0)
    {
        return (0);
    }
    _idx = qMin<int>(qMax<int>(0, _idx), _width - 1);
    switch (data_model)
    {
        case FlatData:
        {
            return ((const Flat * const)data())[_idx];
        }
        case PointData:
        {
            return ((const Point * const)data())[_idx].y;
        }
    }
    return (0);
}

template <class T, class TX>
jItem1D<T, TX> & jItem1D<T, TX>::setData(typename jItem1D<T, TX>::Flat * _data, unsigned int _width, bool _deep_copy)
{
	TX * _prev_x_data = 0;
    if (x_data && isDeepCopy())
    {
		_prev_x_data = x_data;
        x_data = 0;
    }
    setDataModel(jItem1D<T, TX>::FlatData);
    jItem::setData(_data, _width, 1, _deep_copy);
	if (_prev_x_data)
	{
        delete [] x_data;
	}
    return * this;
}

template <class T, class TX>
jItem1D<T, TX> & jItem1D<T, TX>::setData(typename jItem1D<T, TX>::Flat * _data, TX * _x, unsigned int _width, bool _deep_copy = false)
{
	TX * _prev_x_data = 0;
    if (x_data && isDeepCopy())
    {
		_prev_x_data = x_data;
        x_data = 0;
    }
    if (_deep_copy)
    {
        x_data = new TX[_width];
        ::qMemCopy(x_data, _x, _width * sizeof(TX));
    }
    else
    {
        x_data = _x;
    }
    setDataModel(jItem1D<T, TX>::FlatData);
    jItem::setData(_data, _width, 1, _deep_copy);
	if (_prev_x_data)
	{
        delete [] x_data;
	}
    return * this;
}

template <class T, class TX>
jItem1D<T, TX> & jItem1D<T, TX>::setData(typename jItem1D<T, TX>::Point * _data, unsigned int _width, bool _deep_copy)
{
	TX * _prev_x_data = 0;
    if (x_data && isDeepCopy())
    {
		_prev_x_data = x_data;
        x_data = 0;
    }
    setDataModel(jItem1D<T, TX>::PointData);
    jItem::setData(_data, _width, 1, _deep_copy);
	if (_prev_x_data)
	{
        delete [] x_data;
	}
    return * this;
}

template <class T, class TX>
jItem1D<T, TX> & jItem1D<T, TX>::setData(typename jItem1D<T, TX>::Radial * _data, unsigned int _width, bool _deep_copy)
{
	TX * _prev_x_data = 0;
    if (x_data && isDeepCopy())
    {
		_prev_x_data = x_data;
        x_data = 0;
    }
    setDataModel(jItem1D<T, TX>::RadialData);
    jItem::setData(_data, _width, 1, _deep_copy);
	if (_prev_x_data)
	{
        delete [] x_data;
	}
    return * this;
}

template <class T, class TX>
jItem1D<T, TX> & jItem1D<T, TX>::setData(const QPointF & _point, int _data_model)
{
	if (_data_model == RadialData)
	{
		jItem1D<T, TX>::Radial _pt;
		_pt.v = _point.x(); _pt.t = _point.y();
		jItem1D<T, TX>::setData(& _pt, 1, true);
	}
	else
	{
		jItem1D<T, TX>::Point _pt;
		_pt.x = _point.x(); _pt.y = _point.y();
		jItem1D<T, TX>::setData(& _pt, 1, true);
	}
    return (* this);
}

template <class T, class TX>
jItem1D<T, TX> & jItem1D<T, TX>::setData(const QRectF & _rect)
{
	jItem1D<T, TX>::Point _pt[5];
	_pt[0].x = _rect.left(); _pt[0].y = _rect.top();
	_pt[1].x = _rect.right(); _pt[1].y = _rect.top();
	_pt[2].x = _rect.right(); _pt[2].y = _rect.bottom();
	_pt[3].x = _rect.left(); _pt[3].y = _rect.bottom();
	_pt[4].x = _rect.left(); _pt[4].y = _rect.top();
	jItem1D<T, TX>::setData(_pt, 5, true);
    return (* this);
}

template <class T, class TX>
jItem1D<T, TX> & jItem1D<T, TX>::setData(const QLineF & _line)
{
	jItem1D<T, TX>::Point _pt[2];
	_pt[0].x = _line.x1(); _pt[0].y = _line.y1();
	_pt[1].x = _line.x2(); _pt[1].y = _line.y2();
	jItem1D<T, TX>::setData(_pt, 2, true);
    return (* this);
}

// ------------------------------------------------------------------------

template <class T, class TX>
jFigureItem<T, TX>::jFigureItem() : jItem1D<T, TX>(jItem1D<T, TX>::Dots)
{
}

template <class T, class TX>
jFigureItem<T, TX>::~jFigureItem()
{
}

template <class T, class TX>
QPainter * jFigureItem<T, TX>::createPainter(const QRectF & _rectangle)
{
    SAFE_SET(symbol_img, QImage(QSize(_rectangle.width() + 1, _rectangle.height() + 1), QImage::Format_ARGB32));
    symbol_img.fill(0x000000ff);
    QPainter * _painter = new QPainter(& symbol_img);
    _painter->setPen(pen());
    _painter->setBrush(brush());
    return _painter;
}

template <class T, class TX>
void jFigureItem<T, TX>::deletePainter(QPainter * & _painter)
{
    delete _painter;
    _painter = 0;
    setSymbol(symbol_img);
}

template <class T, class TX>
jFigureItem<T, TX> & jFigureItem<T, TX>::setArcSymbol(const QRectF & _rectangle, int _startAngle, int _spanAngle)
{
    QPainter * _painter = createPainter(_rectangle);
    _painter->drawArc(_rectangle, _startAngle, _spanAngle);
    deletePainter(_painter);
    return * this;
}

template <class T, class TX>
jFigureItem<T, TX> & jFigureItem<T, TX>::setChordSymbol(const QRectF & _rectangle, int _startAngle, int _spanAngle)
{
    QPainter * _painter = createPainter(_rectangle);
    _painter->drawChord(_rectangle, _startAngle, _spanAngle);
    deletePainter(_painter);
    return * this;
}

template <class T, class TX>
jFigureItem<T, TX> & jFigureItem<T, TX>::setEllipseSymbol(const QRectF & _rectangle)
{
    QPainter * _painter = createPainter(_rectangle);
    _painter->drawEllipse(_rectangle);
    deletePainter(_painter);
    return * this;
}

template <class T, class TX>
jFigureItem<T, TX> & jFigureItem<T, TX>::setPieSymbol(const QRectF & _rectangle, int _startAngle, int _spanAngle)
{
    QPainter * _painter = createPainter(_rectangle);
    _painter->drawPie(_rectangle, _startAngle, _spanAngle);
    deletePainter(_painter);
    return * this;
}

template <class T, class TX>
jFigureItem<T, TX> & jFigureItem<T, TX>::setRectSymbol(const QRectF & _rectangle)
{
    QPainter * _painter = createPainter(_rectangle);
    _painter->drawRect(_rectangle);
    deletePainter(_painter);
    return * this;
}

template <class T, class TX>
jFigureItem<T, TX> & jFigureItem<T, TX>::setRoundedRectSymbol( const QRectF & _rectangle, double _xRadius, double _yRadius, Qt::SizeMode _mode)
{
    QPainter * _painter = createPainter(_rectangle);
    _painter->drawRoundedRect(_rectangle, _xRadius, _yRadius, _mode);
    deletePainter(_painter);
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
            setScaler(0).
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
	    QRectF _adj_src_rect = QRectF(QPointF(_src_rect.left() - _origin.x(), _src_rect.top() - _origin.y()), _src_rect.size());
		if (_adj_src_rect.left() < 0)
		{
			_adj_src_rect.setLeft(0);
		}
		if (_adj_src_rect.top() < 0)
		{
			_adj_src_rect.setTop(0);
		}
		if (_adj_src_rect.right() > _w)
		{
			_adj_src_rect.setRight(_w);
		}
		if (_adj_src_rect.bottom() > _h)
		{
			_adj_src_rect.setBottom(_h);
		}

		const QRectF _src_img_rect = QRectF(
			_adj_src_rect.left(),
			_h - (_adj_src_rect.bottom()),
			_adj_src_rect.width(),
			_adj_src_rect.height()
			);

		if (scaler)
		{
			_painter.drawImage(QPointF(), scaler(_image, _src_img_rect, _transform.mapRect(_adj_src_rect).size(), this));
		}
		else
		{
			_painter.drawImage(QRectF(QPointF(), _transform.mapRect(_adj_src_rect).size()), _image, _src_img_rect, (Qt::ImageConversionFlags)conversion_flags);
		}
	}

	if (_need_to_delete)
    {
        delete [] _buf_i32;
    }
    THREAD_UNSAFE
	addCounter(_w * _h);
}

#include <typeinfo>

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
            _buf_i32[_idx] &= 0x00ffffff;
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

template <class T>
jItem2D<T> & jItem2D<T>::setScaler(typename jItem2D<T>::scale_func _scale_func)
{
    SAFE_SET(scaler, _scale_func);
    return * this;
}

template <class T>
typename jItem2D<T>::scale_func jItem2D<T>::scaleFunc() const
{
    return SAFE_GET(scaler);
}

#endif
