#include "jview.h"
#include "jitems.h"
#include <math.h>

// ------------------------------------------------------------------------

__inline bool jQuadToQuad(const QRectF & _from, const QRectF & _to, QTransform & _transform)
{
	return QTransform::quadToQuad(QPolygonF(_from).mid(0, 4), QPolygonF(_to).mid(0, 4), _transform);
}

__inline bool intEqual(double _op1, double _op2)
{
//	static const double _precision = 1000000;
//	return ((qint64)(_op1 * _precision + (double)0.5) == (qint64)(_op2 * _precision + (double)0.5));
//	return qFuzzyCompare(_op1, _op2);
	return qFuzzyCompare((float)_op1, (float)_op2);
}

__inline bool fuzzyRectFCompare(const QRectF & r1, const QRectF & r2)
{
	return qFuzzyCompare(r1.left(), r2.left()) && qFuzzyCompare(r1.right(), r2.right()) && qFuzzyCompare(r1.top(), r2.top()) && qFuzzyCompare(r1.bottom(), r2.bottom());
}

__inline bool fuzzySizeFCompare(const QSizeF & s1, const QSizeF & s2)
{
	return qFuzzyCompare(s1.width(), s2.width()) && qFuzzyCompare(s1.height(), s2.height());
}

// ------------------------------------------------------------------------

struct jAxis::Data
{
	double lo, hi;
	jAxis::range_func range_func;
	unsigned int count, count_hint;
	double alignment;
	unsigned int tick_length;
	QPen pen, grid_pen;
	QFont font;
	bool visible;
	QBrush background;
	int id;
	double log10_mpy;
	bool log10_enabled;
	double alignment_offset;
	Data()
	{
		log10_enabled = false;
		log10_mpy = 1.0;
		lo = 0.0;
		hi = 0.0;
		range_func = &jAxis::default_range_convert;
		count = 0;
		count_hint = 10;
		alignment = 1.0;
		tick_length = 5;
		visible = true;
        grid_pen = QColor(Qt::black);
		id = 0;
		alignment_offset = 0.0;
	}
	~Data()
	{

	}
	__inline double alignTick(double _value, double _alignment) const
	{
		qint64 _k = 0;
		if (alignment_offset >= _alignment)
		{
			_k = (alignment_offset / _alignment);
		}
		else
		if (alignment_offset < 0)
		{
			_k = (alignment_offset / _alignment);
			if (_k * _alignment != alignment_offset)
			{
				_k--;
			}
		}
		return ((static_cast<qint64>(_value / _alignment) - _k) * _alignment) + alignment_offset;
	}
	__inline QVector<double> calcTicks(double _lo, double _hi) const
	{
		if (alignment <= 0.0)
		{
			return QVector<double>();
		}
		if (intEqual(_hi, _lo))
		{
			return QVector<double>();
		}

		const double _diff = _hi - _lo;
		if (_diff <= 0)
		{
			return QVector<double>();
		}

		const int _n_diff = ::log10(qMax<double>(qAbs<double>(_diff), 1.0));
		const int _n = qMax<int>(_n_diff, 1);

		double _step = ::pow(10.0f, _n - 1) * alignment;
		const double _k[2] = {2, 5};

		int _k_idx = 1;
		while (_diff > _step * count_hint)
		{
			_step *= _k[_k_idx % 2];
			_k_idx++;
		}

		_k_idx = 0;
		while (_diff < _step * count_hint)
		{
			_step /= _k[_k_idx % 2];
			_k_idx++;
		}

		QVector<double> _ticks;
		const double _start_tick = alignTick(_lo, _step);
        qint64 _tick_number = 0;
        for (double _value = _start_tick; _value <= _hi; _value = _start_tick + (++_tick_number) * _step)
		{
			_ticks << _value;
		}

        if (intEqual(qFloor(_step), _step))
		{
			quint64 _mply = 1;
			while ((_ticks.count() / 2) * 2 > (int)count_hint)
			{
				_mply *= 2;
				for (int _idx = 0; _idx < _ticks.count(); )
				{
                    if (!intEqual(alignTick(_ticks[_idx], _step * _mply), _ticks[_idx]))
                    {
                        _ticks.remove(_idx);
					}
					else
					{
						_idx++;
					}
				}
			}
		}

		return _ticks;
	}
};

jAxis::jAxis(): d(new Data())
{

}

jAxis::~jAxis()
{
	delete d;
}

jAxis::jAxis(const jAxis & _other): d(new Data())
{
	* d = * _other.d;
}

jAxis & jAxis::operator = (const jAxis & _other)
{
	if (& _other != this)
	{
		* d = * _other.d;
	}
	return * this;
}

QString jAxis::default_range_convert(double _value, jAxis *)
{
	return QString::number(_value, 'f', 1);
}

jAxis & jAxis::setRange(double _lo, double _hi, jAxis::range_func _range_func)
{
	d->lo = _lo;
	d->hi = _hi;
	d->range_func = _range_func ? _range_func : &jAxis::default_range_convert;
	return * this;
}

jAxis & jAxis::setRangeFunc(range_func _range_func)
{
	d->range_func = _range_func ? _range_func : &jAxis::default_range_convert;
	return * this;
}

double jAxis::lo() const
{
	return d->lo;
}

double jAxis::hi() const
{
	return d->hi;
}

jAxis::range_func jAxis::rangeFunc() const
{
	return d->range_func;
}

jAxis & jAxis::setStep(unsigned int _count_hint, double _alignment)
{
	d->count_hint = _count_hint ? _count_hint : 1;
	d->alignment = (_alignment > 0.0) ? _alignment : 1.0;
	return * this;
}

unsigned int jAxis::countHint() const
{
	return d->count_hint;
}

unsigned int jAxis::count() const
{
	return d->count;
}

double jAxis::alignment() const
{
	return d->alignment;
}

jAxis & jAxis::setAlignmentOffset(double _offset)
{
	d->alignment_offset = _offset;
	return (* this);
}

double jAxis::alignmentOffset() const
{
	return (d->alignment_offset);
}

jAxis & jAxis::setTickLength(unsigned int _length)
{
	d->tick_length = _length;
	return * this;
}

unsigned int jAxis::tickLength() const
{
	return d->tick_length;
}

void jAxis::render(QPainter & _painter, const QRectF & _dst_rect, int _orientation, double _lo, double _hi, bool _draw_grid)
{
	if (d->visible == false)
	{
		return;
	}
	QPen _pen = d->pen;
	QBrush _background = d->background;
	QPen _grid_pen = d->grid_pen;
//	_painter.setPen(_pen);
	_painter.setPen(_background.color());
	_painter.setFont(d->font);
	QFontMetrics _metrics = _painter.fontMetrics();
	range_func _range_func = d->range_func;
	const QRect _lo_rect = _metrics.boundingRect(_range_func(_lo, this));
	const QRect _hi_rect = _metrics.boundingRect(_range_func(_hi, this));
	switch (_orientation)
	{
		case Qt::Vertical:
		{
			d->count = (_dst_rect.height() > 0 ? _dst_rect.height() : 1) / (qMax(_lo_rect.height(), _hi_rect.height()) * 1.1);
			break;
		}
		case Qt::Horizontal:
		{
			d->count = (_dst_rect.width() > 0 ? _dst_rect.width() : 1) / (qMax(_lo_rect.width(), _hi_rect.width()) * 1.1);
			break;
		}
		default:
		{
			d->count = 0;
			break;
		}
	}
	QVector<double> _ticks = d->calcTicks(_lo, _hi);
	if (_ticks.count() > 0)
	{
		while (_ticks.count() > (int)d->count)
		{
            for (int _idx = 0; _idx < _ticks.count(); _idx++)
			{
				_ticks.remove(_idx);
			}
		}
	}
	d->count = _ticks.count();
	const unsigned int _tick_length = 5;
	switch (_orientation)
	{
	case Qt::Vertical:
		{
			const int _x = _tick_length + _pen.width();
			_painter.setPen(_background.color());
			_painter.drawLine(
				_dst_rect.left() + _pen.width() / 2,
				_dst_rect.top(),
				_dst_rect.left() + _pen.width() / 2,
				_dst_rect.bottom()
				);
			for (int _idx = 0; _idx < _ticks.count(); _idx++)
			{
				int _y = _dst_rect.height() - ((_ticks[_idx] - _lo) * _dst_rect.height() / (_hi - _lo));
				_painter.setPen(_background.color());
				_painter.drawLine(0, _y, _tick_length, _y);
				QString _str = _range_func(_ticks[_idx], this);
				QRect _rect = _metrics.boundingRect(_str);
				const int _w = _rect.width();
				const int _h = _rect.height();
				_y += _h / 2;
				if ((_y - _h >= 0) && (_y + (_h / 2) <= _dst_rect.height()))
				{
					_painter.fillRect(QRectF(QPointF(_x, _y - _h + 2), _rect.size()), _background);
					_painter.setPen(_pen);
					_painter.drawText(_x, _y, _str);
				}
                if (_draw_grid)
                {
                    //_painter.save();
                    _painter.setPen(_grid_pen);
                    _painter.drawLine(_x + _w, _y - _h / 2, _dst_rect.width(), _y - _h / 2);
                    //_painter.restore();
                }
			}
			break;
		}
	case Qt::Horizontal:
		{
			const int _y = _dst_rect.bottom() - _tick_length - _pen.width() * 1.5;
			_painter.drawLine(
				_dst_rect.left(),
				_dst_rect.bottom() + (_pen.width() > 1 ? - _pen.width() + 1 : -1),
				_dst_rect.right(),
				_dst_rect.bottom() + (_pen.width() > 1 ? - _pen.width() + 1 : -1)
				);
			for (int _idx = 0; _idx < _ticks.count(); _idx++)
			{
				int _x = (_ticks[_idx] - _lo) * _dst_rect.width() / (_hi - _lo);
				_painter.setPen(_background.color());
				_painter.drawLine(_x, _dst_rect.bottom(), _x, _y + _pen.width());
				QString _str = _range_func(_ticks[_idx], this);
				QRect _rect = _metrics.boundingRect(_str);
				const int _w = _rect.width();
				const int _h = _rect.height();
				_x -= _w / 2;
				if ((_x - (_w / 2) >= 0) && (_x + _w <= _dst_rect.width()))
				{
					_painter.fillRect(QRectF(QPointF(_x, _y - _h + 2), _rect.size()), _background);
					_painter.setPen(_pen);
					_painter.drawText(_x, _y, _str);
				}
                if (_draw_grid)
                {
                    //_painter.save();
                    _painter.setPen(_grid_pen);
                    _painter.drawLine(_x + _w / 2, 0, _x + _w / 2, _y - _h + 2);
                    //_painter.restore();
                }
			}
			break;
		}
	}
}

jAxis & jAxis::setPen(const QPen & _pen)
{
	d->pen = _pen;
	return * this;
}

QPen jAxis::pen() const
{
	return d->pen;
}

jAxis & jAxis::setFont(const QFont & _font)
{
	d->font = _font;
	return * this;
}

QFont jAxis::font() const
{
	return d->font;
}

jAxis & jAxis::setVisible(bool _state)
{
	d->visible = _state;
	return * this;
}

bool jAxis::isVisible() const
{
	return d->visible;
}

jAxis & jAxis::setBackground(const QBrush & _brush)
{
	d->background = _brush;
	return * this;
}

QBrush jAxis::background() const
{
	return d->background;
}

jAxis & jAxis::setGridPen(const QPen & _grid_pen)
{
	d->grid_pen = _grid_pen;
	return * this;
}

QPen jAxis::gridPen() const
{
	return d->grid_pen;
}

jAxis & jAxis::setId(int _id)
{
	d->id = _id;
	return * this;
}

int jAxis::id() const
{
	return d->id;
}

jAxis & jAxis::setLog10ScaleEnabled(bool _state)
{
    d->log10_enabled = _state;
	return * this;
}

bool jAxis::isLog10ScaleEnabled() const
{
	return d->log10_enabled;
}

jAxis & jAxis::setLog10Multiplier(double _mpy)
{
    d->log10_mpy = _mpy;
	return * this;
}

double jAxis::log10Multiplier() const
{
	return d->log10_mpy;
}

double jAxis::toLog10(double _value, double _minimum) const
{
	return (_value > 0.0) ? d->log10_mpy * ::log10(_value) : _minimum;
}

double jAxis::fromLog10(double _value) const
{
	return (d->log10_mpy != 0.0) ? ::pow(10.0, _value / d->log10_mpy) : 0.0;
}

double jAxis::mapToAxis(double _value, const jAxis & _dst) const
{
	return (!intEqual(d->hi, d->lo)) ? _dst.d->lo + ((_value - d->lo) * (_dst.d->hi - _dst.d->lo) / (d->hi - d->lo)) : 0.0;
}

double jAxis::mapFromAxis(double _value, const jAxis & _src) const
{
	return (!intEqual(_src.d->hi, _src.d->lo)) ? d->lo + ((_value - _src.d->lo) * (d->hi - d->lo) / (_src.d->hi - _src.d->lo)) : 0.0;
}

double jAxis::normalizeToScale(double _value, double _minimum) const
{
	return (d->log10_enabled ? toLog10(_value, _minimum) : _value);
}

double jAxis::normalizeFromScale(double _value) const
{
	return (d->log10_enabled ? fromLog10(_value) : _value);
}

double jAxis::normalizeToScale(const jAxis * _axis, double _value, double _minimum)
{
	return (_axis ? _axis->normalizeToScale(_value, _minimum) : _value);
}

double jAxis::normalizeFromScale(const jAxis * _axis, double _value)
{
	return (_axis ? _axis->normalizeFromScale(_value) : _value);
}

// ------------------------------------------------------------------------

struct jSelector::Data
{	
	QRectF rect;
	bool visible;
	QBrush background;
	jItem1D<double> item;
	bool preview_enabled;
	Data() 
	{
		visible = true;
		preview_enabled = false;
	}
	~Data()
	{

	}
};

jSelector::jSelector(): d(new Data())
{

}

jSelector::~jSelector()
{
	delete d;
}

jSelector & jSelector::setPen(const QPen & _pen)
{
	d->item.setPen(_pen);
	return * this;
}

QPen jSelector::pen() const
{
	return d->item.pen();
}

jSelector & jSelector::setRect(const QRectF & _rect)
{
	d->rect = _rect;
	d->item.setData(_rect);
	return * this;
}

QRectF jSelector::rect() const
{
	return d->rect;
}

jSelector & jSelector::setVisible(bool _state)
{
	d->visible = _state;
	return * this;
}

bool jSelector::isVisible() const
{
	return d->visible;
}

void jSelector::render(QPainter & _painter, const QRectF & _dst_rect, const QRectF & _src_rect)
{
	if (d->visible == false)
	{
		return;
	}
	QTransform _transform;
	const QRectF _rect = d->rect;
	const QBrush _brush = d->background;
	if (::jQuadToQuad(_src_rect, _dst_rect, _transform))
	{
		_painter.fillRect(_transform.mapRect(
			QRectF(
			_rect.x(), 
			_src_rect.top() + _src_rect.bottom() - _rect.y() - _rect.height(), 
			_rect.width(), 
			_rect.height()
			)), 
			_brush);
	}
	d->item.render(_painter, _dst_rect, _src_rect);
}

void jSelector::renderPreview(QPainter & _painter, const QRectF & _dst_rect, const QRectF & _src_rect)
{
	if (d->preview_enabled)
	{
		render(_painter, _dst_rect, _src_rect);
	}
}

jSelector & jSelector::setBackground(const QBrush & _brush)
{
	d->background = _brush;
	return * this;
}

QBrush jSelector::background() const
{
	return d->background;
}

jSelector & jSelector::setPreviewEnabled(bool _state)
{
	d->preview_enabled = _state;
	return * this;
}

bool jSelector::previewEnabled() const
{
	return d->preview_enabled;
}

jItem & jSelector::internalItem() const
{
	return d->item;
}

// ------------------------------------------------------------------------

struct jViewport::Data
{	
	friend class jSync;
	QRectF base;
	QVector<QRectF> history;
	jSelector selector;
	int orientation;
	QSizeF minimum_size, maximum_size;
	Data()
	{
		history << QRectF();
		selector.setVisible(false);
		minimum_size = QSizeF(0, 0);
		maximum_size = QSizeF(0, 0);
	}
	~Data()
	{

	}
	__inline QRectF minmaxRect0(QRectF _rect) const
	{
		_rect = minmaxRect(_rect);
		if (_rect.size().width() < maximum_size.width() && maximum_size.width() > 0)
		{
			const double _dx = maximum_size.width() - _rect.size().width();
			_rect.setLeft(_rect.left() - _dx / 2.0);			
			_rect.setRight(_rect.right() + _dx / 2.0);			
		}
		if (_rect.size().height() < maximum_size.height() && maximum_size.height() > 0)
		{
			const double _dy = maximum_size.height() - _rect.size().height();
			_rect.setTop(_rect.top() - _dy / 2.0);			
			_rect.setBottom(_rect.bottom() + _dy / 2.0);			
		}
		return _rect;

	}
	__inline QRectF minmaxRect(QRectF _rect) const
	{
		if (_rect.size().width() < minimum_size.width() && minimum_size.width() >= 0)
		{
			const double _dx = minimum_size.width() - _rect.size().width();
			_rect.setLeft(_rect.left() - _dx / 2.0);			
			_rect.setRight(_rect.right() + _dx / 2.0);			
		}
		if (_rect.size().width() > maximum_size.width() && maximum_size.width() > 0)
		{
			const double _dx = _rect.size().width() - maximum_size.width();
			_rect.setLeft(_rect.left() + _dx / 2.0);			
			_rect.setRight(_rect.right() - _dx / 2.0);			
		}
		if (_rect.size().height() < minimum_size.height() && minimum_size.height() >= 0)
		{
			const double _dy = minimum_size.height() - _rect.size().height();
			_rect.setTop(_rect.top() - _dy / 2.0);			
			_rect.setBottom(_rect.bottom() + _dy / 2.0);			
		}
		if (_rect.size().height() > maximum_size.height() && maximum_size.height() > 0)
		{
			const double _dy = _rect.size().height() - maximum_size.height();
			_rect.setTop(_rect.top() + _dy / 2.0);			
			_rect.setBottom(_rect.bottom() - _dy / 2.0);			
		}
		return _rect;
	}
	__inline QRectF adjustRect(const QRectF & _rect, bool _to_orientation_only = false) const
	{
		QRectF _adj_rect = _to_orientation_only ? _rect.intersected(base) : minmaxRect(_rect).intersected(base);
		if ((orientation & Qt::Vertical) == 0)
		{
			_adj_rect.setTop(base.top());
			_adj_rect.setBottom(base.bottom());
		}
		if ((orientation & Qt::Horizontal) == 0)
		{
			_adj_rect.setLeft(base.left());
			_adj_rect.setRight(base.right());
		}
		return _adj_rect;
	}
	__inline void adjustHistory()
	{
		if (!history.isEmpty())
		{
			history[0] = minmaxRect0(history[0]);
		}
		for (int _idx = 1; _idx < history.count(); _idx++)
		{
			history[_idx] = minmaxRect(history[_idx]);
		}
	}
	__inline void moveHistory(double _dx, double _dy)
	{
		for (int _idx = history.count() - 1; _idx > 0; _idx--)
		{
			history[_idx].moveTo(history[_idx].x() + _dx, history[_idx].y() + _dy);
			history[_idx] = adjustRect(history[_idx]);
		}
		if (!history.isEmpty())
		{
			QRectF _adj_rect;
			history[0].moveTo(history[0].x() + _dx, history[0].y());
			_adj_rect = adjustRect(history[0]);
			if (!intEqual(_adj_rect.width(), history[0].width()))
			{
				history[0].moveTo(history[0].x() - _dx, history[0].y());
			}
			history[0].moveTo(history[0].x(), history[0].y() + _dy);
			_adj_rect = adjustRect(history[0]);
			if (!intEqual(_adj_rect.height(), history[0].height()))
			{
				history[0].moveTo(history[0].x(), history[0].y() - _dy);
			}
		}
	}
};

jViewport::jViewport(): QObject(), d(new Data())
{
	setZoomOrientation(Qt::Vertical | Qt::Horizontal);
}

jViewport::~jViewport()
{
	delete d;
}

void jViewport::clearHistory()
{
	d->history.clear();
}

jViewport & jViewport::setBase(const QRectF & _rect)
{
	if (_rect.isValid() == false)
	{
		return * this;
	}
	clearHistory();
	if (d->maximum_size.width() <= 0 && d->maximum_size.height() <= 0)
	{
		setMaximumSize(_rect.size());
	}
	d->base = _rect;
	d->history << d->minmaxRect(_rect);
	QRectF _history_back = d->history.back();
	emit zoomedFullView(_history_back);
	return * this;
}

jViewport & jViewport::setBase(const jAxis & _x_axis, const jAxis & _y_axis)
{
	return setBase(QRectF(QPointF(_x_axis.lo(), _y_axis.lo()), QPointF(_x_axis.hi(), _y_axis.hi())));
}

QRectF jViewport::rectBase() const
{
	return d->base;
}

void jViewport::adjustBase(const QRectF & _rect)
{
	if (_rect.isValid() == false)
	{
		return;
	}
	if (_rect == d->base)
	{
		return;
	}
	d->base = _rect;
	d->history[0] = d->minmaxRect(_rect);
	for (int _idx = 1; _idx < d->history.count(); _idx++)
	{
		d->history[_idx] = d->adjustRect(d->history[_idx]);
	}
}

void jViewport::adjustBase(const jAxis & _x_axis, const jAxis & _y_axis)
{
	adjustBase(QRectF(QPointF(_x_axis.lo(), _y_axis.lo()), QPointF(_x_axis.hi(), _y_axis.hi())));
}

void jViewport::zoomIn(const QRectF & _rect)
{
	if (_rect.isValid() == false)
	{
		return;
	}
	if (d->history.isEmpty())
	{
		return;
	}
	QRectF _adj_rect = d->adjustRect(_rect);
	QVector<QRectF>::iterator _it = d->history.begin();
	for (; _it !=  d->history.end(); _it++)
	{
		if (fuzzyRectFCompare(* _it, _adj_rect))
		{
			break;
		}
	}
	if (d->history.count() >= 2 && fuzzyRectFCompare(* _it, * (d->history.end() - 2)))
	{
		d->history.erase(d->history.end() - 1);
		QRectF _history_back = d->history.back();
		emit zoomedOut(_history_back);
	}
	else if (_it == d->history.end() && !fuzzySizeFCompare(_adj_rect.size(), d->history.front().size()))
	{
		d->history << _adj_rect;
		QRectF _history_back = d->history.back();
		emit zoomedIn(_history_back);
	}
	else if (d->history.count() >= 1 && fuzzySizeFCompare(_adj_rect.size(), d->history.front().size()))
	{
		clearHistory();
		d->history << _adj_rect;
		QRectF _history_back = d->history.back();
		emit zoomedFullView(_history_back);
	}
	else
	{
		qDebug() << "zoomIn - what we gonna do?";
	}
}

void jViewport::zoomOut()
{
	if (d->history.count() > 1)
	{
		d->history.erase(d->history.end() - 1);
		QRectF _history_back = d->history.back();
		emit zoomedOut(_history_back);
	}
}

void jViewport::zoomFullView()
{
	d->history.erase(d->history.begin() + 1, d->history.end());
	QRectF _history_back = d->history.back();
	emit zoomedFullView(_history_back);
}

QRectF jViewport::rect() const
{
	return d->history.back();
}

QVector<QRectF> jViewport::history() const
{
	return d->history;
}

jSelector & jViewport::selector() const
{
	return d->selector;
}

void jViewport::pan(double _dx, double _dy)
{
	if (_dx == 0.0 && _dy == 0.0)
	{
		return;
	}
	QRectF _rect = d->history.back();
	if (_rect.left() + _dx < d->base.left())
	{
		_dx = d->base.left() - _rect.left();
	}
	if (_rect.top() + _dy < d->base.top())
	{
		_dy = d->base.top() - _rect.top();
	}
	if (_rect.right() + _dx > d->base.right())
	{
		_dx = d->base.right() - _rect.right();
	}
	if (_rect.bottom() + _dy > d->base.bottom())
	{
		_dy = d->base.bottom() - _rect.bottom();
	}
	d->moveHistory(_dx, _dy);
	QRectF _history_back = d->history.back();
	emit panned(_history_back);
}

jViewport & jViewport::setZoomOrientation(int _orientation)
{
	d->orientation = _orientation;
	return * this;
}

int jViewport::zoomOrientation() const
{
	return d->orientation;
}

QRectF jViewport::adjustRect(const QRectF & _rect, bool _to_orientation_only) const
{
	return d->adjustRect(_rect, _to_orientation_only);
}

jViewport & jViewport::setMinimumSize(const QSizeF & _size)
{
	d->minimum_size = _size;
	d->adjustHistory();
	return * this;
}

jViewport & jViewport::setMinimumWidth(float _width)
{
	d->minimum_size.setWidth(_width);
	d->adjustHistory();
	return * this;
}

jViewport & jViewport::setMinimumHeight(float _height)
{
	d->minimum_size.setHeight(_height);
	d->adjustHistory();
	return * this;
}

QSizeF jViewport::minimumSize() const
{
	return d->minimum_size;
}

jViewport & jViewport::setMaximumSize(const QSizeF & _size)
{
	d->maximum_size = _size;
	d->adjustHistory();
	return * this;
}

jViewport & jViewport::setMaximumWidth(float _width)
{
	d->maximum_size.setWidth(_width);
	d->adjustHistory();
	return * this;
}

jViewport & jViewport::setMaximumHeight(float _height)
{
	d->maximum_size.setHeight(_height);
	d->adjustHistory();
	return * this;
}

QSizeF jViewport::maximumSize() const
{
	return d->maximum_size;
}


// ------------------------------------------------------------------------

struct jLabel::Data
{	
	QString text;
	QPen pen;
	QFont font;
	QPointF pos;
	QSizeF size;
	bool visible;
	QTextOption options;
	QBrush background;
	bool auto_size;
	bool preview_enabled;
	Data()
	{
		visible = true;
		auto_size = true;
		preview_enabled = false;
	}
	~Data()
	{

	}
};

jLabel::jLabel(const QString & _text): d(new Data())
{
	setText(_text);
}

jLabel::~jLabel()
{
	delete d;
}

jLabel & jLabel::setText(const QString & _text)
{
	d->text = _text;
	if (d->auto_size)
	{
		setSize(sizeHint());
	}
	return * this;
}

QString jLabel::text() const
{
	return d->text;
}

jLabel & jLabel::setPen(const QPen & _pen)
{
	d->pen = _pen;
	return * this;
}

QPen jLabel::pen() const
{
	return d->pen;
}

jLabel & jLabel::setFont(const QFont & _font)
{
	d->font = _font;
	return * this;
}

QFont jLabel::font() const
{
	return d->font;
}

jLabel & jLabel::setPos(const QPointF & _pos)
{
	d->pos = _pos;
	return * this;
}

QPointF jLabel::pos() const
{
	return d->pos;
}

jLabel & jLabel::setSize(const QSizeF & _size)
{
	d->size = _size;
	return * this;
}

QSizeF jLabel::size() const
{
	return d->size;
}

jLabel & jLabel::setVisible(bool _state)
{
	d->visible = _state;
	return * this;
}

bool jLabel::isVisible() const
{
	return d->visible;
}

jLabel & jLabel::setOptions(const QTextOption & _options)
{
	d->options = _options;
	return * this;
}

QTextOption jLabel::options() const
{
	return d->options;
}

jLabel & jLabel::setBackground(const QBrush & _brush)
{
	d->background = _brush;
	return * this;
}

QBrush jLabel::background() const
{
	return d->background;
}

jLabel & jLabel::setAutoSize(bool _state)
{
	if (d->auto_size = _state)
	{
		setSize(sizeHint());
	}
	return * this;
}

bool jLabel::autoSize() const
{
	return (d->auto_size);
}

void jLabel::render(QPainter & _painter, const QRectF & _dst_rect, const QRectF & _src_rect)
{
	if (d->visible == false)
	{
		return;
	}

	QTransform _transform;
	if (::jQuadToQuad(_src_rect, _dst_rect, _transform))
	{
		QRectF _rect = QRectF(_transform.map(QPointF(d->pos.x(), d->pos.y())), d->size);
		_rect.moveTo(QPointF(_dst_rect.left() + _rect.left(), _dst_rect.top() + _rect.top() - _rect.height()));
		
		double _x = 0, _y = 0;
		if (_dst_rect.contains(_rect) == false)
		{
			if (_dst_rect.right() < _rect.right() && _dst_rect.left() <= _rect.left() - _rect.width())
			{
				_x -= d->size.width();
			}
			if (_dst_rect.top() > _rect.top() && _dst_rect.top() >= _rect.top() - _rect.height())
			{
				_y -= d->size.height();
			}
		}
		if (_x == 0)
		{
			_x += _dst_rect.left();
		}
		if (_y == 0)
		{
			_y += _dst_rect.top();
		}

	    QRectF _adj_src_rect = QRectF(QPointF(_src_rect.left(), - _src_rect.bottom()), _src_rect.size());
		if (::jQuadToQuad(_adj_src_rect, QRectF(_x, _y, _dst_rect.width(), _dst_rect.height()), _transform))
		{
			_rect = QRectF(_transform.map(QPointF(d->pos.x(), -d->pos.y())), d->size);
			_painter.setPen(d->pen);
			_painter.setFont(d->font);
			_painter.fillRect(_rect, d->background);
			_rect.moveLeft(_rect.left() + 4);
			_rect.moveTop(_rect.top() + 4);
			_painter.drawText(_rect, d->text, d->options);
		}
	}
}

void jLabel::renderPreview(QPainter & _painter, const QRectF & _dst_rect, const QRectF & _src_rect)
{
	if (d->preview_enabled)
	{
		render(_painter, _dst_rect, _src_rect);
	}
}

QSizeF jLabel::sizeHint() const
{
	double _w = 0, _h = 0;
	const QFontMetricsF _fm(d->font);
	const QString _text = d->text;
	foreach (const QString & _str, _text.split("\n"))
	{
		_w = qMax<double>(_w, _fm.width(_str)) + 8;
		_h += _fm.height();
	}
	_h += 8;
	return QSizeF(_w, _h);
}

jLabel & jLabel::setPreviewEnabled(bool _state)
{
	d->preview_enabled = _state;
	return * this;
}

bool jLabel::previewEnabled() const
{
	return d->preview_enabled;
}

// ------------------------------------------------------------------------

struct jCoordinator::Data
{
	jLabel label;
	jCoordinator::format_func format_func;
	jAxis::range_func range_func;
	QPointF pos;
	QPointF offset;
	Data()
	{
        pos = QPointF(-1e38, -1e38);
		offset = QPointF(8, 8);
		range_func = &jAxis::default_range_convert;
		format_func = &jCoordinator::default_format;	
	}
	~Data()
	{

	}
};

jCoordinator::jCoordinator(): d(new Data())
{

}

jCoordinator::~jCoordinator()
{
	delete d;
}

QString jCoordinator::default_format(double _x, double _y, jAxis * _x_axis, jAxis * _y_axis, jCoordinator *)
{
	QString _x_str = "x: " + (_x_axis ? _x_axis->rangeFunc()(_x, _x_axis) : QString::number(_x, 'f', 1));
	QString _y_str = "y: " + (_y_axis ? _y_axis->rangeFunc()(_y, _y_axis) : QString::number(_y, 'f', 1));
	return _x_str + "\n" + _y_str;
}

jCoordinator & jCoordinator::setFormat(jCoordinator::format_func _format_func)
{
	d->format_func = _format_func ? _format_func : &jCoordinator::default_format;
	return * this;
}

jCoordinator::format_func jCoordinator::formatFunc() const
{
	return d->format_func;
}

jLabel & jCoordinator::label() const
{
	return d->label;
}

jCoordinator & jCoordinator::setPos(const QPointF & _pos)
{
	d->pos = _pos;
	return * this;
}

QPointF jCoordinator::pos() const
{
	return d->pos;
}

jCoordinator & jCoordinator::setOffset(const QPointF & _offset)
{
	d->offset = _offset;
	return * this;
}

QPointF jCoordinator::offset() const
{
	return d->offset;
}

void jCoordinator::render(QPainter & _painter, const QRectF & _dst_rect, const QRectF & _src_rect, const jAxis * _x_axis, const jAxis * _y_axis)
{
	QPointF _pos = d->pos;
	QPointF _offset = d->offset;
	format_func _format_func = d->format_func;
	d->label.
		setText(_format_func(_pos.x(), _pos.y(), const_cast<jAxis *>(_x_axis), const_cast<jAxis *>(_y_axis), this)).
		setPos(_pos);

	d->label.
		render(_painter, _dst_rect, _src_rect);
}

// ------------------------------------------------------------------------

struct jItem::Data
{
	void * data;
	unsigned int width, height, bytes_per_item;
	bool deep_copy;
	QPen pen;
	bool visible;
	QBrush brush;
	QPointF origin;
	double z;
	QString tooltip;
	quint64 counter;
	jInputPattern pattern;
	QPointF press_point, release_point;
	jItemHandler * item_control;
	QImage symbol_img;
	bool preview_enabled;
	Data() 
	{
		preview_enabled = true;
		counter = 0;
		data = 0;
		width = 0;
		height = 0;
		bytes_per_item = 0;
		z = 0;
		deep_copy = false;
		visible = true;
		pattern.setEnabled(false);
		pattern.setDefaultPattern();
	}
	~Data()
	{
		clear();
	}
	__inline void clear()
	{
		if (data)
		{
			if (deep_copy)
			{
				delete [] (reinterpret_cast<char *>(data));
			}
			data = 0;
		}
	}
};

jItem::jItem(): d(new Data())
{
	d->item_control = new jItemHandler(this);
	QObject::connect(&d->pattern, SIGNAL(actionAccepted(int, int, int, int, QPointF, QWidget *)), d->item_control, SLOT(actionAccepted(int, int, int, int, QPointF, QWidget *)), Qt::DirectConnection);
}

jItem::~jItem()
{
	delete d->item_control;
	delete d;
}

jItem & jItem::setBytesPerItem(unsigned int _bytes_per_item)
{
	d->bytes_per_item = _bytes_per_item;
	return * this;
}

jItem & jItem::setData(void * _data, unsigned int _width, unsigned int _height, bool _deep_copy)
{
	if (_data == 0 || _width == 0 || _height == 0)
	{
		d->clear();
		d->deep_copy = false;
		d->width = 0;
		d->height = 0;
		return * this;
	}
	if (!_deep_copy)
	{
		if (d->deep_copy)
		{
			d->clear();
		}
		d->data = _data;
	}
	else
	{
		const unsigned int _data_size = _width * _height * d->bytes_per_item;
		if (_data_size != d->width * d->height * d->bytes_per_item || !d->deep_copy)
		{
			d->clear();
			d->data = new char [_data_size];
		}
		::memcpy(d->data, _data, _data_size);
	}
	d->width = _width;
	d->height = _height;
	d->deep_copy = _deep_copy;
	return * this;
}

const void * jItem::data() const
{
	return d->data;
}

QSize jItem::size() const
{
	return QSize(d->width, d->height);
}

unsigned int jItem::bytesPerItem() const
{
	return d->bytes_per_item;
}

bool jItem::isDeepCopy() const
{
	return d->deep_copy;
}

jItem & jItem::setPen(const QPen & _pen)
{
	d->pen = _pen;
	return * this;
}

QPen jItem::pen() const
{
	return d->pen;
}

jItem & jItem::setVisible(bool _state)
{
	d->visible = _state;
	return * this;
}

bool jItem::isVisible() const
{
	return d->visible;
}

jItem & jItem::setBrush(const QBrush & _brush)
{
	d->brush = _brush;
	return * this;
}

QBrush jItem::brush() const
{
	return d->brush;
}

jItem & jItem::setOrigin(const QPointF & _origin)
{
	d->origin = _origin;
	return * this;
}

QPointF jItem::origin() const
{
	return d->origin;
}

jItem & jItem::setZ(double _z)
{
	d->z = _z;
	return * this;
}

double jItem::z() const
{
	return d->z;
}

bool jItem::intersects(const QRectF & _rect, const jAxis * _x_axis, const jAxis * _y_axis) const
{
	return _rect.intersects(boundingRect(_x_axis, _y_axis));
}

jItem & jItem::setToolTip(const QString & _text)
{
	d->tooltip = _text;
	return * this;
}

QString jItem::toolTip() const
{
	return d->tooltip;
}

void jItem::updateViewport(const QRectF &)
{
}

void jItem::renderPreview(QPainter & _painter, const QRectF & _dst_rect, const QRectF & _src_rect, const jAxis * _x_axis, const jAxis * _y_axis)
{
	if (d->preview_enabled == false)
	{
		return;
	}
	QImage _saved_symbol_img = symbol();
	if (_saved_symbol_img.isNull())
	{
		setSymbol(QImage());
	}
	else if (_dst_rect.width() < _dst_rect.height())
	{
		setSymbol(_saved_symbol_img.scaledToWidth(_dst_rect.width()));
	}
	else
	{
		setSymbol(_saved_symbol_img.scaledToHeight(_dst_rect.height()));
	}
	render(_painter, _dst_rect, _src_rect, _x_axis, _y_axis);
	// return back symbol
	setSymbol(_saved_symbol_img);
}

quint64 jItem::counter() const
{
	return d->counter;
}

void jItem::addCounter(quint64 _count)
{
	d->counter = d->counter + _count;
}

QRectF jItem::boundingRect(const jAxis * _x_axis, const jAxis * _y_axis) const
{
	const QSizeF _size = size();
	return QRectF(d->origin, QSizeF(jAxis::normalizeToScale(_x_axis, _size.width()), jAxis::normalizeToScale(_y_axis, _size.height())));
}

jItem & jItem::setInputPattern(const jInputPattern & _pattern)
{
	d->pattern = _pattern;
	return * this;
}

jInputPattern & jItem::inputPattern() const
{
	return d->pattern;
}

jItemHandler * jItem::itemControl() const
{
	return d->item_control;
}

bool jItem::userCommand(int _action, int _method, int, int, QPointF _mpos, QWidget * _w) // jInputPattern::Action, jInputPattern::Method, buttons or key, modifiers or delta, mouse position
{
	if (_method == jInputPattern::MousePress)
	{
		d->press_point = _mpos;
	}
	else
	if (_method == jInputPattern::MouseRelease)
	{
		d->release_point = _mpos;
	}

	jView * _view = dynamic_cast<jView *>(_w);
	if (_view == 0)
	{
		return (false);
	}

	switch (_action)
	{
		case jInputPattern::ItemMenuRequested:
			{
				if ((d->press_point == d->release_point) || (_method == jInputPattern::KeyPress) || (_method == jInputPattern::KeyRelease))
				{
					_view->restoreCursorBeforePan();
					d->item_control->emitContextMenuRequested(_view->mapToGlobal(_mpos.toPoint()));
				}
			}
			break;
		default:
			return false;
	}   
	return (true);
}

jItem & jItem::setSymbol(const QImage & _img)
{
	d->symbol_img = _img;
	return * this;
}

QImage jItem::symbol() const
{
	return d->symbol_img;
}

jItem & jItem::setPreviewEnabled(bool _state)
{
	d->preview_enabled = _state;
	return * this;
}

bool jItem::previewEnabled() const
{
	return d->preview_enabled;
}

void jItem::clear()
{
	setData(0, 0, 0);
}


// ------------------------------------------------------------------------

struct jItemHandler::Data
{
	jItem * item;
	bool enabled;
	int from, to;
	QSize sens_area;
	Data()
	{
		from = jInputPattern::ItemActionGroupBegin;
		to = jInputPattern::ItemActionGroupEnd;
		sens_area = QSize(3, 3);
	}
	~Data()
	{

	}
};

jItemHandler::jItemHandler(jItem * _item): QObject(), d(new Data())
{
	d->item = _item;
}

jItemHandler::~jItemHandler()
{
	delete d;
}

jItemHandler & jItemHandler::setSensitiveArea(const QSize & _size)
{
	d->sens_area = _size;
	return (* this);
}

QSize jItemHandler::sensitiveArea() const
{
	return d->sens_area;
}

void jItemHandler::actionAccepted(int _action, int _method, int _buttons, int _modifier, QPointF _mpos, QWidget * _w) // jInputPattern::Action, jInputPattern::Method, buttons or key, modifiers or delta, mouse position
{
	if (!_w)
	{
		return;
	}
	jInputPattern * _pattern = dynamic_cast<jInputPattern *>(sender());
	if (_pattern == 0)
	{
		return;
	}
	if ((_action < d->from) || (_action > d->to))
	{
		return;
	}
	jView * _view = dynamic_cast<jView *>(_w);
	if (_view)
	{
		QRect _screen_rect(QPoint(), d->sens_area);
		_screen_rect.moveCenter(_mpos.toPoint());
		QRectF _rect = _view->screenToAxis(_screen_rect);
		if (d->item->intersects(_rect, _view->xAxis(), _view->yAxis()) == false)
		{
			return;
		}
	}
	jInputPattern::setAccepted(_pattern, d->item->userCommand(_action, _method, _buttons, _modifier, _mpos, _w));
}

jItem * jItemHandler::item() const
{
	return d->item;
}

jItemHandler & jItemHandler::setPatternFilter(int _from, int _to)
{
	d->from = qMin<int>(qMax<int>(_from, jInputPattern::ItemActionGroupBegin), jInputPattern::ItemActionGroupEnd);
	d->to = qMax<int>(qMin<int>(_to, jInputPattern::ItemActionGroupEnd), jInputPattern::ItemActionGroupBegin);
	if (d->from > d->to)
	{
		::qSwap(d->from, d->to);
	}
	return * this;
}

int jItemHandler::patternFilterFrom() const
{
	return d->from;
}

int jItemHandler::patternFilterTo() const
{
	return d->to;
}

void jItemHandler::emitContextMenuRequested(QPoint _pt)
{
	emit contextMenuRequested(_pt);
}

// ------------------------------------------------------------------------

struct jMarker::Data
{
	int orientation;
	double value;
	jItem1D<double> item;
	bool preview_enabled;
	Data()
	{
		value = 0.0;
		orientation = Qt::Vertical;
		preview_enabled = false;
	}
	~Data()
	{

	}
};

jMarker::jMarker(): d(new Data())
{

}

jMarker::~jMarker()
{
	delete d;
}

jMarker & jMarker::setPen(const QPen & _pen)
{
	d->item.setPen(_pen);
	return * this;
}

QPen jMarker::pen() const
{
	return d->item.pen();
}

jMarker & jMarker::setValue(double _value)
{
	d->value = _value;
	return * this;
}

inline double jMarker::value() const
{
	return d->value;
}

jMarker & jMarker::setVisible(bool _state)
{
	d->item.setVisible(_state);
	return * this;
}

bool jMarker::isVisible() const
{
	return d->item.isVisible();
}

jMarker & jMarker::setOrientation(int _orientation)
{
	d->orientation = _orientation;
	return * this;
}

int jMarker::orientation() const
{
	return d->orientation;
}

void jMarker::render(QPainter & _painter, const QRectF & _dst_rect, const QRectF & _src_rect)
{
	if (isVisible() == false)
	{
		return;
	}
	double _value = d->value;
	switch (d->orientation)
	{
	case Qt::Vertical:
		{
			d->item.setData(QLineF(_value, _src_rect.top(), _value, _src_rect.bottom()));
			break;
		}
	case Qt::Horizontal:
		{
			d->item.setData(QLineF(_src_rect.left(), _value, _src_rect.right(), _value));
			break;
		}
	}
	d->item.render(_painter, _dst_rect, _src_rect);
}

void jMarker::renderPreview(QPainter & _painter, const QRectF & _dst_rect, const QRectF & _src_rect)
{
	if (d->preview_enabled)
	{
		render(_painter, _dst_rect, _src_rect);
	}
}

jMarker & jMarker::setPreviewEnabled(bool _state)
{
	d->preview_enabled = _state;
	return * this;
}

bool jMarker::previewEnabled() const
{
	return d->preview_enabled;
}

jItem & jMarker::internalItem() const
{
	return d->item;
}

// ------------------------------------------------------------------------

struct jInputPattern::Data
{
	struct ActionEntry
	{
		int action, method, code, modifier;
		bool operator == (const ActionEntry & _other) const
		{
			return (action == _other.action) && (method == _other.method) && (code == _other.code) && ((modifier & 0x7f000000) == (_other.modifier & 0x7f000000));
		}
		bool operator <(const ActionEntry & _other) const
		{
			return (action < _other.action);
		}
	};
	QMap<int, QVector<ActionEntry> > actions;
	QPointF last_mouse_pos;
	int last_btn, last_key, last_modifier;
	bool enabled;
	Data()
	{
		last_btn = 0;
		last_key = 0;
		last_modifier = 0;
		enabled = true;
	}
	~Data()
	{
	}
	__inline void setDefaultPattern(jInputPattern * _pattern)
	{
		if (_pattern == 0)
		{
			return;
		}
		_pattern->clear();
		_pattern->
			addAction(jInputPattern::MoveCursorLeft, jInputPattern::KeyPress, Qt::Key_Left).
			addAction(jInputPattern::MoveCursorRight, jInputPattern::KeyPress, Qt::Key_Right).
			addAction(jInputPattern::MoveCursorUp, jInputPattern::KeyPress, Qt::Key_Up).
			addAction(jInputPattern::MoveCursorDown, jInputPattern::KeyPress, Qt::Key_Down).
			addAction(jInputPattern::ContextMenuRequested, jInputPattern::MouseRelease, Qt::RightButton).
			addAction(jInputPattern::ZoomStart, jInputPattern::MousePress, Qt::LeftButton).
			addAction(jInputPattern::ZoomEnd, jInputPattern::MouseRelease, Qt::LeftButton).
			addAction(jInputPattern::ZoomMove, jInputPattern::MouseMove, Qt::LeftButton).
			addAction(jInputPattern::ZoomDeltaVertical, jInputPattern::WheelVertical).
			addAction(jInputPattern::ZoomDeltaHorizontal, jInputPattern::WheelHorizontal).
			addAction(jInputPattern::ZoomFullView, jInputPattern::MouseDoubleClick, Qt::LeftButton).
			addAction(jInputPattern::PanStart, jInputPattern::MousePress, Qt::RightButton).
			addAction(jInputPattern::PanEnd, jInputPattern::MouseRelease, Qt::RightButton).
			addAction(jInputPattern::PanMove, jInputPattern::MouseMove, Qt::RightButton).
			addAction(jInputPattern::PreviewPanStart, jInputPattern::MousePress, Qt::LeftButton).
			addAction(jInputPattern::PreviewPanEnd, jInputPattern::MouseRelease, Qt::LeftButton).
			addAction(jInputPattern::PreviewPanMove, jInputPattern::MouseMove, Qt::LeftButton).
			addAction(jInputPattern::PreviewDeltaVertical, jInputPattern::WheelVertical).
			addAction(jInputPattern::PreviewDeltaHorizontal, jInputPattern::WheelHorizontal).
			addAction(jInputPattern::PreviewFullView, jInputPattern::MouseDoubleClick, Qt::LeftButton).
			addAction(jInputPattern::MoveItemLeft, jInputPattern::KeyPress, Qt::Key_Left).
			addAction(jInputPattern::MoveItemRight, jInputPattern::KeyPress, Qt::Key_Right).
			addAction(jInputPattern::MoveItemUp, jInputPattern::KeyPress, Qt::Key_Up).
			addAction(jInputPattern::MoveItemDown, jInputPattern::KeyPress, Qt::Key_Down).
			addAction(jInputPattern::ItemPanStart, jInputPattern::MousePress, Qt::RightButton).
			addAction(jInputPattern::ItemPanEnd, jInputPattern::MouseRelease, Qt::RightButton).
			addAction(jInputPattern::ItemPanMove, jInputPattern::MouseMove, Qt::RightButton).
            addAction(jInputPattern::ItemMenuRequested, jInputPattern::MouseRelease, Qt::RightButton).
            addAction(jInputPattern::ItemSelected, jInputPattern::MousePress, Qt::LeftButton);
	}
	__inline void addEntry(int _action, int _method, int _code, int _modifier)
	{
		ActionEntry _entry;
		_entry.action = _action;
		_entry.method = _method;
		_entry.code = _code;
		_entry.modifier = (_modifier & 0x7f000000);
		QVector<ActionEntry>::iterator _it = ::qFind(actions[_entry.action].begin(), actions[_entry.action].end(), _entry);
		if (_it != actions[_entry.action].end())
		{
			return;
		}
		actions[_entry.action] << _entry;
	}
	__inline void removeEntry(int _action, int _method)
	{
		ActionEntry _entry;
		_entry.action = _action;
		_entry.method = _method;
		for (QVector<ActionEntry>::iterator _it = actions[_entry.action].begin(); _it != actions[_entry.action].end(); )
		{
			if (((*_it).action == _entry.action) && ((*_it).method == _entry.method))
			{
				actions[_entry.action].erase(_it);
			}
			else
			{
				_it++;
			}
		}
	}
	__inline void removeEntry(int _action, int _method, int _code)
	{
		ActionEntry _entry;
		_entry.action = _action;
		_entry.method = _method;
		_entry.code = _code;
		for (QVector<ActionEntry>::iterator _it = actions[_entry.action].begin(); _it != actions[_entry.action].end(); )
		{
			if (((*_it).action == _entry.action) && ((*_it).method == _entry.method) && ((*_it).code == _entry.code))
			{
				actions[_entry.action].erase(_it);
			}
			else
			{
				_it++;
			}
		}
	}
	__inline void removeEntry(int _action, int _method, int _code, int _modifier)
	{
		ActionEntry _entry;
		_entry.action = _action;
		_entry.method = _method;
		_entry.code = _code;
		_entry.modifier = _modifier;
		for (QVector<ActionEntry>::iterator _it = actions[_entry.action].begin(); _it != actions[_entry.action].end(); )
		{
			if (((*_it).action == _entry.action) && ((*_it).method == _entry.method) && ((*_it).code == _entry.code) && (((*_it).modifier & 0x7f000000) == (_entry.modifier & 0x7f000000)))
			{
				actions[_entry.action].erase(_it);
			}
			else
			{
				_it++;
			}
		}
	}
	__inline void removeEntries(int _action)
	{
		actions.remove(_action);
	}
	__inline QVector<int> methods(int _action) const
	{
		QVector<int> _methods;
		foreach (int _tmp_action, actions.keys())
		{
			if (_tmp_action == _action)
			{
				foreach (ActionEntry _entry, actions[_tmp_action])
				{
					_methods << _entry.method;
				}
			}
		}
		return _methods;
	}
	__inline void codes(int _action, int _method, QVector<int> & _codes, QVector<int> & _modifiers) const
	{
		foreach (int _tmp_action, actions.keys())
		{
			if (_tmp_action == _action)
			{
				foreach (ActionEntry _entry, actions[_tmp_action])
				{
					if (_entry.method == _method)
					{
						_codes << _entry.code;
						_modifiers << _entry.modifier;
					}
				}
			}
		}
	}
	__inline QVector<ActionEntry> checkAction(int _method, int _code = 0, int _modifier = 0)
	{
		QVector<ActionEntry> _accepted;
		foreach (int _tmp_action, actions.keys())
		{
			foreach (ActionEntry _entry, actions[_tmp_action])
			{
				if ((_entry.method == _method) && (_entry.code == _code) && ((_entry.modifier & 0x7f000000) == (_modifier & 0x7f000000)))
				{
					_accepted << _entry;
				}
			}
		}
		::qSort(_accepted.begin(), _accepted.end()); 
		return _accepted;
	}
	__inline static bool actionsForwardSort(const ActionEntry & _entry1, const ActionEntry & _entry2)
	{
		return (_entry1.action < _entry2.action);
	}
	__inline static bool actionsBackwardSort(const ActionEntry & _entry1, const ActionEntry & _entry2)
	{
		return (_entry1.action > _entry2.action);
	}
};

jInputPattern::jInputPattern(QObject * _parent): QObject(_parent), d(new Data())
{
}

jInputPattern::jInputPattern(const jInputPattern & _other, QObject * _parent): QObject(_parent), d(new Data())
{
	d->actions = _other.d->actions;
	d->last_btn = _other.d->last_btn;
	d->last_key = _other.d->last_key;
	d->last_modifier = _other.d->last_modifier;
	d->last_mouse_pos = _other.d->last_mouse_pos;
}

jInputPattern & jInputPattern::operator = (const jInputPattern & _other)
{
	if (&_other != this)
	{
		d->actions = _other.d->actions;
		d->last_btn = _other.d->last_btn;
		d->last_key = _other.d->last_key;
		d->last_modifier = _other.d->last_modifier;
		d->last_mouse_pos = _other.d->last_mouse_pos;
	}
	return * this;
}

jInputPattern::~jInputPattern()
{
	delete d;
}

jInputPattern & jInputPattern::setDefaultPattern()
{
	d->setDefaultPattern(this);
	return * this;
}

jInputPattern & jInputPattern::addAction(int _action, int _method, int _code, int _modifier)
{
	d->addEntry(_action, _method, _code, _modifier);
	return * this;
}

jInputPattern & jInputPattern::removeAction(int _action, int _method)
{
	d->removeEntry(_action, _method);
	return * this;
}

jInputPattern & jInputPattern::removeAction(int _action, int _method, int _code)
{
	d->removeEntry(_action, _method, _code);
	return * this;
}

jInputPattern & jInputPattern::removeAction(int _action, int _method, int _code, int _modifier)
{
	d->removeEntry(_action, _method, _code, _modifier);
	return * this;
}

jInputPattern & jInputPattern::removeAction(int _action)
{
	d->removeEntries(_action);
	return * this;
}

void jInputPattern::clear()
{
	d->actions.clear();
}

QVector<int> jInputPattern::actions() const
{
	return d->actions.keys().toVector();
}

QVector<int> jInputPattern::actionMethods(int _action) const
{
	return d->methods(_action);
}

QVector<int> jInputPattern::actionCodes(int _action, int _method, QVector<int> & _modifiers) const
{
	QVector<int> _codes;
	d->codes(_action, _method, _codes, _modifiers);
	return _codes;
}

QVector<int> jInputPattern::actionModifiers(int _action, int _method) const
{
	QVector<int> _codes, _modifiers;
	d->codes(_action, _method, _codes, _modifiers);
	return _modifiers;
}

QPointF jInputPattern::lastMousePosition() const
{
	return d->last_mouse_pos;
}

int jInputPattern::lastMouseButtons() const
{
	return d->last_btn;
}

int jInputPattern::lastKeyboardKey() const
{
	return d->last_key;
}

int jInputPattern::lastModifiers() const
{
	return d->last_modifier;
}

int jInputPattern::lastDelta() const
{
	return d->last_modifier;
}

jInputPattern & jInputPattern::setEnabled(bool _state)
{
	d->enabled = _state;
	return * this;
}

bool jInputPattern::isEnabled() const
{
	return d->enabled;
}

bool jInputPattern::eventFilter(QObject * _object, QEvent * _event)
{
	if (d->enabled == false)
	{
		return false;
	}
	QWidget * _widget = dynamic_cast<QWidget *>(_object);
	if (_widget && !_widget->isEnabled())
	{
		return false;
	}
	QMouseEvent *	_me	= 0;
	QKeyEvent *	_ke	= 0;
	QWheelEvent *	_we	= 0;
	int _method = UnknownMethod;
	QVector<Data::ActionEntry> _actions;
	int _code = 0;
	int _modifier = 0;
	int _delta = 0;
	QPointF _mpos = QPoint(0, 0);
	switch (_event->type())
	{
	case QEvent::MouseButtonPress:
		_me = dynamic_cast<QMouseEvent *>(_event);
		_method = MousePress;
		_code = _me->buttons();
		if (_code == Qt::NoButton)
		{
			_code = _me->button();
		}
		_modifier = _me->modifiers();
		_mpos = _me->posF();
		_actions = d->checkAction(_method, _code, _modifier);
		break;
	case QEvent::MouseButtonRelease:
		_me = dynamic_cast<QMouseEvent *>(_event);
		_method = MouseRelease;
		_code = _me->buttons();
		if (_code == Qt::NoButton)
		{
			_code = _me->button();
		}
		_modifier = _me->modifiers();
		_mpos = _me->posF();
		_actions = d->checkAction(_method, _code, _modifier);
		break;
	case QEvent::MouseButtonDblClick:
		_me = dynamic_cast<QMouseEvent *>(_event);
		_method = MouseDoubleClick;
		_code = _me->buttons();
		_modifier = _me->modifiers();
		_mpos = _me->posF();
		_actions = d->checkAction(_method, _code, _modifier);
		break;
	case QEvent::KeyPress:
		_ke = dynamic_cast<QKeyEvent *>(_event);
		_method = KeyPress;
		_code = _ke->key();
		_modifier = _ke->modifiers();
		_mpos = dynamic_cast<QWidget *>(_object)->mapFromGlobal(QCursor::pos());
		_actions = d->checkAction(_method, _code, _modifier);
		break;
	case QEvent::KeyRelease:
		_ke = dynamic_cast<QKeyEvent *>(_event);
		_method = KeyRelease;
		_code = _ke->key();
		_modifier = _ke->modifiers();
		_mpos = dynamic_cast<QWidget *>(_object)->mapFromGlobal(QCursor::pos());
		_actions = d->checkAction(_method, _code, _modifier);
		break;
	case QEvent::Wheel:
		_we = dynamic_cast<QWheelEvent *>(_event);
		_method = (_we->orientation() == Qt::Vertical) ? WheelVertical : WheelHorizontal;
		_code = _we->buttons();
		_delta = _we->delta();
		_modifier = _we->modifiers();
		_modifier = ((quint16)_delta) | _modifier;
		_mpos = _we->pos();
		_actions = d->checkAction(_method, _code, _modifier & 0x7f000000);
		break;
	case QEvent::MouseMove:
		_me = dynamic_cast<QMouseEvent *>(_event);
		_method = MouseMove;
		_code = _me->buttons();
		_modifier = _me->modifiers();
		_mpos = _me->posF();
		_actions = d->checkAction(_method, _code, _modifier);
		break;
	}
	d->last_mouse_pos = _mpos;
	d->last_btn = _code;
	d->last_modifier = _modifier;
	d->last_key = _code;
	if (_actions.count())
	{
        bool _accepted = false;
		qSort(_actions.begin(), _actions.end(), & Data::actionsForwardSort);
		foreach (Data::ActionEntry _entry, _actions)
		{
			setProperty("accepted", false);
			emit actionAccepted(_entry.action, _method, _code, _modifier, _mpos, _widget);
            _accepted = _accepted || property("accepted").toBool();
		}
        if (_accepted) // first processed are items, cause they installed as events last, so we can exit
		{
			return true;
		}
	}
	return _object->eventFilter(_object, _event);
}

void jInputPattern::setAccepted(QObject * _obj, bool _state)
{
	jInputPattern * _pattern = dynamic_cast<jInputPattern *>(_obj);
	if (_pattern == 0)
	{
		return;
	}
	_pattern->setProperty("accepted", _state);
}

void jInputPattern::accepted(QObject * _obj)
{
	setAccepted(_obj, true);
}

void jInputPattern::rejected(QObject * _obj)
{
	setAccepted(_obj, false);
}


// ------------------------------------------------------------------------

struct jView::Data
{
	jAxis * x_axis, * y_axis;
	bool x_axis_vis_ovr, y_axis_vis_ovr;
	bool internal_x_axis, internal_y_axis;
	jViewport viewport;
	jCoordinator coordinator;
	jMarker hmarker, vmarker;
	jStack<bool> coordinator_visibility, hmarker_visibility, vmarker_visibility;
	QVector<jItem *> items;
	QVector<jLabel *> labels;
	QVector<jMarker *> markers;
	QVector<jSelector *> selectors;
	QBrush background;
	QPointF press_point, release_point, move_point;
    bool in_zoom, draw_grid;
	QCursor default_cursor;
	jRenderer * renderer;
	jInputPattern pattern;
    QRect widget_rect;
	int axes_plane;
	Data()
	{
		pattern.setDefaultPattern();
		axes_plane = jView::AxesInForeplane;
		x_axis = new jAxis();
		y_axis = new jAxis();
		internal_x_axis = true;
		internal_y_axis = true;
		in_zoom = false;
        draw_grid = false;
		x_axis_vis_ovr = true;
		y_axis_vis_ovr = true;
		coordinator.label().
			setVisible(true);
		hmarker.
			setVisible(false).
			setOrientation(Qt::Horizontal);
		vmarker.
			setVisible(false).
			setOrientation(Qt::Vertical);
	}
	~Data()
	{
		if (internal_x_axis)
		{
			delete x_axis;
		}
		if (internal_y_axis)
		{
			delete y_axis;
		}
	}
	__inline QTransform screenToAxisTransform(const QRectF & _screen_rect) const
	{
		QTransform _transform;
		const QRectF & _viewport_rect = viewport.rect();
		QPolygonF _viewport_poly;
		_viewport_poly << QPointF(_viewport_rect.left(), _viewport_rect.bottom());
		_viewport_poly << QPointF(_viewport_rect.right(), _viewport_rect.bottom());
		_viewport_poly << QPointF(_viewport_rect.right(), _viewport_rect.top());
		_viewport_poly << QPointF(_viewport_rect.left(), _viewport_rect.top());
		QTransform::quadToQuad(QPolygonF(_screen_rect).mid(0, 4), _viewport_poly, _transform);
		return _transform;
	}
	__inline QRectF screenToAxis(const QRectF & _screen_rect, const QRectF & _src_rect) const
	{
		return screenToAxisTransform(_screen_rect).mapRect(_src_rect);
	}
	__inline QPointF screenToAxis(const QRectF & _screen_rect, const QPointF & _src_point) const
	{
		return screenToAxisTransform(_screen_rect).map(_src_point);
	}
	__inline QTransform axisToScreenTransform(const QRectF & _screen_rect) const
	{
		QTransform _transform;
		QPolygonF _screen_poly;
		_screen_poly << QPointF(_screen_rect.left(), _screen_rect.bottom());
		_screen_poly << QPointF(_screen_rect.right(), _screen_rect.bottom());
		_screen_poly << QPointF(_screen_rect.right(), _screen_rect.top());
		_screen_poly << QPointF(_screen_rect.left(), _screen_rect.top());
		QTransform::quadToQuad(QPolygonF(viewport.rect()).mid(0, 4), _screen_poly, _transform);
		return _transform;
	}
	__inline QRectF axisToScreen(const QRectF & _src_rect, const QRectF & _screen_rect) const
	{
		return axisToScreenTransform(_screen_rect).mapRect(_src_rect);
	}
	__inline QPointF axisToScreen(const QPointF & _src_point, const QRectF & _screen_rect) const
	{
		return axisToScreenTransform(_screen_rect).map(_src_point);
	}
	__inline void setBase()
	{
		viewport.setBase(* x_axis, * y_axis);
	}
	__inline static bool itemZSort(const jItem * _item1, const jItem * _item2)
	{
		if (_item1 && _item2)
		{
			return (_item1->z() < _item2->z());
		}
		return false;
	}
	__inline static bool markerZSort(const jMarker * _marker1, const jMarker * _marker2)
	{
		if (_marker1 && _marker2)
		{
			return (_marker1->internalItem().z() < _marker2->internalItem().z());
		}
		return false;
	}
	__inline static bool selectorZSort(const jSelector * _selector1, const jSelector * _selector2)
	{
		if (_selector1 && _selector2)
		{
			return (_selector1->internalItem().z() < _selector2->internalItem().z());
		}
		return false;
	}
	__inline static bool itemZSortReversed(const jItem * _item1, const jItem * _item2)
	{
		if (_item1 && _item2)
		{
			return (_item1->z() > _item2->z());
		}
		return false;
	}
	__inline static bool markerZSortReversed(const jMarker * _marker1, const jMarker * _marker2)
	{
		if (_marker1 && _marker2)
		{
			return (_marker1->internalItem().z() > _marker2->internalItem().z());
		}
		return false;
	}
	__inline static bool selectorZSortReversed(const jSelector * _selector1, const jSelector * _selector2)
	{
		if (_selector1 && _selector2)
		{
			return (_selector1->internalItem().z() > _selector2->internalItem().z());
		}
		return false;
	}
	__inline void adjustCoordinator(const QRectF & _screen_rect, const QPointF & _local_pt)
	{
		QPointF _axis_pt = screenToAxis(_screen_rect, _local_pt);
		if (!intEqual(hmarker.value(), _axis_pt.y()))
		{
			hmarker.setValue(_axis_pt.y());
		}
		if (!intEqual(vmarker.value(), _axis_pt.x()))
		{
			vmarker.setValue(_axis_pt.x());
		}
		if (coordinator.pos() != _axis_pt)
		{
			coordinator.setPos(_axis_pt);
		}
	}
	__inline void updateViewports(const QRectF & _rect)
	{
		foreach (jItem * _item, items)
		{
			_item->updateViewport(_rect);
		}
	}
	__inline static void render_func(QWidget * _widget, QPainter & _painter)
	{
		dynamic_cast<jView *>(_widget)->render(_painter);
	}
	__inline void init(QWidget * _instance)
	{
		viewport.setParent(_instance);

		QObject::connect(&viewport, SIGNAL(zoomedIn(QRectF)), _instance, SIGNAL(viewportChanged(QRectF)));
		QObject::connect(&viewport, SIGNAL(zoomedOut(QRectF)), _instance, SIGNAL(viewportChanged(QRectF)));
		QObject::connect(&viewport, SIGNAL(zoomedFullView(QRectF)), _instance, SIGNAL(viewportChanged(QRectF)));
		QObject::connect(&viewport, SIGNAL(panned(QRectF)), _instance, SIGNAL(viewportChanged(QRectF)));

		renderer = new jRenderer(_instance, &Data::render_func);
		_instance->installEventFilter(renderer);

		_instance->setMouseTracking(true);
		_instance->setCursor(Qt::CrossCursor);
		_instance->setFocusPolicy(Qt::WheelFocus);

		_instance->installEventFilter(&pattern);
	}
};

jView::jView(QWidget * _parent)
	: JUNGLE_WIDGET_CLASS(_parent), d(new Data())
{
	connect(&d->pattern, SIGNAL(actionAccepted(int, int, int, int, QPointF, QWidget *)), this, SLOT(actionAccepted(int, int, int, int, QPointF, QWidget *)), Qt::DirectConnection);
	d->init(this);
}

jView::jView(jAxis * _x_axis, jAxis * _y_axis, QWidget * _parent)
	: JUNGLE_WIDGET_CLASS(_parent), d(new Data())
{
	connect(&d->pattern, SIGNAL(actionAccepted(int, int, int, int, QPointF, QWidget *)), this, SLOT(actionAccepted(int, int, int, int, QPointF, QWidget *)), Qt::DirectConnection);
	d->init(this);

	d->internal_x_axis = false;
	d->internal_y_axis = false;
	this->
		setXAxis(_x_axis).
		setYAxis(_y_axis);
}

jView::jView(const jAxis & _x_axis, const jAxis & _y_axis, QWidget * _parent)
	: JUNGLE_WIDGET_CLASS(_parent), d(new Data())
{
	connect(&d->pattern, SIGNAL(actionAccepted(int, int, int, int, QPointF, QWidget *)), this, SLOT(actionAccepted(int, int, int, int, QPointF, QWidget *)), Qt::DirectConnection);
	d->init(this);
	delete d->x_axis;
	delete d->y_axis;

	this->
		setXAxis(new jAxis(_x_axis)).
		setYAxis(new jAxis(_y_axis));
}

jView::~jView()
{
	setVisible(false);
	d->viewport.setParent(0);
	removeEventFilter(d->renderer);
	delete d->renderer;
	delete d;
}

jView & jView::setXAxis(jAxis * _axis)
{
	if (d->x_axis != _axis)
	{
		if (d->internal_x_axis)
		{
			delete d->x_axis;
		}
		if (_axis)
		{
			d->internal_x_axis = false;
			d->x_axis = _axis;
		}
		else
		{
			d->internal_x_axis = true;
			d->x_axis = new jAxis();
		}
	}
	d->setBase();
	return * this;
}

jAxis * jView::xAxis() const
{
	return d->x_axis;
}

jView & jView::setYAxis(jAxis * _axis)
{
	if (d->y_axis != _axis)
	{
		if (d->internal_y_axis)
		{
			delete d->y_axis;
		}
		if (_axis)
		{
			d->internal_y_axis = false;
			d->y_axis = _axis;
		}
		else
		{
			d->internal_y_axis = true;
			d->y_axis = new jAxis();
		}
	}
	d->setBase();
	return * this;
}

jAxis * jView::yAxis() const
{
	return d->y_axis;
}

jView & jView::setXAxisVisibleOverride(bool _state)
{
	d->x_axis_vis_ovr = _state;
	return * this;
}

jView & jView::setYAxisVisibleOverride(bool _state)
{
	d->y_axis_vis_ovr = _state;
	return * this;
}

bool jView::isXAxisVisibleOverride() const
{
	return d->x_axis_vis_ovr;
}

bool jView::isYAxisVisibleOverride() const
{
	return d->y_axis_vis_ovr;
}

bool jView::isXAxisVisible() const
{
	return (d->x_axis_vis_ovr) && d->x_axis->isVisible();
}

bool jView::isYAxisVisible() const
{
	return (d->y_axis_vis_ovr) && d->y_axis->isVisible();
}

jView & jView::setGridEnabled(bool _draw_grid)
{
	d->draw_grid = _draw_grid;
	return * this;
}

bool jView::gridEnabled() const
{
    return d->draw_grid;
}

jView & jView::setAxesPlane(int _plane)
{
	d->axes_plane = _plane;
	return * this;
}

int jView::axesPlane() const
{
	return d->axes_plane;
}

inline jViewport & jView::viewport() const
{
	return d->viewport;
}

jView & jView::addItem(jItem * _item)
{
	d->items << _item;
	installEventFilter(& d->items.back()->inputPattern());
	::qSort(d->items.begin(), d->items.end(), &Data::itemZSort);
	return * this;
}

jView & jView::addItems(const QVector<jItem *> & _items)
{
	d->items << _items;
	foreach (jItem * _item, _items)
	{
		installEventFilter(& _item->inputPattern());
	}
	::qSort(d->items.begin(), d->items.end(), &Data::itemZSort);
	return * this;
}

jView & jView::setItem(jItem * _item)
{
	d->items = QVector<jItem *>() << _item;
	installEventFilter(& d->items.back()->inputPattern());
	::qSort(d->items.begin(), d->items.end(), &Data::itemZSort);
	return * this;
}

jView & jView::setItems(const QVector<jItem *> & _items)
{
	d->items = _items;
	foreach (jItem * _item, _items)
	{
		installEventFilter(& _item->inputPattern());
	}
	::qSort(d->items.begin(), d->items.end(), &Data::itemZSort);
	return * this;
}

jView & jView::removeItem(jItem * _item)
{
	QVector<jItem *>::iterator _it = ::qFind(d->items.begin(), d->items.end(), _item);
	if (_it != d->items.end())
	{
		removeEventFilter(& _item->inputPattern());
		d->items.erase(_it);
	}
	::qSort(d->items.begin(), d->items.end(), &Data::itemZSort);
	return * this;
}

jView & jView::removeItems(const QVector<jItem *> & _items)
{
	foreach (jItem * _item, _items)
	{
		QVector<jItem *>::iterator _it = ::qFind(d->items.begin(), d->items.end(), _item);
		if (_it != d->items.end())
		{
			removeEventFilter(& _item->inputPattern());
			d->items.erase(_it);
		}
	}
	::qSort(d->items.begin(), d->items.end(), &Data::itemZSort);
	return * this;
}

void jView::clear()
{
	for (int _idx = 0; _idx < d->items.count(); _idx++)
	{
		removeEventFilter(& d->items[_idx]->inputPattern());
	}
	d->items.clear();
	d->labels.clear();
	d->selectors.clear();
	d->markers.clear();
}

QVector<jItem *> jView::items() const
{
	return d->items;
}

jView & jView::setBackground(const QBrush & _brush)
{
	d->background = _brush;
	return * this;
}

QBrush jView::background() const
{
	return d->background;
}

void jView::render(QPainter & _painter) const
{
	QVector<jItem *> _items = d->items;
	::qSort(_items.begin(), _items.end(), &Data::itemZSort);
    QRectF _rect = d->widget_rect;
	QRectF _viewport_rect = d->viewport.rect();
	jAxis * _x_axis = d->x_axis;
	jAxis * _y_axis = d->y_axis;
	QVector<jMarker *> _markers = d->markers;
	::qSort(_markers.begin(), _markers.end(), &Data::markerZSort);
	QVector<jLabel *> _labels = d->labels;
	QVector<jSelector *> _selectors = d->selectors;
	::qSort(_selectors.begin(), _selectors.end(), &Data::selectorZSort);
	if (d->background.style() != Qt::NoBrush)
	{
		_painter.fillRect(_rect, d->background);
	}
	if (d->axes_plane == AxesInBackplane)
	{
		if (d->x_axis_vis_ovr)
		{
			_x_axis->render(
				_painter, 
				_rect, 
				Qt::Horizontal,
				_viewport_rect.left(),
							   _viewport_rect.right(),
							   d->draw_grid
					);
		}
		if (d->y_axis_vis_ovr)
		{
			_y_axis->render(
				_painter, 
				_rect, 
				Qt::Vertical,
				_viewport_rect.top(),
							   _viewport_rect.bottom(),
							   d->draw_grid
					);
		}
	}
	foreach (jItem * _item, _items)
    {
		_item->render(_painter, _rect, _viewport_rect, _x_axis, _y_axis);
	}
	foreach (jSelector * _selector, _selectors)
	{
		_selector->render(_painter, _rect, _viewport_rect);
	}
	if (d->axes_plane == AxesInForeplane)
	{
		if (d->x_axis_vis_ovr)
		{
			_x_axis->render(
				_painter, 
				_rect, 
				Qt::Horizontal,
				_viewport_rect.left(),
							   _viewport_rect.right(),
							   d->draw_grid
					);
		}
		if (d->y_axis_vis_ovr)
		{
			_y_axis->render(
				_painter, 
				_rect, 
				Qt::Vertical,
				_viewport_rect.top(),
							   _viewport_rect.bottom(),
							   d->draw_grid
					);
		}
	}
	foreach (jMarker * _marker, _markers)
	{
		_marker->render(_painter, _rect, _viewport_rect);
	}
	d->hmarker.render(_painter, _rect, _viewport_rect);
	d->vmarker.render(_painter, _rect, _viewport_rect);
	foreach (jLabel * _label, _labels)
	{
		_label->render(_painter, _rect, _viewport_rect);
	}
	if (d->in_zoom)
	{
		d->viewport.selector().render(_painter, _rect, _viewport_rect);
	}
	d->coordinator.render(_painter, _rect, _viewport_rect, d->x_axis, d->y_axis);
}

void jView::actionAccepted(int _action, int _method, int _code, int _modifier, QPointF _mpos, QWidget * _w)
{
	if (_w != this)
	{
		return;
	}
	if ((_action < jInputPattern::WidgetActionGroupBegin) || (_action > jInputPattern::WidgetActionGroupEnd))
	{
		return;
	}
	jInputPattern::setAccepted(& d->pattern, userCommand(_action, _method, _code, _modifier, _mpos, _w));
}

bool jView::userCommand(int _action, int _method, int /*_code*/, int _modifier, QPointF _mpos, QWidget * /*_w*/)
{
	if (_method == jInputPattern::MousePress)
	{
		d->press_point = _mpos;
	}
	else
	if (_method == jInputPattern::MouseRelease)
	{
		d->release_point = _mpos;
	}

	const QCursor & _cursor = cursor();

	switch (_action)
	{
	case jInputPattern::MoveCursorLeft:
		{
			const QPoint & _pt_current = QCursor::pos();
			QPoint _pt_new = _pt_current;

			_pt_new.setX(_pt_current.x() - 1);

			if (rect().contains(mapFromGlobal(_pt_new)))
			{
				QCursor::setPos(_pt_new);
			}
			else
			{
				QPointF _p1 = d->screenToAxis(rect(), _pt_new);
				QPointF _p2 = d->screenToAxis(rect(), _pt_current);
				d->viewport.pan(_p1.x() - _p2.x(), - _p1.y() + _p2.y());
				d->updateViewports(d->viewport.rect());
				QPoint _local_pt = mapFromGlobal(_pt_current);
				d->adjustCoordinator(rect(), _local_pt);
				d->renderer->rebuild();
			}
		}
		break;
	case jInputPattern::MoveCursorRight:
		{
			const QPoint & _pt_current = QCursor::pos();
			QPoint _pt_new = _pt_current;

			_pt_new.setX(_pt_current.x() + 1);

			if (rect().contains(mapFromGlobal(_pt_new)))
			{
				QCursor::setPos(_pt_new);
			}
			else
			{
				QPointF _p1 = d->screenToAxis(rect(), _pt_new);
				QPointF _p2 = d->screenToAxis(rect(), _pt_current);
				d->viewport.pan(_p1.x() - _p2.x(), - _p1.y() + _p2.y());
				d->updateViewports(d->viewport.rect());
				QPoint _local_pt = mapFromGlobal(_pt_current);
				d->adjustCoordinator(rect(), _local_pt);
				d->renderer->rebuild();
			}
		}
		break;
	case jInputPattern::MoveCursorUp:
		{
			const QPoint & _pt_current = QCursor::pos();
			QPoint _pt_new = _pt_current;

			_pt_new.setY(_pt_current.y() - 1);

			if (rect().contains(mapFromGlobal(_pt_new)))
			{
				QCursor::setPos(_pt_new);
			}
			else
			{
				QPointF _p1 = d->screenToAxis(rect(), _pt_new);
				QPointF _p2 = d->screenToAxis(rect(), _pt_current);
				d->viewport.pan(_p1.x() - _p2.x(), _p1.y() - _p2.y());
				d->updateViewports(d->viewport.rect());
				QPoint _local_pt = mapFromGlobal(_pt_current);
				d->adjustCoordinator(rect(), _local_pt);
				d->renderer->rebuild();
			}
		}		
		break;
	case jInputPattern::MoveCursorDown:
		{
			const QPoint & _pt_current = QCursor::pos();
			QPoint _pt_new = _pt_current;

			_pt_new.setY(_pt_current.y() + 1);

			if (rect().contains(mapFromGlobal(_pt_new)))
			{
				QCursor::setPos(_pt_new);
			}
			else
			{
				QPointF _p1 = d->screenToAxis(rect(), _pt_new);
				QPointF _p2 = d->screenToAxis(rect(), _pt_current);
				d->viewport.pan(_p1.x() - _p2.x(), _p1.y() - _p2.y());
				d->updateViewports(d->viewport.rect());
				QPoint _local_pt = mapFromGlobal(_pt_current);
				d->adjustCoordinator(rect(), _local_pt);
				d->renderer->rebuild();
			}
		}
		break;
	case jInputPattern::ZoomDeltaVertical:
		{
			const double _axis_point_x = d->screenToAxis(rect(), QPointF(_mpos.x(), 0)).x();
			QRectF _rect;
			QRectF _zoom_rect = d->viewport.rect();
			double _k = (_axis_point_x - _zoom_rect.left()) / _zoom_rect.width();
			if ((qint16)(_modifier & 0x0000ffff) > 0)
			{
				_rect = QRectF(QPointF(_axis_point_x - (_zoom_rect.width() * _k) / 2.0, _zoom_rect.top()) , 
					QSizeF(_zoom_rect.width() / 2.0, _zoom_rect.height()));
			}
			if ((qint16)(_modifier & 0x0000ffff) < 0)
			{
				if (_zoom_rect.width() * 2.0 > d->viewport.maximumSize().width() && !d->viewport.maximumSize().isEmpty())
				{
					QPointF _zc = _zoom_rect.center();
					_zoom_rect.setLeft(_zc.x() - d->viewport.maximumSize().width() / 4);
					_zoom_rect.setRight(_zc.x() + d->viewport.maximumSize().width() / 4);
//					_k = (_axis_point_x - _zoom_rect.left()) / _zoom_rect.width();
				}
				_rect =	QRectF(QPointF(_axis_point_x - (_zoom_rect.width() * _k) * 2.0, _zoom_rect.top()) , 
					QSizeF(_zoom_rect.width() * 2.0, _zoom_rect.height()));
			}
			if (_rect.isValid())
			{
				d->viewport.zoomIn(d->viewport.adjustRect(_rect));
				d->updateViewports(d->viewport.rect());
				d->adjustCoordinator(rect(), _mpos);
				d->renderer->rebuild();
			}
		}
		break;
	case jInputPattern::ZoomDeltaHorizontal:
		{
			const double _axis_point_y = d->screenToAxis(rect(), QPointF(0, _mpos.y())).y();
			QRectF _rect;
			QRectF _zoom_rect = d->viewport.rect();
			double _k = (_axis_point_y - _zoom_rect.top()) / _zoom_rect.height();
			if ((qint16)(_modifier & 0x0000ffff) > 0)
			{
				_rect = QRectF(QPointF(_zoom_rect.left(), _axis_point_y - (_zoom_rect.height() * _k) / 2.0) , 
					QSizeF(_zoom_rect.width(), _zoom_rect.height() / 2.0));
			}
			if ((qint16)(_modifier & 0x0000ffff) < 0)
			{
				if (_zoom_rect.height() * 2.0 > d->viewport.maximumSize().height() && !d->viewport.maximumSize().isEmpty())
				{
					QPointF _zc = _zoom_rect.center();
					_zoom_rect.setTop(_zc.y() - d->viewport.maximumSize().height() / 4);
					_zoom_rect.setBottom(_zc.y() + d->viewport.maximumSize().height() / 4);
//					_k = (_axis_point_y - _zoom_rect.top()) / _zoom_rect.height();
				}
				_rect = QRectF(QPointF(_zoom_rect.left(), _axis_point_y - (_zoom_rect.height() * _k) * 2.0) , 
					QSizeF(_zoom_rect.width(), _zoom_rect.height() * 2.0));
			}
			if (_rect.isValid())
			{
				d->viewport.zoomIn(d->viewport.adjustRect(_rect));
				d->updateViewports(d->viewport.rect());
				d->adjustCoordinator(rect(), _mpos);
				d->renderer->rebuild();
			}
		}
		break;
	case jInputPattern::ZoomMove:
		{
			QPointF _move_point = _mpos;
			if (d->in_zoom)
			{
				d->viewport.selector().setRect(d->viewport.adjustRect(d->screenToAxis(rect(), QRectF(d->press_point, _move_point)), true));
				d->renderer->rebuild();
			}
			d->move_point = _move_point;
		}
		break;
	case jInputPattern::ZoomStart:
		d->move_point = _mpos;

		d->in_zoom = true;
		d->viewport.selector().setRect(QRectF());
		d->viewport.selector().setVisible(d->viewport.zoomOrientation() != 0);
		break;
	case jInputPattern::ZoomEnd:	
		d->in_zoom = false;
		d->viewport.selector().setVisible(false);

		if (!intEqual(d->press_point.x(), d->release_point.x()))
		{
			if (d->press_point.x() > d->release_point.x())
			{
				d->viewport.zoomOut();
			}
			else
			{
				d->viewport.zoomIn(d->screenToAxis(rect(), QRectF(d->press_point, d->release_point)));
			}
			d->updateViewports(d->viewport.rect());
		}
		d->adjustCoordinator(rect(), d->release_point);
		d->renderer->rebuild();
		break;
	case jInputPattern::ZoomFullView:
		d->viewport.zoomFullView();
		d->updateViewports(d->viewport.rect());
		d->renderer->rebuild();
		break;
	case jInputPattern::PanMove:
		{
			QPointF _p1 = d->screenToAxis(rect(), d->move_point);
			QPointF _p2 = d->screenToAxis(rect(), _mpos);
			d->viewport.pan(_p1.x() - _p2.x(), _p1.y() - _p2.y());
			d->updateViewports(d->viewport.rect());
			setCursor(Qt::ClosedHandCursor);
			d->renderer->rebuild();
			d->move_point = _mpos;
		}
		break;
	case jInputPattern::PanStart:
		d->move_point = _mpos;

		if ((_cursor.shape() != Qt::OpenHandCursor) && (_cursor.shape() != Qt::ClosedHandCursor))
		{
			d->default_cursor = _cursor;
			setCursor(Qt::OpenHandCursor);
		}
		break;
	case jInputPattern::PanEnd:
		restoreCursorBeforePan();
		d->adjustCoordinator(rect(), d->release_point);
		d->renderer->rebuild();
		break;
	case jInputPattern::ContextMenuRequested:
		if ((d->press_point == d->release_point) || (_method == jInputPattern::KeyPress) || (_method == jInputPattern::KeyRelease))
		{
			restoreCursorBeforePan();
			emit contextMenuRequested(mapToGlobal(_mpos.toPoint()));
		}
		break;
	default:
		return false;
	}
	return true;
}

void jView::restoreCursorBeforePan()
{
	const QCursor & _cursor = cursor();
	if (_cursor.shape() == Qt::ClosedHandCursor || _cursor.shape() == Qt::OpenHandCursor)
	{
		setCursor(d->default_cursor);
	}
}

void jView::mouseMoveEvent(QMouseEvent * _me)
{
	QPointF _move_point = _me->posF();
	d->adjustCoordinator(rect(), _move_point);
	d->renderer->rebuild();
}


jView & jView::addLabel(jLabel * _label)
{
	return addLabels(QVector<jLabel *>() << _label);
}

jView & jView::addLabels(const QVector<jLabel *> & _labels)
{
	d->labels << _labels;
	return (* this);
}

jView & jView::setLabel(jLabel * _label)
{
	return setLabels(QVector<jLabel *>() << _label);
}

jView & jView::removeLabel(jLabel * _label)
{
	return removeLabels(QVector<jLabel *>() << _label);
}

jView & jView::removeLabels(const QVector<jLabel *> & _labels)
{
	foreach (jLabel * _label, _labels)
	{
		QVector<jLabel *>::iterator _it = ::qFind(d->labels.begin(), d->labels.end(), _label);
		if (_it != d->labels.end())
		{
			d->labels.erase(_it);
		}
	}
	return * this;
}

jView & jView::setLabels(const QVector<jLabel *> & _labels)
{
	d->labels = _labels;
	return * this;
}

QVector<jLabel *> jView::labels() const
{
	return d->labels;
}

jView & jView::addSelector(jSelector * _selector)
{
	return addSelectors(QVector<jSelector *>() << _selector);
}

jView & jView::addSelectors(const QVector<jSelector *> & _selectors)
{
	d->selectors << _selectors;
	for (int _idx = 0; _idx < _selectors.count(); _idx++)
	{
		installEventFilter(& _selectors[_idx]->internalItem().inputPattern());
	}
	::qSort(d->selectors.begin(), d->selectors.end(), &Data::selectorZSort);
	return (* this);
}

jView & jView::setSelector(jSelector * _selector)
{
	return setSelectors(QVector<jSelector *>() << _selector);
}

jView & jView::removeSelector(jSelector * _selector)
{
	return removeSelectors(QVector<jSelector *>() << _selector);
}

jView & jView::removeSelectors(const QVector<jSelector *> & _selectors)
{
	foreach (jSelector * _selector, _selectors)
	{
		QVector<jSelector *>::iterator _it = ::qFind(d->selectors.begin(), d->selectors.end(), _selector);
		if (_it != d->selectors.end())
		{
			removeEventFilter(& (* _it)->internalItem().inputPattern());
			d->selectors.erase(_it);
		}
	}
	::qSort(d->selectors.begin(), d->selectors.end(), &Data::selectorZSort);
	return * this;
}

jView & jView::setSelectors(const QVector<jSelector *> & _selectors)
{
	d->selectors = _selectors;
	for (int _idx = 0; _idx < d->selectors.count(); _idx++)
	{
		installEventFilter(& d->selectors[_idx]->internalItem().inputPattern());
	}
	::qSort(d->selectors.begin(), d->selectors.end(), &Data::selectorZSort);
	return * this;
}

QVector<jSelector *> jView::selectors() const
{
	return d->selectors;
}

jCoordinator & jView::coordinator() const
{
	return d->coordinator;
}

void jView::enterEvent(QEvent *)
{
    setFocus(Qt::MouseFocusReason);
	d->coordinator.label().setVisible(d->coordinator_visibility.pop(d->coordinator.label().isVisible()));
	d->hmarker.setVisible(d->hmarker_visibility.pop(d->hmarker.isVisible()));
	d->vmarker.setVisible(d->vmarker_visibility.pop(d->vmarker.isVisible()));
	d->renderer->rebuild();
}

void jView::leaveEvent(QEvent *)
{
	d->coordinator_visibility.push(d->coordinator.label().isVisible());
	d->hmarker_visibility.push(d->hmarker.isVisible());
	d->vmarker_visibility.push(d->vmarker.isVisible());
	d->coordinator.label().setVisible(false);
	d->hmarker.setVisible(false);
	d->vmarker.setVisible(false);
	d->renderer->rebuild();
}

void jView::resizeEvent(QResizeEvent *)
{
    if (d->widget_rect.isValid())
    {
        d->renderer->rebuild();
    }
    d->widget_rect = rect();
}

jView & jView::setMarkers(const QVector<jMarker *> & _markers)
{
	d->markers = _markers;
	for (int _idx = 0; _idx < d->markers.count(); _idx++)
	{
		installEventFilter(& d->markers[_idx]->internalItem().inputPattern());
	}
	::qSort(d->markers.begin(), d->markers.end(), &Data::markerZSort);
	return * this;
}

QVector<jMarker *> jView::markers() const
{
	return d->markers;
}

jMarker & jView::horizontalMarker() const
{
	return d->hmarker;
}

jMarker & jView::verticalMarker() const
{
	return d->vmarker;
}

jRenderer & jView::renderer() const
{
	return * d->renderer;
}

QTransform jView::screenToAxisTransform() const
{
	return d->screenToAxisTransform(rect());
}

QRectF jView::screenToAxis(const QRectF & _src_rect) const
{
	return d->screenToAxis(rect(), _src_rect);
}

QPointF jView::screenToAxis(const QPointF & _src_point) const
{
	return d->screenToAxis(rect(), _src_point);
}

QTransform jView::axisToScreenTransform() const
{
	return d->axisToScreenTransform(rect());
}

QRectF jView::axisToScreen(const QRectF & _src_rect) const
{
	return d->axisToScreen(_src_rect, rect());
}

QPointF jView::axisToScreen(const QPointF & _src_point) const
{
	return d->axisToScreen(_src_point, rect());
}


QVector<jItem *> jView::itemsAt(const QPointF & _point, bool _exclude_invisible) const
{
	QVector<jItem *> _items;
	foreach (jItem * _item, d->items)
	{
		if (_exclude_invisible && (_item->isVisible() == false))
		{
			continue;
		}
		if (_item->intersects(d->screenToAxis(rect(), QRectF(QPointF(_point.x() - 2, _point.y() - 2), QSizeF(4, 4))), d->x_axis, d->y_axis))
		{
			_items << _item;
		}
	}
	return _items;
}

QVector<jItem *> jView::showToolTip(const QPointF & _point)
{
	if (rect().contains(_point.toPoint()) == false)
	{
		return QVector<jItem *>();
	}
	QVector<jItem *> _items = itemsAt(_point);
	if (_items.count() > 0)
	{
		QToolTip::hideText();
		QString _tooltip;
		foreach (jItem * _item, _items)
		{
			if (_item->toolTip().length() > 0)
			{
				_tooltip += "<div>" + _item->toolTip() + "</div>";
			}
		}
		if (_tooltip.length() > 1)
		{
			QToolTip::showText(mapToGlobal(_point.toPoint()), _tooltip, this);
		}
	}
	return _items;
}

void jView::rebuild()
{
	d->updateViewports(d->viewport.rect());
	d->renderer->rebuild();
}

QRectF jView::itemsBoundingRect(bool _exclude_invisible) const
{
	QRectF _united;
	QVector<jItem *> _items;
	foreach (jItem * _item, d->items)
	{
		if (_exclude_invisible && (_item->isVisible() == false))
		{
			continue;
		}
		QRectF _rect = _item->boundingRect(d->x_axis, d->y_axis);
		if (_united.isValid())
		{
			if (_rect.isValid())
			{
				_united.setLeft(qMin(_united.left(), _rect.left()));
				_united.setTop(qMin(_united.top(), _rect.top()));
				_united.setRight(qMax(_united.right(), _rect.right()));
				_united.setBottom(qMax(_united.bottom(), _rect.bottom()));
			}
		}
		else
		{
			_united = _rect;
		}
	}
	return _united;
}

void jView::autoScaleX(double _margin_x)
{
	QRectF _bounding_rect = itemsBoundingRect();

	const double _width = _bounding_rect.width();
	const double _offset_x = _width * _margin_x;
	_bounding_rect.setLeft(_bounding_rect.left() - _offset_x);
	_bounding_rect.setRight(_bounding_rect.right() + _offset_x);

	d->x_axis->setRange(_bounding_rect.left(), _bounding_rect.right(), d->x_axis->rangeFunc());
	d->viewport.adjustBase(* d->x_axis, * d->y_axis);
	d->updateViewports(d->viewport.rect());
}

void jView::autoScaleY(double _margin_y)
{
	QRectF _bounding_rect = itemsBoundingRect();

	const double _height = _bounding_rect.height();
	const double _offset_y = _height * _margin_y;
	_bounding_rect.setTop(_bounding_rect.top() - _offset_y);
	_bounding_rect.setBottom(_bounding_rect.bottom() + _offset_y);

	d->y_axis->setRange(_bounding_rect.top(), _bounding_rect.bottom(), d->y_axis->rangeFunc());
	d->viewport.adjustBase(* d->x_axis, * d->y_axis);
	d->updateViewports(d->viewport.rect());
}

void jView::autoScale(double _margin_x, double _margin_y)
{
	QRectF _bounding_rect = itemsBoundingRect();

	const double _width = _bounding_rect.width();
	const double _offset_x = _width * _margin_x;
	_bounding_rect.setLeft(_bounding_rect.left() - _offset_x);
	_bounding_rect.setRight(_bounding_rect.right() + _offset_x);
	const double _height = _bounding_rect.height();
	const double _offset_y = _height * _margin_y;
	_bounding_rect.setTop(_bounding_rect.top() - _offset_y);
	_bounding_rect.setBottom(_bounding_rect.bottom() + _offset_y);

	d->x_axis->setRange(_bounding_rect.left(), _bounding_rect.right(), d->x_axis->rangeFunc());
	d->y_axis->setRange(_bounding_rect.top(), _bounding_rect.bottom(), d->y_axis->rangeFunc());
	d->viewport.adjustBase(* d->x_axis, * d->y_axis);
	d->updateViewports(d->viewport.rect());
}

jView & jView::setInputPattern(const jInputPattern & _pattern)
{
	d->pattern = _pattern;
	return * this;
}

jInputPattern & jView::inputPattern() const
{
	return d->pattern;
}

QPointF jView::cursorPos() const
{
	return screenToAxis(mapFromGlobal(QCursor::pos()));
}

bool jView::itemZSort(const jItem * _item1, const jItem * _item2)
{
	return Data::itemZSort(_item1, _item2);
}

bool jView::markerZSort(const jMarker * _marker1, const jMarker * _marker2)
{
	return Data::markerZSort(_marker1, _marker2);
}

bool jView::selectorZSort(const jSelector * _selector1, const jSelector * _selector2)
{
	return Data::selectorZSort(_selector1, _selector2);
}

bool jView::itemZSortReversed(const jItem * _item1, const jItem * _item2)
{
	return Data::itemZSortReversed(_item1, _item2);
}

bool jView::markerZSortReversed(const jMarker * _marker1, const jMarker * _marker2)
{
	return Data::markerZSortReversed(_marker1, _marker2);
}

bool jView::selectorZSortReversed(const jSelector * _selector1, const jSelector * _selector2)
{
	return Data::selectorZSortReversed(_selector1, _selector2);
}
// ------------------------------------------------------------------------

struct jPreview::Data
{
	jView * view;
	jSelector selector;
	QBrush background;
	int orientation;
	QPoint prev_point, press_point;
	jRenderer * renderer;
	jInputPattern pattern;
	bool x_axis_visible, y_axis_visible;
	QCursor saved_cursor, pan_cursor;
    QRect widget_rect;
	int min_dim;
	Data()
	{
		saved_cursor = Qt::OpenHandCursor;
		pan_cursor = Qt::ClosedHandCursor;
		pattern.setDefaultPattern();
		view = 0;
		min_dim = 1;
		orientation = Qt::Vertical | Qt::Horizontal;
		x_axis_visible = false;
		y_axis_visible = false;
	}
	~Data()
	{
	}
	__inline static bool itemZSort(const jItem * _item1, const jItem * _item2)
	{
		if (_item1 && _item2)
		{
			return (_item1->z() < _item2->z());
		}
		return false;
	}
	__inline QRectF updateSelector(const QRectF & _rect)
	{
		QTransform _back_transform, _transform;
		if ((view == 0) || (::jQuadToQuad(view->viewport().rectBase(), _rect, _back_transform) == false))
		{
			return QRectF();
		}
		QRectF _viewport_rect_base = view->viewport().rectBase();
		QRectF _viewport_rect = view->viewport().rect();

		const int _min_dim = min_dim;
		if (_min_dim == 1)
		{
			selector.setRect(_viewport_rect);
			return _viewport_rect_base;
		}

		QRectF _rect_adjusted = _rect;

		float _dx = 0, _dy = 0;

		QRectF _screen_rect = _back_transform.mapRect(_viewport_rect);
		if (_screen_rect.width() < _min_dim)
		{
			_dx = qMin(_rect_adjusted.width(), _min_dim - _screen_rect.width()) / 2;
			_screen_rect.adjust(- _dx, 0, _dx, 0);
			_rect_adjusted.adjust(- _dx, 0, _dx, 0);
		}
		if (_screen_rect.height() < _min_dim)
		{
			_dy = qMin(_rect_adjusted.height(), _min_dim - _screen_rect.height()) / 2;
			_screen_rect.adjust(0, - _dy, 0, _dy);
			_rect_adjusted.adjust(0, - _dy, 0, _dy);
		}

		if (::jQuadToQuad(_rect_adjusted, _viewport_rect_base, _transform) == false)
		{
			selector.setRect(_viewport_rect);
			return _viewport_rect_base;
		}

		_viewport_rect = _transform.mapRect(_screen_rect);

		if ((orientation & Qt::Vertical) == 0)
		{
			_viewport_rect.setTop(_viewport_rect_base.top());
			_viewport_rect.setBottom(_viewport_rect_base.bottom());
		}
		if ((orientation & Qt::Horizontal) == 0)
		{
			_viewport_rect.setLeft(_viewport_rect_base.left());
			_viewport_rect.setRight(_viewport_rect_base.right());
		}
		selector.setRect(_viewport_rect);

		if (::jQuadToQuad(_rect, _viewport_rect_base, _transform) == false)
		{
			return _viewport_rect_base;
		}

		QRectF _min_dim_transformed = _transform.mapRect(QRectF(0, 0, _dx, _dy));

		if ((orientation & Qt::Vertical) == Qt::Vertical)
		{
			_viewport_rect_base.adjust(0, -_min_dim_transformed.height(), 0, _min_dim_transformed.height());
		}
		if ((orientation & Qt::Horizontal) == Qt::Horizontal)
		{
			_viewport_rect_base.adjust(-_min_dim_transformed.width(), 0, _min_dim_transformed.width(), 0);
		}

		return _viewport_rect_base;
	}
    __inline QPointF previewToViewScreen(const QRectF & _rect, const QPointF & _point) const // screen to screen
	{
		QTransform _transform;
		if ((view == 0) || (::jQuadToQuad(_rect, view->viewport().rectBase(), _transform) == false))
		{
			return QPointF();
		}
        return view->axisToScreen(_transform.map(_point));
	}
    __inline QPointF viewToPreviewScreen(const QRectF & _rect, const QPointF & _point) const // screen to screen
    {
        QTransform _transform;
        if ((view == 0) || (::jQuadToQuad(view->viewport().rectBase(), _rect, _transform) == false))
        {
            return QPointF();
        }
        return _transform.map(view->screenToAxis(_point));
    }    
	__inline void setToPosition(const QRect & _rect, const QMouseEvent * _me)
	{
		if (view == 0)
		{
			return;
		}
		QRectF _viewport_rect_base = view->viewport().rectBase();
		QTransform _transform;
		if (::jQuadToQuad(_rect, _viewport_rect_base, _transform) == false)
		{
			return;
		}
		QRectF _viewport_rect = view->viewport().rect();
		if ((orientation & Qt::Vertical) == 0)
		{
			_viewport_rect.setTop(_viewport_rect_base.top());
			_viewport_rect.setBottom(_viewport_rect_base.bottom());
		}
		if ((orientation & Qt::Horizontal) == 0)
		{
			_viewport_rect.setLeft(_viewport_rect_base.left());
			_viewport_rect.setRight(_viewport_rect_base.right());
		}
		QPointF _axis_pt = _transform.map(_me->pos());
		_axis_pt.setY(_viewport_rect_base.top() + _viewport_rect_base.bottom() - _axis_pt.y());
		if (_viewport_rect.contains(_axis_pt))
		{
			return;
		}
		double _delta_x = _axis_pt.x() - (_viewport_rect.x() + _viewport_rect.width() / 2);
		double _delta_y = _axis_pt.y() - (_viewport_rect.y() + _viewport_rect.height() / 2);
		if ((orientation & Qt::Vertical) == 0)
		{
			_delta_y = 0; 
		}
		if ((orientation & Qt::Horizontal) == 0)
		{
			_delta_x = 0; 
		}
		view->viewport().pan(_delta_x, _delta_y);
		view->rebuild();
	}
    __inline void pan(const QRect & _rect, const QMouseEvent * _me)
	{
		if (view == 0)
		{
			return;
		}
		QRectF _viewport_rect_base = view->viewport().rectBase();
		QTransform _transform;
		if (::jQuadToQuad(_rect, _viewport_rect_base, _transform) == false)
		{
			return;
		}
		QRectF _viewport_rect = view->viewport().rect();
		if ((orientation & Qt::Vertical) == 0)
		{
			_viewport_rect.setTop(_viewport_rect_base.top());
			_viewport_rect.setBottom(_viewport_rect_base.bottom());
		}
		if ((orientation & Qt::Horizontal) == 0)
		{
			_viewport_rect.setLeft(_viewport_rect_base.left());
			_viewport_rect.setRight(_viewport_rect_base.right());
		}
		QPointF _axis_pt = _transform.map(_me->pos());
		_axis_pt.setY(_viewport_rect_base.top() + _viewport_rect_base.bottom() - _axis_pt.y());
		QPointF _axis_prev_pt = _transform.map(prev_point);
		_axis_prev_pt.setY(_viewport_rect_base.top() + _viewport_rect_base.bottom() - _axis_prev_pt.y());
		double _delta_x = _axis_pt.x() - _axis_prev_pt.x();
		double _delta_y = _axis_pt.y() - _axis_prev_pt.y();
		if ((orientation & Qt::Vertical) == 0)
		{
			_delta_y = 0; 
		}
		if ((orientation & Qt::Horizontal) == 0)
		{
			_delta_x = 0; 
		}
		view->viewport().pan(_delta_x, _delta_y);
		view->rebuild();
	}
    __inline void zoomFullView(const QRectF &, const QMouseEvent *)
	{
		if (view == 0)
		{
			return;
		}
		view->viewport().zoomFullView();
		view->rebuild();
	}
	__inline void wheelScale(const QRectF & _rect, const QWheelEvent * _we)
	{
		if (view == 0)
		{
			return;
		}
        QWheelEvent * _wheel = new QWheelEvent(
            previewToViewScreen(_rect, _we->pos()).toPoint(),
            _we->delta(),
            _we->buttons(),
            _we->modifiers(),
            _we->orientation()
            );
        QCoreApplication::postEvent(view, _wheel);
        QCoreApplication::sendPostedEvents(view, QEvent::Wheel);
	}
	__inline static void render_func(QWidget * _widget, QPainter & _painter)
	{
		dynamic_cast<jPreview *>(_widget)->render(_painter);
	}
};

jPreview::jPreview(QWidget * _parent)
	: JUNGLE_WIDGET_CLASS(_parent), d(new Data())
{
	d->renderer = new jRenderer(this, &Data::render_func);
	installEventFilter(d->renderer);
	connect(&d->pattern, SIGNAL(actionAccepted(int, int, int, int, QPointF, QWidget *)), this, SLOT(actionAccepted(int, int, int, int, QPointF, QWidget *)), Qt::DirectConnection);
	installEventFilter(&d->pattern);
	setCursor(d->saved_cursor);
}

jPreview::jPreview(jView * _view, QWidget * _parent)
	: JUNGLE_WIDGET_CLASS(_parent), d(new Data())
{
	d->renderer = new jRenderer(this, &Data::render_func);
	installEventFilter(d->renderer);

	connect(&d->pattern, SIGNAL(actionAccepted(int, int, int, int, QPointF, QWidget *)), this, SLOT(actionAccepted(int, int, int, int, QPointF, QWidget *)), Qt::DirectConnection);
	installEventFilter(&d->pattern);

	setView(_view);
	setCursor(d->saved_cursor);
}

jPreview::~jPreview()
{
	setView(0);
	removeEventFilter(d->renderer);
	delete d->renderer;
	delete d;
}

jPreview & jPreview::setView(jView * _view)
{
	if (d->view)
	{
		disconnect(d->view, SIGNAL(viewportChanged(QRectF)), d->renderer, SLOT(rebuild()));
		disconnect(d->view, SIGNAL(destroyed(QObject *)), this, SLOT(viewDestroyed(QObject *)));
	}
	d->view = _view;
	if (d->view)
	{
		connect(d->view, SIGNAL(viewportChanged(QRectF)), d->renderer, SLOT(rebuild()));
		connect(d->view, SIGNAL(destroyed(QObject *)), this, SLOT(viewDestroyed(QObject *)));
		setInputPattern(d->view->inputPattern());
	}
	return * this;
}

const jView * jPreview::view() const
{
	return d->view;
}

void jPreview::viewDestroyed(QObject *)
{
	d->view = 0;
}

jSelector & jPreview::selector() const
{
	return d->selector;
}

jRenderer & jPreview::renderer() const
{
	return * d->renderer;
}

void jPreview::render(QPainter & _painter) const
{
	if (d->view == 0)
	{
		return;
	}
    QRectF _rect = d->widget_rect;
	if (d->background.style() != Qt::NoBrush)
	{
		_painter.fillRect(_rect, d->background);
	}
	QRectF _zoom_rect = d->view->viewport().rectBase();
	QRectF _adjusted_zoom_rect = d->updateSelector(_rect);
	QVector<jItem *> _items = d->view->items();
	::qSort(_items.begin(), _items.end(), &Data::itemZSort);
	jAxis * _x_axis = const_cast<jAxis *>(d->view->xAxis());
	jAxis * _y_axis = const_cast<jAxis *>(d->view->yAxis());
	const int _axes_plane = d->view->axesPlane();
	if (_axes_plane == jView::AxesInBackplane)
	{
		if (d->x_axis_visible && _x_axis && ((d->orientation & Qt::Horizontal) == Qt::Horizontal))
		{
			_x_axis->render(_painter, _rect, Qt::Horizontal, _adjusted_zoom_rect.left(), _adjusted_zoom_rect.right(), false);
		}
		if (d->y_axis_visible && _y_axis && ((d->orientation & Qt::Vertical) == Qt::Vertical))
		{
			_y_axis->render(_painter, _rect, Qt::Vertical, _adjusted_zoom_rect.bottom(), _adjusted_zoom_rect.top(), false);
		}
	}
	foreach (jItem * _item, _items)
	{
		_item->renderPreview(_painter, _rect, _adjusted_zoom_rect, _x_axis, _y_axis);
	}
	foreach (jSelector * _selector, d->view->selectors())
	{
		_selector->renderPreview(_painter, _rect, _adjusted_zoom_rect);
	}
	foreach (jMarker * _marker, d->view->markers())
	{
		_marker->renderPreview(_painter, _rect, _adjusted_zoom_rect);
	}
	foreach (jLabel * _label, d->view->labels())
	{
		_label->renderPreview(_painter, _rect, _adjusted_zoom_rect);
	}
	if (_axes_plane == jView::AxesInForeplane)
	{
		if (d->x_axis_visible && _x_axis && ((d->orientation & Qt::Horizontal) == Qt::Horizontal))
		{
			_x_axis->render(_painter, _rect, Qt::Horizontal, _adjusted_zoom_rect.left(), _adjusted_zoom_rect.right(), false);
		}
		if (d->y_axis_visible && _y_axis && ((d->orientation & Qt::Vertical) == Qt::Vertical))
		{
			_y_axis->render(_painter, _rect, Qt::Vertical, _adjusted_zoom_rect.bottom(), _adjusted_zoom_rect.top(), false);
		}
	}
	d->selector.render(_painter, _rect, _zoom_rect);
}

void jPreview::resizeEvent(QResizeEvent *)
{
    if (d->widget_rect.isValid())
    {
        d->renderer->rebuild();
    }
    d->widget_rect = rect();
}

QSize jPreview::minimumSizeHint() const
{
	return QSize(8, 8);
}

jPreview & jPreview::setBackground(const QBrush & _brush)
{
	d->background = _brush;
	return * this;
}

QBrush jPreview::background() const
{
	return d->background;
}

jPreview & jPreview::setOrientation(int _orientation)
{
	d->orientation = _orientation;
	return * this;
}

int jPreview::orientation() const
{
	return d->orientation;
}

jPreview & jPreview::setInputPattern(const jInputPattern & _pattern)
{
	d->pattern = _pattern;
	return * this;
}

jInputPattern & jPreview::inputPattern() const
{
	return d->pattern;
}

void jPreview::actionAccepted(int _action, int _method, int _code, int _modifier, QPointF _mpos, QWidget * _w)
{
	if (_w != this)
	{
		return;
	}
	if ((_action < jInputPattern::WidgetActionGroupBegin) || (_action > jInputPattern::WidgetActionGroupEnd))
	{
		return;
	}
	jInputPattern::setAccepted(& d->pattern, userCommand(_action, _method, _code, _modifier, _mpos, _w));
}

bool jPreview::userCommand(int _action, int /*_method*/, int _code, int _modifier, QPointF _mpos, QWidget * /*_w*/)
{
    QMouseEvent _me(QEvent::MouseButtonRelease, _mpos.toPoint(), (Qt::MouseButton)_code, (Qt::MouseButtons)_code, (Qt::KeyboardModifiers)_modifier);
    QWheelEvent _we(
		(qint16)(_modifier & 0x0000ffff) < 0 ? d->viewToPreviewScreen(rect(), d->view->rect().center()).toPoint() : _mpos.toPoint(), 
		(qint16)(_modifier & 0x0000ffff), 
		(Qt::MouseButtons)_code, 
		(Qt::KeyboardModifiers)(_modifier & 0x7f000000));
	switch (_action)
	{
	case jInputPattern::PreviewPanStart:
		d->prev_point = _mpos.toPoint();
		d->press_point = _mpos.toPoint();
		d->saved_cursor = cursor();
		setCursor(d->pan_cursor);
		break;
	case jInputPattern::PreviewPanEnd:
		if (d->press_point == _mpos.toPoint())
		{
            d->setToPosition(rect(), &_me);
		}
		setCursor(d->saved_cursor);
		break;
	case jInputPattern::PreviewPanMove:
        d->pan(rect(), &_me);
		d->prev_point = _mpos.toPoint();
		break;
	case jInputPattern::PreviewFullView:
        d->zoomFullView(rect(), &_me);
		break;
	case jInputPattern::PreviewDeltaVertical:
	case jInputPattern::PreviewDeltaHorizontal:
        if (_we.orientation() != d->orientation)
        {
            d->wheelScale(rect(), &_we);
        }
        else
        {
			const QPointF & _pt_00 = d->view->screenToAxis(QPointF(0, 0));
			const QPointF & _pt_11 = d->view->screenToAxis(QPointF(1, 1));
			QSizeF _step;
			_step.setWidth(_we.orientation() == Qt::Horizontal ? _pt_11.x() - _pt_00.x() : 0);
			_step.setHeight(_we.orientation() == Qt::Vertical ? _pt_11.y() - _pt_00.y() : 0);
            d->view->viewport().pan(-_we.delta() * _step.width(), -_we.delta() * _step.height());
            d->view->rebuild();
        }
		break;
	default:
		return false;
	}
	return true;
}

void jPreview::rebuild()
{
	d->renderer->rebuild();
}


jPreview & jPreview::setXAxisVisible(bool _state)
{
	d->x_axis_visible = _state;
	return * this;
}

bool jPreview::isXAxisVisible() const
{
	return d->x_axis_visible;
}

jPreview & jPreview::setYAxisVisible(bool _state)
{
	d->y_axis_visible = _state;
	return * this;
}

bool jPreview::isYAxisVisible() const
{
	return d->y_axis_visible;
}

jPreview & jPreview::setPanCursor(const QCursor & _pan_cursor)
{
	d->pan_cursor = _pan_cursor;
	return * this;
}

QCursor jPreview::panCursor() const
{
	return d->pan_cursor;
}

QPointF jPreview::previewToViewScreen(const QPointF & _point) const
{
	return d->previewToViewScreen(rect(), _point);
}

QPointF jPreview::viewToPreviewScreen(const QPointF & _point) const
{
	return d->viewToPreviewScreen(rect(), _point);
}

jPreview & jPreview::setMinimumSelectorSize(int _min_dim)
{
	d->min_dim = qMax(_min_dim, 1);
	return (* this);
}

int jPreview::minimumSelectorSize() const
{
	return d->min_dim;
}

// ------------------------------------------------------------------------

struct jRenderer::Data
{
	QWidget * widget;
    QSize widget_size;
    quint64 counter;
	bool force_update;
	jRenderer::render_func render_func;
	Data()
	{
		force_update = false;
		counter = 0;
		widget = 0;
	}
	~Data()
	{
	}
	__inline void render(QPaintDevice & _device)
	{
		QPainter _painter(&_device);
		_painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing, false);
		render_func(widget, _painter);
	}
};

jRenderer::jRenderer(QWidget * _widget, jRenderer::render_func _render_func)
	: QObject(), d(new Data())
{
	d->widget = _widget;
	d->render_func = _render_func;
//	connect(this, SIGNAL(update()), d->widget, SLOT(update()));
}

jRenderer::~jRenderer()
{
	delete d;
}

quint64 jRenderer::counter() const
{
	return d->counter;
}

bool jRenderer::eventFilter(QObject * _object, QEvent * _event)
{
	if (qobject_cast<QWidget *>(_object) == d->widget)
	{
		switch ((int)_event->type())
		{
		case QEvent::Paint:
			{
				d->render(* d->widget);
				d->counter++;
				_event->accept();
				return true;
			}
		}
	}
//	_event->ignore();
	return false;
}

void jRenderer::rebuild()
{
	d->widget->update();
}

// ------------------------------------------------------------------------

struct jSync::Data
{
	QVector<jView *> views;
	QVector<jPreview *> previews;
	QMap< QObject *, jStack<bool> > blocked_signals;
	Data()
	{

	}
	~Data()
	{

	}
	__inline void connectAll(jSync * _sync)
	{
		foreach (jView * _view, views)
		{
			QObject::connect(&_view->viewport(), SIGNAL(panned(QRectF)), _sync, SLOT(onPanned(QRectF)));
			QObject::connect(&_view->viewport(), SIGNAL(zoomedIn(QRectF)), _sync, SLOT(onZoomedIn(QRectF)));
			QObject::connect(&_view->viewport(), SIGNAL(zoomedOut(QRectF)), _sync, SLOT(onZoomedOut(QRectF)));
			QObject::connect(&_view->viewport(), SIGNAL(zoomedFullView(QRectF)), _sync, SLOT(onZoomedFullView(QRectF)));
		}
	}
	__inline void disconnectAll(jSync * _sync)
	{
		foreach (jView * _view, views)
		{
			QObject::disconnect(&_view->viewport(), SIGNAL(panned(QRectF)), _sync, SLOT(onPanned(QRectF)));
			QObject::disconnect(&_view->viewport(), SIGNAL(zoomedIn(QRectF)), _sync, SLOT(onZoomedIn(QRectF)));
			QObject::disconnect(&_view->viewport(), SIGNAL(zoomedOut(QRectF)), _sync, SLOT(onZoomedOut(QRectF)));
			QObject::disconnect(&_view->viewport(), SIGNAL(zoomedFullView(QRectF)), _sync, SLOT(onZoomedFullView(QRectF)));
		}
	}
	__inline void reconnectAll(jSync * _sync)
	{
		disconnectAll(_sync);
		connectAll(_sync);
	}
	__inline QRectF mapRect(jView * _src, const QRectF & _src_rect, jView * _dst)
	{
		QRectF _zoom_rect(_dst->viewport().rect());
		if (_src->xAxis()->id())
		{
			if ((_dst->xAxis()->id() == _src->xAxis()->id()) && (_dst->viewport().zoomOrientation() & Qt::Horizontal))
			{
				_zoom_rect.setLeft(_dst->xAxis()->mapFromAxis(_src_rect.left(), * _src->xAxis()));
				_zoom_rect.setRight(_dst->xAxis()->mapFromAxis(_src_rect.right(), * _src->xAxis()));
			}
			if ((_dst->yAxis()->id() == _src->xAxis()->id()) && (_dst->viewport().zoomOrientation() & Qt::Vertical))
			{
				_zoom_rect.setTop(_dst->yAxis()->mapFromAxis(_src_rect.left(), * _src->xAxis()));
				_zoom_rect.setBottom(_dst->yAxis()->mapFromAxis(_src_rect.right(), * _src->xAxis()));
			}
		}
		if (_src->yAxis()->id())
		{
			if ((_dst->yAxis()->id() == _src->yAxis()->id()) && (_dst->viewport().zoomOrientation() & Qt::Vertical))
			{
				_zoom_rect.setTop(_dst->yAxis()->mapFromAxis(_src_rect.top(), * _src->yAxis()));
				_zoom_rect.setBottom(_dst->yAxis()->mapFromAxis(_src_rect.bottom(), * _src->yAxis()));
			}
			if ((_dst->xAxis()->id() == _src->yAxis()->id()) && (_dst->viewport().zoomOrientation() & Qt::Horizontal))
			{
				_zoom_rect.setLeft(_dst->xAxis()->mapFromAxis(_src_rect.top(), * _src->yAxis()));
				_zoom_rect.setRight(_dst->xAxis()->mapFromAxis(_src_rect.bottom(), * _src->yAxis()));
			}
		}

		return _zoom_rect;
	}
	__inline void update(jView * _view)
	{
		_view->renderer().rebuild();
		foreach (jPreview * _preview, previews)
		{
			if (_preview && (_preview->view() == _view))
			{
				_preview->renderer().rebuild();
			}
		}
	}
	__inline void blockSignals(QObject * _object)
	{
		blocked_signals[_object].push(_object->signalsBlocked());
		_object->blockSignals(true);
	}
	__inline void unblockSignals(QObject * _object)
	{
		_object->blockSignals(blocked_signals[_object].pop(_object->signalsBlocked()));
	}
};

jSync::jSync(): QObject(), d(new Data())
{

}

jSync::~jSync()
{
	reset();
	delete d;
}

jSync & jSync::setViews(const QVector<jView *> & _views)
{
	d->views = _views;
	d->reconnectAll(this);
	return * this;
}

const QVector<jView *> & jSync::views() const
{
	return d->views;
}

jSync & jSync::setPreviews(const QVector<jPreview *> & _previews)
{
	d->previews = _previews;
	return * this;
}

const QVector<jPreview *> & jSync::previews() const
{
	return d->previews;
}

void jSync::reset()
{
	d->disconnectAll(this);
	d->views.clear();
	d->previews.clear();
}

void jSync::onPanned(const QRectF & _rect)
{
	jView * _sender = dynamic_cast<jView *>(sender()->parent());
	if (_sender == 0)
	{
		return;
	}
	foreach (jView * _view, d->views)
	{
		if ((_view == 0) || (_sender == _view))
		{
			continue;
		}
		QRectF _zoom_rect = d->mapRect(_sender, _rect, _view);
		jViewport & _viewport = _view->viewport();
		d->blockSignals(&_viewport);
		_viewport.pan(_zoom_rect.x() - _viewport.rect().x(), _zoom_rect.y() - _viewport.rect().y());
		d->unblockSignals(&_viewport);
		d->update(_view);
	}
}

void jSync::onZoomedIn(const QRectF & _rect)
{
	jView * _sender = dynamic_cast<jView *>(sender()->parent());
	if (_sender == 0)
	{
		return;
	}
	if (_sender->viewport().zoomOrientation() == 0)
	{
		return;
	}
	foreach (jView * _view, d->views)
	{
		if ((_view == 0) || (_sender == _view))
		{
			continue;
		}
		const QRectF _zoom_rect = d->mapRect(_sender, _rect, _view);
		d->blockSignals(&_view->viewport());
		_view->viewport().zoomIn(_zoom_rect);
		d->unblockSignals(&_view->viewport());
		d->update(_view);
	}
}

void jSync::onZoomedOut(const QRectF & _rect)
{
	jView * _sender = dynamic_cast<jView *>(sender()->parent());
	if (_sender == 0)
	{
		return;
	}
	if (_sender->viewport().zoomOrientation() == 0)
	{
		return;
	}
	foreach (jView * _view, d->views)
	{
		if ((_view == 0) || (_sender == _view))
		{
			continue;
		}
		const QRectF _zoom_rect = d->mapRect(_sender, _rect, _view);
		if (fuzzyRectFCompare(_zoom_rect, _view->viewport().rect()))
		{
			continue;
		}
		d->blockSignals(&_view->viewport());
		_view->viewport().zoomOut();
		d->unblockSignals(&_view->viewport());
		d->update(_view);
	}
}

void jSync::onZoomedFullView(const QRectF & _rect)
{
	jView * _sender = dynamic_cast<jView *>(sender()->parent());
	if (_sender == 0)
	{
		return;
	}
	if (_sender->viewport().zoomOrientation() == 0)
	{
		return;
	}
	foreach (jView * _view, d->views)
	{
		if ((_view == 0) || (_sender == _view))
		{
			continue;
		}
		const QRectF _zoom_rect = d->mapRect(_sender, _rect, _view);
		if (fuzzyRectFCompare(_zoom_rect, _view->viewport().rect()))
		{
			continue;
		}
		_view->viewport().clearHistory();
		_view->viewport().d->history << _zoom_rect;
		d->update(_view);
	}
}

void jSync::rebuild()
{
	foreach (jView * _view, d->views)
	{
		_view->rebuild();
	}
	foreach (jPreview * _preview, d->previews)
	{
		_preview->rebuild();
	}
}

// ------------------------------------------------------------------------

struct jLegend::Data
{
	QVector<jItem *> items;
	QMap<jItem *, QLabel *> color_labels, text_labels;
	QMap<jItem *, QCheckBox *> visibility_checkboxes;
	QMap<jItem *, QScrollBar *> updown_scrollbars;
	QGridLayout * layout;
	int behavior;
	Data()
	{
		behavior = jLegend::Simple;
		layout = new QGridLayout();
		layout->setSizeConstraint(QLayout::SetFixedSize);
		layout->setSpacing(2);
	}
	~Data()
	{

	}
	__inline static bool itemZSort(const jItem * _item1, const jItem * _item2)
	{
		if (_item1 && _item2)
		{
			return (_item1->z() > _item2->z());
		}
		return false;
	}
	__inline void createWidgets()
	{
		QLayoutItem * _child;
		while ((_child = layout->takeAt(0)) != 0) 
		{
			if (_child->widget())
			{
				_child->widget()->setVisible(false);
				_child->widget()->deleteLater();
			}
		}
		color_labels.clear();
		text_labels.clear();
		visibility_checkboxes.clear();
		updown_scrollbars.clear();
		::qSort(items.begin(), items.end(), &itemZSort);
		for (int _idx = 0; _idx < items.count(); _idx++)
		{
			jItem * _item = items[_idx];
			QCheckBox * & _visibility_checkbox = visibility_checkboxes[_item];
			QLabel * & _color_label = color_labels[_item];
			QLabel * & _text_label = text_labels[_item];
			QScrollBar * & _updown_scrollbar = updown_scrollbars[_item];

			_visibility_checkbox = new QCheckBox();
			_visibility_checkbox->setProperty("index", _idx);
			_color_label = new QLabel();
			_text_label = new QLabel();
			_updown_scrollbar = new QScrollBar(Qt::Vertical);
			_updown_scrollbar->setMaximumHeight(24);
			_updown_scrollbar->setMaximumWidth(16);
			_updown_scrollbar->setProperty("index", _idx);
			layout->addWidget(_visibility_checkbox, _idx, 0);
			layout->addWidget(_color_label, _idx, 1);
			layout->addWidget(_text_label, _idx, 2);
			layout->addWidget(_updown_scrollbar, _idx, 3);
		}
	}
	__inline void updateWidgets()
	{
		::qSort(items.begin(), items.end(), &itemZSort);
		for (int _idx = 0; _idx < items.count(); _idx++)
		{
			jItem * _item = items[_idx];
			bool _visible = _item->isVisible();
			QCheckBox * & _visibility_checkbox = visibility_checkboxes[_item];
			QLabel * & _color_label = color_labels[_item];
			QLabel * & _text_label = text_labels[_item];
			QScrollBar * & _updown_scrollbar = updown_scrollbars[_item];

			_visibility_checkbox->setVisible(((behavior & jLegend::VisibilityControl) == jLegend::VisibilityControl) && (_visible || ((behavior & jLegend::ShowHidden) == jLegend::ShowHidden)));
			_color_label->setVisible(_visible || ((behavior & jLegend::ShowHidden) == jLegend::ShowHidden));
			_text_label->setVisible(_visible || ((behavior & jLegend::ShowHidden) == jLegend::ShowHidden));
			_updown_scrollbar->setVisible(((behavior & jLegend::UpDownControl) == jLegend::UpDownControl) && (_visible || ((behavior & jLegend::ShowHidden) == jLegend::ShowHidden)));

			_visibility_checkbox->setProperty("index", _idx);
			_updown_scrollbar->setProperty("index", _idx);

			if (((behavior & jLegend::ShowHidden) == 0) && (!_visible))
			{
				continue;
			}
			_visibility_checkbox->setChecked(_visible);

			QPixmap _pixmap(60, 10);
			_pixmap.fill(Qt::black);
			QPainter _painter;
			_painter.begin(&_pixmap);
			_painter.setPen(_item->pen());
			_painter.fillRect(_pixmap.rect(), _item->brush());
			_painter.drawRect(_pixmap.rect());
			_painter.end();
			_color_label->setPixmap(_pixmap);

			_text_label->setText(_item->toolTip());
		}
	}
};

jLegend::jLegend(QWidget * _parent, Qt::WindowFlags _flags): QWidget(_parent,_flags), d(new Data())
{
	setWindowTitle("Legend");
	setLayout(d->layout);
}
jLegend::~jLegend()
{
	delete d;
}

jLegend & jLegend::setItems(const QVector<jItem *> & _items)
{
	d->items = _items;
	d->createWidgets();
	for (int _idx = 0; _idx < d->items.count(); _idx++)
	{
		connect(d->visibility_checkboxes[d->items[_idx]], SIGNAL(clicked()), this, SLOT(onVisibilityChanged()));
		connect(d->updown_scrollbars[d->items[_idx]], SIGNAL(actionTriggered(int)), this, SLOT(onZOrderChanged(int)));
	}
	return * this;
}

QVector<jItem *> jLegend::items() const
{
	return d->items;
}

void jLegend::rebuild()
{
	d->updateWidgets();
}

jLegend & jLegend::setBehavior(int _behavior)
{
	d->behavior = _behavior;
	return * this;
}

int jLegend::behavior() const
{
	return d->behavior;
}

void jLegend::onVisibilityChanged()
{
	QCheckBox * _visibility_checkbox = dynamic_cast<QCheckBox *>(sender());
	if (_visibility_checkbox == 0)
	{
		return;
	}
	int _index = _visibility_checkbox->property("index").toInt();
	if ((_index < 0) || (_index >= d->items.count()))
	{
		return;
	}
	d->items[_index]->setVisible(_visibility_checkbox->isChecked());
	d->updateWidgets();
}

void jLegend::onZOrderChanged(int _action)
{
	QScrollBar * _updown_scrollbar = dynamic_cast<QScrollBar *>(sender());
	if (_updown_scrollbar == 0)
	{
		return;
	}
	int _index = _updown_scrollbar->property("index").toInt();
	if ((_index < 0) || (_index >= d->items.count()))
	{
		return;
	}
	jItem * _item1 = 0, * _item2 = 0;

	switch (_action)
	{
	case QAbstractSlider::SliderSingleStepAdd:
		{
			if (_index < d->items.count() - 1)
			{
				_item1 = d->items[_index];
				_item2 = d->items[_index + 1];
			}
			break;
		}
	case QAbstractSlider::SliderSingleStepSub:
		{
			if (_index > 0)
			{
				_item1 = d->items[_index - 1];
				_item2 = d->items[_index];
			}
			break;
		}
	}
	if (_item1 && _item2)
	{
		double _z = _item1->z();
		_item1->setZ(_item2->z());
		_item2->setZ(_z);

		::qSwap(d->visibility_checkboxes[_item1], d->visibility_checkboxes[_item2]);
		::qSwap(d->color_labels[_item1], d->color_labels[_item2]);
		::qSwap(d->text_labels[_item1], d->text_labels[_item2]);
		::qSwap(d->updown_scrollbars[_item1], d->updown_scrollbars[_item2]);

		d->updateWidgets();
	}
}
