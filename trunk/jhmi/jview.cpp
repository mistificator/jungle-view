#include "jview.h"
#include "jitems.h"
#include <math.h>

// ------------------------------------------------------------------------

bool jQuadToQuad(const QRectF & _from, const QRectF & _to, QTransform & _transform)
{
	QPolygonF _src_p = _from;
	QPolygonF _dst_p = _to;
	if (_src_p.count() > 4)
	{
		_src_p.remove(4, _src_p.count() - 4);
	}
	if (_dst_p.count() > 4)
	{
		_dst_p.remove(4, _dst_p.count() - 4);
	}

	return QTransform::quadToQuad(_src_p, _dst_p, _transform);
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
	qreal log10_mpy;
	bool log10_enabled;
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
		grid_pen = Qt::NoPen;
		id = 0;
	}
	~Data()
	{

	}
	double alignTick(double _value, double _alignment) const
	{
		return static_cast<int>(_value / _alignment) * _alignment;
	}
	QVector<double> calcTicks(double _lo, double _hi) const
	{
		if (alignment <= 0.0)
		{
			return QVector<double>();
		}
		if (_hi - _lo <= alignment)
		{
			return QVector<double>() << alignTick(_hi - _lo, alignment);
		}
		QVector<double> _ticks;
		for (double _value = alignTick(_lo, alignment); _value <= _hi; _value += alignment)
		{
			_ticks << _value;
		}
		unsigned int _mply = 1;
		while (_ticks.count() > count_hint)
		{
			_mply *= 2;
			for (unsigned int _idx = 0; _idx < _ticks.count(); _idx++)
			{
				if (alignTick(_ticks[_idx], alignment * _mply) != _ticks[_idx])
				{
					_ticks.remove(_idx);
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

QString jAxis::default_range_convert(double _value, jAxis *)
{
	return QString::number(_value, '.', 1);
}

jAxis & jAxis::setRange(double _lo, double _hi, jAxis::range_func _range_func)
{
	THREAD_SAFE(Write);
	d->lo = _lo;
	d->hi = _hi;
	d->range_func = _range_func ? _range_func : &jAxis::default_range_convert;
	THREAD_UNSAFE
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
	return SAFE_GET(d->range_func);
}

jAxis & jAxis::setStep(unsigned int _count_hint, double _alignment)
{
	THREAD_SAFE(Write)
	d->count_hint = _count_hint ? _count_hint : 1;
	d->alignment = (_alignment > 0.0) ? _alignment : 1.0;
	THREAD_UNSAFE
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

jAxis & jAxis::setTickLength(unsigned int _length)
{
	SAFE_SET(d->tick_length, _length);
	return * this;
}

unsigned int jAxis::tickLength() const
{
	return d->tick_length;
}

void jAxis::render(QPainter & _painter, const QRectF & _dst_rect, int _orientation, double _lo, double _hi)
{
	THREAD_SAFE(Read)
	if (d->visible == false)
	{
		THREAD_UNSAFE
		return;
	}
	QPen _pen = d->pen;
	QBrush _background = d->background;
	QPen _grid_pen = d->grid_pen;
	_painter.setPen(_pen);
	_painter.setFont(d->font);
	QFontMetrics _metrics = _painter.fontMetrics();
	range_func _range_func = d->range_func;
	const QRect _lo_rect = _metrics.boundingRect(_range_func(_lo, this));
	const QRect _hi_rect = _metrics.boundingRect(_range_func(_hi, this));
	THREAD_UNSAFE
	THREAD_SAFE(Write)
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
	THREAD_UNSAFE
	QVector<double> _ticks = d->calcTicks(_lo, _hi);
	if (_ticks.count() > 0)
	{
		while (_ticks.count() > d->count)
		{
			for (unsigned int _idx = 0; _idx < _ticks.count(); _idx++)
			{
				_ticks.remove(_idx);
			}
		}
	}
	THREAD_SAFE(Write)
	d->count = _ticks.count();
	THREAD_UNSAFE
	const unsigned int _tick_length = 5;
	switch (_orientation)
	{
	case Qt::Vertical:
		{
			const int _x = _tick_length + _pen.width();
			_painter.drawLine(
				_dst_rect.left() + _pen.width() / 2,
				_dst_rect.top(),
				_dst_rect.left() + _pen.width() / 2,
				_dst_rect.bottom()
				);
			for (unsigned int _idx = 0; _idx < _ticks.count(); _idx++)
			{
				int _y = _dst_rect.height() - ((_ticks[_idx] - _lo) * _dst_rect.height() / (_hi - _lo));
				_painter.drawLine(0, _y, _tick_length, _y);
				QString _str = _range_func(_ticks[_idx], this);
				QRect _rect = _metrics.boundingRect(_str);
				const int _w = _rect.width();
				const int _h = _rect.height();
				_y += _h / 2;
				if ((_y - _h >= 0) && (_y + (_h / 2) <= _dst_rect.height()))
				{
					_painter.fillRect(QRectF(QPointF(_x, _y - _h + 2), _rect.size()), _background);
					_painter.drawText(_x, _y, _str);
				}
				_painter.save();
				_painter.setPen(_grid_pen);
				_painter.drawLine(_x + _w, _y - _h / 2, _dst_rect.width(), _y - _h / 2);				
				_painter.restore();
			}
			break;
		}
	case Qt::Horizontal:
		{
			const int _y = _dst_rect.bottom() - _tick_length - _pen.width() * 1.5;
			_painter.drawLine(
				_dst_rect.left(),
				_dst_rect.bottom() - _pen.width() + 1,
				_dst_rect.right(),
				_dst_rect.bottom() - _pen.width() + 1
				);
			for (unsigned int _idx = 0; _idx < _ticks.count(); _idx++)
			{
				int _x = (_ticks[_idx] - _lo) * _dst_rect.width() / (_hi - _lo);
				_painter.drawLine(_x, _dst_rect.bottom(), _x, _y + _pen.width());
				QString _str = _range_func(_ticks[_idx], this);
				QRect _rect = _metrics.boundingRect(_str);
				const int _w = _rect.width();
				const int _h = _rect.height();
				_x -= _w / 2;
				if ((_x - (_w / 2) >= 0) && (_x + _w <= _dst_rect.width()))
				{
					_painter.fillRect(QRectF(QPointF(_x, _y - _h + 2), _rect.size()), _background);
					_painter.drawText(_x, _y, _str);
				}
				_painter.save();
				_painter.setPen(_grid_pen);
				_painter.drawLine(_x + _w / 2, 0, _x + _w / 2, _y - _h + 2);				
				_painter.restore();
			}
			break;
		}
	}
}

jAxis & jAxis::setPen(const QPen & _pen)
{
	SAFE_SET(d->pen, _pen);
	return * this;
}

QPen jAxis::pen() const
{
	return SAFE_GET(d->pen);
}

jAxis & jAxis::setFont(const QFont & _font)
{
	SAFE_SET(d->font, _font);
	return * this;
}

QFont jAxis::font() const
{
	return SAFE_GET(d->font);
}

jAxis & jAxis::setVisible(bool _state)
{
	SAFE_SET(d->visible, _state);
	return * this;
}

bool jAxis::isVisible() const
{
	return d->visible;
}

jAxis & jAxis::setBackground(const QBrush & _brush)
{
	SAFE_SET(d->background, _brush);
	return * this;
}

QBrush jAxis::background() const
{
	return SAFE_GET(d->background);
}

jAxis & jAxis::setGridPen(const QPen & _grid_pen)
{
	SAFE_SET(d->grid_pen, _grid_pen);
	return * this;
}

QPen jAxis::gridPen() const
{
	return SAFE_GET(d->grid_pen);
}

jAxis & jAxis::setId(int _id)
{
	SAFE_SET(d->id, _id);
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

jAxis & jAxis::setLog10Multiplier(qreal _mpy)
{
	d->log10_mpy = _mpy;
	return * this;
}

qreal jAxis::log10Multiplier() const
{
	return d->log10_mpy;
}

qreal jAxis::toLog10(qreal _value, qreal _minimum) const
{
	return (_value > 0.0) ? d->log10_mpy * ::log10f(_value) : _minimum;
}

qreal jAxis::fromLog10(qreal _value) const
{
	return (d->log10_mpy != 0.0) ? ::powf(10.0, _value / d->log10_mpy) : 0.0;
}

// ------------------------------------------------------------------------

struct jSelector::Data
{	
	QRectF rect;
	bool visible;
	QBrush background;
	jItem1D<qreal> item;
	jItem1D<qreal>::Point points[5];
	Data() 
	{
		points[0].x = 0; points[0].y = 0;
		points[1].x = 0; points[1].y = 0;
		points[2].x = 0; points[2].y = 0;
		points[3].x = 0; points[3].y = 0;
		points[4].x = 0; points[4].y = 0;
		visible = true;
		item.setDataModel(jItem1D<qreal>::PointData);
		item.setData(points, 5);
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
	THREAD_SAFE(Write)
	d->item.setPen(_pen);
	THREAD_UNSAFE
	return * this;
}

QPen jSelector::pen() const
{
	return SAFE_GET(d->item.pen());
}

jSelector & jSelector::setRect(const QRectF & _rect)
{
	THREAD_SAFE(Write)
	d->rect = _rect;
	d->points[4].x = _rect.left(); d->points[4].y = _rect.top();
	d->points[0].x = _rect.left(); d->points[0].y = _rect.top();
	d->points[1].x = _rect.right(); d->points[1].y = _rect.top();
	d->points[2].x = _rect.right(); d->points[2].y = _rect.bottom();
	d->points[3].x = _rect.left(); d->points[3].y = _rect.bottom();
	THREAD_UNSAFE
	return * this;
}

QRectF jSelector::rect() const
{
	return SAFE_GET(d->rect);
}

jSelector & jSelector::setVisible(bool _state)
{
	SAFE_SET(d->visible, _state);
	return * this;
}

bool jSelector::isVisible() const
{
	return d->visible;
}

void jSelector::render(QPainter & _painter, const QRectF & _dst_rect, const QRectF & _src_rect)
{
	THREAD_SAFE(Read)
	if (d->visible == false)
	{
		THREAD_UNSAFE
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
	THREAD_UNSAFE
}

jSelector & jSelector::setBackground(const QBrush & _brush)
{
	SAFE_SET(d->background, _brush);
	return * this;
}

QBrush jSelector::background() const
{
	return SAFE_GET(d->background);
}

// ------------------------------------------------------------------------

struct jZoom::Data
{	
	QRectF base;
	QVector<QRectF> history;
	jSelector selector;
	int orientation;
	QSizeF minimum_size, maximum_size;
	Data()
	{
		history << QRectF();
		selector.setVisible(false);
	}
	~Data()
	{

	}
	QRectF minmaxRect(QRectF _rect) const
	{
		if (_rect.size().width() < minimum_size.width())
		{
			const double _dx = minimum_size.width() - _rect.size().width();
			_rect.setLeft(_rect.left() - _dx / 2.0);			
			_rect.setRight(_rect.right() + _dx / 2.0);			
		}
		if (_rect.size().width() > maximum_size.width())
		{
			const double _dx = _rect.size().width() - maximum_size.width();
			_rect.setLeft(_rect.left() + _dx / 2.0);			
			_rect.setRight(_rect.right() - _dx / 2.0);			
		}
		if (_rect.size().height() < minimum_size.height())
		{
			const double _dy = minimum_size.height() - _rect.size().height();
			_rect.setTop(_rect.top() - _dy / 2.0);			
			_rect.setBottom(_rect.bottom() + _dy / 2.0);			
		}
		if (_rect.size().height() > maximum_size.height())
		{
			const double _dy = _rect.size().height() - maximum_size.height();
			_rect.setTop(_rect.top() + _dy / 2.0);			
			_rect.setBottom(_rect.bottom() - _dy / 2.0);			
		}
		return _rect;
	}
	QRectF adjustRect(const QRectF & _rect) const
	{
		QRectF _adj_rect = minmaxRect(_rect).intersected(base);
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
	void adjustHistory()
	{
		for (unsigned int _idx = 0; _idx < history.count(); _idx++)
		{
			history[_idx] = minmaxRect(history[_idx]);
		}
	}
};

jZoom::jZoom(): QObject(), d(new Data())
{
	setOrientation(Qt::Vertical | Qt::Horizontal);
}

jZoom::~jZoom()
{
	delete d;
}

void jZoom::clearHistory()
{
	THREAD_SAFE(Write)
	d->history.clear();
	THREAD_UNSAFE
}

jZoom & jZoom::setZoomBase(const QRectF & _rect)
{
	THREAD_SAFE(Write)
	clearHistory();
	if (d->maximum_size == QSizeF(-1, -1))
	{
		setMaximumSize(_rect.size());
	}
	d->base = _rect;
	d->history << d->minmaxRect(_rect);
	QRectF _history_back = d->history.back();
	THREAD_UNSAFE
	emit zoomedFullView(_history_back);
	return * this;
}

jZoom & jZoom::setZoomBase(const jAxis & _x_axis, const jAxis & _y_axis)
{
	return setZoomBase(QRectF(QPointF(_x_axis.lo(), _y_axis.lo()), QPointF(_x_axis.hi(), _y_axis.hi())));
}

QRectF jZoom::rectBase() const
{
	return SAFE_GET(d->base);
}

void jZoom::adjustZoomBase(const QRectF & _rect)
{
	if (_rect == SAFE_GET(d->base))
	{
		return;
	}
	THREAD_SAFE(Write)
	d->base = _rect;
	d->history[0] = d->minmaxRect(_rect);
	for (int _idx = 1; _idx < d->history.count(); _idx++)
	{
		d->history[_idx] = d->adjustRect(d->history[_idx]);
	}
	THREAD_UNSAFE
}

void jZoom::adjustZoomBase(const jAxis & _x_axis, const jAxis & _y_axis)
{
	adjustZoomBase(QRectF(QPointF(_x_axis.lo(), _y_axis.lo()), QPointF(_x_axis.hi(), _y_axis.hi())));
}

void jZoom::zoomIn(const QRectF & _rect)
{
	THREAD_SAFE(Write)
	QRectF _adj_rect = d->adjustRect(_rect);
	QVector<QRectF>::iterator _it = qFind(d->history.begin(), d->history.end(), _adj_rect);
	if (_it == (d->history.end() - 2))
	{
		d->history.erase(d->history.end() - 1);
		QRectF _history_back = d->history.back();
		THREAD_UNSAFE
		emit zoomedOut(_history_back);
	}
	else if (_it == d->history.end())
	{
		d->history << _adj_rect;
		QRectF _history_back = d->history.back();
		THREAD_UNSAFE
		emit zoomedIn(_history_back);
	}
	else if (_it == d->history.begin())
	{
		clearHistory();
		d->history << _adj_rect;
		QRectF _history_back = d->history.back();
		THREAD_UNSAFE
		emit zoomedFullView(_history_back);
	}
	else
	{
		THREAD_UNSAFE
	}
}

void jZoom::zoomOut()
{
	THREAD_SAFE(Write)
	if (d->history.count() > 1)
	{
		d->history.erase(d->history.end() - 1);
		QRectF _history_back = d->history.back();
		THREAD_UNSAFE
		emit zoomedOut(_history_back);
	}
	else
	{
		THREAD_UNSAFE
	}
}

void jZoom::zoomFullView()
{
	THREAD_SAFE(Write)
	d->history.erase(d->history.begin() + 1, d->history.end());
	QRectF _history_back = d->history.back();
	THREAD_UNSAFE
	emit zoomedFullView(_history_back);
}

QRectF jZoom::rect() const
{
	return SAFE_GET(d->history.back());
}

QVector<QRectF> jZoom::history() const
{
	return SAFE_GET(d->history);
}

jSelector & jZoom::selector() const
{
	return d->selector;
}

void jZoom::pan(double _dx, double _dy)
{
	THREAD_SAFE(Write)
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
	d->history.back() = d->adjustRect(QRectF(QPointF(_rect.left() + _dx, _rect.top() + _dy), _rect.size()));
	QRectF _history_back = d->history.back();
	THREAD_UNSAFE
	emit panned(_history_back);
}

jZoom & jZoom::setOrientation(int _orientation)
{
	SAFE_SET(d->orientation, _orientation);
	return * this;
}

int jZoom::orientation() const
{
	return d->orientation;
}

QRectF jZoom::adjustRect(const QRectF & _rect) const
{
	return SAFE_GET(d->adjustRect(_rect));
}

jZoom & jZoom::setMinimumSize(const QSizeF & _size)
{
	THREAD_SAFE(Write)
	d->minimum_size = _size;
	d->adjustHistory();
	THREAD_UNSAFE
	return * this;
}

QSizeF jZoom::minimumSize() const
{
	return SAFE_GET(d->minimum_size);
}

jZoom & jZoom::setMaximumSize(const QSizeF & _size)
{
	THREAD_SAFE(Write)
	d->maximum_size = _size;
	d->adjustHistory();
	THREAD_UNSAFE
	return * this;
}

QSizeF jZoom::maximumSize() const
{
	return SAFE_GET(d->maximum_size);
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
	Data()
	{
		visible = true;
	}
	~Data()
	{

	}
};

jLabel::jLabel(const QString & _text): d(new Data())
{
	d->text = _text;
}

jLabel::~jLabel()
{
	delete d;
}

jLabel & jLabel::setText(const QString & _text)
{
	SAFE_SET(d->text, _text);
	return * this;
}

QString jLabel::text() const
{
	return SAFE_GET(d->text);
}

jLabel & jLabel::setPen(const QPen & _pen)
{
	SAFE_SET(d->pen, _pen);
	return * this;
}

QPen jLabel::pen() const
{
	return SAFE_GET(d->pen);
}

jLabel & jLabel::setFont(const QFont & _font)
{
	SAFE_SET(d->font, _font);
	return * this;
}

QFont jLabel::font() const
{
	return SAFE_GET(d->font);
}

jLabel & jLabel::setPos(const QPointF & _pos)
{
	SAFE_SET(d->pos, _pos);
	return * this;
}

QPointF jLabel::pos() const
{
	return SAFE_GET(d->pos);
}

jLabel & jLabel::setSize(const QSizeF & _size)
{
	SAFE_SET(d->size, _size);
	return * this;
}

QSizeF jLabel::size() const
{
	return SAFE_GET(d->size);
}

jLabel & jLabel::setVisible(bool _state)
{
	SAFE_SET(d->visible, _state);
	return * this;
}

bool jLabel::isVisible() const
{
	return d->visible;
}

jLabel & jLabel::setOptions(const QTextOption & _options)
{
	SAFE_SET(d->options, _options);
	return * this;
}

QTextOption jLabel::options() const
{
	return SAFE_GET(d->options);
}

jLabel & jLabel::setBackground(const QBrush & _brush)
{
	SAFE_SET(d->background, _brush);
	return * this;
}

QBrush jLabel::background() const
{
	return SAFE_GET(d->background);
}

void jLabel::render(QPainter & _painter, const QRectF & _dst_rect, const QRectF & _src_rect)
{
	THREAD_SAFE(Read)
	if (d->visible == false)
	{
		THREAD_UNSAFE
		return;
	}
	QTransform _transform;
	if (::jQuadToQuad(_src_rect, _dst_rect, _transform))
	{
		_painter.setPen(d->pen);
		_painter.setFont(d->font);
		QRectF _rect = QRectF(_transform.map(d->pos), d->size);
		_painter.fillRect(_rect, d->background);
		_painter.drawText(_rect, d->text, d->options);
	}
	THREAD_UNSAFE
}

QSizeF jLabel::sizeHint() const
{
	THREAD_SAFE(Read)
	double _w = 0, _h = 0;
	const QFontMetricsF _fm(d->font);
	const QString _text = d->text;
	THREAD_UNSAFE
	foreach (const QString & _str, _text.split("\n"))
	{
		_w = qMax<double>(_w, _fm.width(_str));
		_h += _fm.height();
	}
	return QSizeF(_w, _h);
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
	QString _x_str = "x: " + (_x_axis ? _x_axis->rangeFunc()(_x, _x_axis) : QString::number(_x, '.', 1));
	QString _y_str = "y: " + (_y_axis ? _y_axis->rangeFunc()(_y, _y_axis) : QString::number(_y, '.', 1));
	return _x_str + "\n" + _y_str;
}

jCoordinator & jCoordinator::setFormat(jCoordinator::format_func _format_func)
{
	THREAD_SAFE(Write)
	d->format_func = _format_func ? _format_func : &jCoordinator::default_format;
	THREAD_UNSAFE
	return * this;
}

jCoordinator::format_func jCoordinator::formatFunc() const
{
	return SAFE_GET(d->format_func);
}

jLabel & jCoordinator::label() const
{
	return d->label;
}

jCoordinator & jCoordinator::setPos(const QPointF & _pos)
{
	SAFE_SET(d->pos, _pos);
	return * this;
}

QPointF jCoordinator::pos() const
{
	return SAFE_GET(d->pos);
}

jCoordinator & jCoordinator::setOffset(const QPointF & _offset)
{
	SAFE_SET(d->offset, _offset);
	return * this;
}

QPointF jCoordinator::offset() const
{
	return SAFE_GET(d->offset);
}

void jCoordinator::render(QPainter & _painter, const QRectF & _dst_rect, const QRectF & _src_rect, const jAxis * _x_axis, const jAxis * _y_axis)
{
	THREAD_SAFE(Read)
	QPointF _pos = d->pos;
	QPointF _offset = d->offset;
	format_func _format_func = d->format_func;
	d->label.
		setText(_format_func(_pos.x(), _pos.y(), (jAxis *)_x_axis, (jAxis *)_y_axis, this));
	d->label.
		setSize(d->label.sizeHint()).
		setPos(QPointF(_pos.x(), _src_rect.top() + _src_rect.bottom() - _pos.y()));

	QSizeF _size = d->label.size();
	THREAD_UNSAFE

	QTransform _transform;
	if (::jQuadToQuad(_src_rect, _dst_rect, _transform))
	{
		QRectF _rect = QRectF(_transform.map(_pos), _size);
		_rect.moveTo(QPointF(_dst_rect.left() + _offset.x() + _rect.left(), _dst_rect.top() - _offset.y() + _rect.top() - _rect.height()));
		
		qreal _x = 0, _y = 0;
		if (_dst_rect.contains(_rect) == false)
		{
			if (_dst_rect.right() < _rect.right())
			{
				_x -= _offset.x() + _size.width();
			}
			if (_dst_rect.top() > _rect.top())
			{
				_y -= _offset.y() + _size.height();
			}
		}
		if (_x == 0)
		{
			_x += _offset.x() + _dst_rect.left();
		}
		if (_y == 0)
		{
			_y += _offset.y() + _dst_rect.top();
		}

		d->label.
			render(_painter, QRectF(_x, _y, _dst_rect.width(), _dst_rect.height()), _src_rect);
	}
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
	Data() 
	{
		counter = 0;
		data = 0;
		width = 0;
		height = 0;
		bytes_per_item = 0;
		z = 0;
		deep_copy = false;
		visible = true;
	}
	~Data()
	{
		clear();
	}
	void clear()
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

}

jItem::~jItem()
{
	delete d;
}

jItem & jItem::setBytesPerItem(unsigned int _bytes_per_item)
{
	SAFE_SET(d->bytes_per_item, _bytes_per_item);
	return * this;
}

jItem & jItem::setData(void * _data, unsigned int _width, unsigned int _height, bool _deep_copy)
{
	THREAD_SAFE(Write)
	d->clear();
	d->width = _width;
	d->height = _height;
	d->deep_copy = _deep_copy;
	if (!_deep_copy)
	{
		d->data = _data;
	}
	else
	{
		unsigned int _data_size = _width * _height * d->bytes_per_item;
		d->data = new char [_data_size];
		::memcpy(d->data, _data, _data_size);
	}
	THREAD_UNSAFE
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
	SAFE_SET(d->pen, _pen);
	return * this;
}

QPen jItem::pen() const
{
	return SAFE_GET(d->pen);
}

jItem & jItem::setVisible(bool _state)
{
	SAFE_SET(d->visible, _state);
	return * this;
}

bool jItem::isVisible() const
{
	return d->visible;
}

jItem & jItem::setBrush(const QBrush & _brush)
{
	SAFE_SET(d->brush, _brush);
	return * this;
}

QBrush jItem::brush() const
{
	return SAFE_GET(d->brush);
}

jItem & jItem::setOrigin(const QPointF & _origin)
{
	SAFE_SET(d->origin, _origin);
	return * this;
}

QPointF jItem::origin() const
{
	return SAFE_GET(d->origin);
}

jItem & jItem::setZ(double _z)
{
	SAFE_SET(d->z, _z);
	return * this;
}

double jItem::z() const
{
	return d->z;
}

bool jItem::intersects(const QRectF &, const jAxis *, const jAxis *) const
{
	return false;
}

jItem & jItem::setToolTip(const QString & _text)
{
	SAFE_SET(d->tooltip, _text);
	return * this;
}

QString jItem::toolTip() const
{
	return SAFE_GET(d->tooltip);
}

void jItem::updateViewport(const QRectF &)
{
}

void jItem::renderPreview(QPainter & _painter, const QRectF & _dst_rect, const QRectF & _src_rect, const jAxis * _x_axis, const jAxis * _y_axis)
{
	render(_painter, _dst_rect, _src_rect, _x_axis, _y_axis);
}

quint64 jItem::counter() const
{
	return d->counter;
}

void jItem::addCounter(quint64 _count)
{
	SAFE_SET(d->counter, d->counter + _count);
}

QRectF jItem::boundingRect(const jAxis * _x_axis, const jAxis * _y_axis) const
{
	QSizeF _size = size();
	qreal _w = _size.width();
	qreal _h = _size.height();
	if (_x_axis && _x_axis->isLog10ScaleEnabled())
	{
		_w = _x_axis->toLog10(_w);
	}
	if (_y_axis && _y_axis->isLog10ScaleEnabled())
	{
		_h = _y_axis->toLog10(_h);
	}
	return QRectF(d->origin, QSizeF(_w, _h));
}

// ------------------------------------------------------------------------

struct jMarker::Data
{
	int orientation;
	qreal value;
	jItem1D<qreal> item;
	jItem1D<qreal>::Point points[2];
	Data()
	{
		points[0].x = 0; points[0].y = 0;
		points[1].x = 0; points[1].y = 0;
		value = 0.0;
		orientation = Qt::Vertical;
		item.setDataModel(jItem1D<double>::PointData);
		item.setData(points, 2);
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
	THREAD_SAFE(Write)
	d->item.setPen(_pen);
	THREAD_UNSAFE
	return * this;
}

QPen jMarker::pen() const
{
	return SAFE_GET(d->item.pen());
}

jMarker & jMarker::setValue(qreal _value)
{
	SAFE_SET(d->value, _value);
	return * this;
}

qreal jMarker::value() const
{
	return d->value;
}

jMarker & jMarker::setVisible(bool _state)
{
	THREAD_SAFE(Write)
	d->item.setVisible(_state);
	THREAD_UNSAFE
	return * this;
}

bool jMarker::isVisible() const
{
	return d->item.isVisible();
}

jMarker & jMarker::setOrientation(int _orientation)
{
	SAFE_SET(d->orientation, _orientation);
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
			d->points[0].x = _value; d->points[0].y = _src_rect.top();
			d->points[1].x = _value; d->points[1].y = _src_rect.bottom();
			break;
		}
	case Qt::Horizontal:
		{
			d->points[0].x = _src_rect.left(); d->points[0].y = _value;
			d->points[1].x = _src_rect.right(); d->points[1].y = _value;
			break;
		}
	}
	d->item.render(_painter, _dst_rect, _src_rect);
}

// ------------------------------------------------------------------------

struct jView::Data
{
	jAxis * x_axis, * y_axis;
	jZoom zoomer;
	jCoordinator coordinator;
	jMarker hmarker, vmarker;
	jStack<bool> coordinator_visibility, hmarker_visibility, vmarker_visibility;
	QVector<jItem *> items;
	QVector<jLabel *> labels;
	QVector<jMarker *> markers;
	QVector<jSelector *> selectors;
	QBrush background;
	QPointF press_point, release_point, move_point;
	bool in_zoom;
	QCursor before_pan_cursor;
	jLazyRenderer * renderer;
	Data()
	{
		x_axis = 0;
		y_axis = 0;
		in_zoom = false;
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
	}
	QTransform screenToAxisTransform(const QRectF & _screen_rect) const
	{
		QTransform _transform;
		if (x_axis && y_axis)
		{
			::jQuadToQuad(_screen_rect, zoomer.rect(), _transform);
		}
		return _transform;
	}
	QRectF screenToAxis(const QRectF & _screen_rect, const QRectF & _src_rect) const
	{
		return screenToAxisTransform(_screen_rect).mapRect(_src_rect);
	}
	QPointF screenToAxis(const QRectF & _screen_rect, const QPointF & _src_point) const
	{
		return screenToAxisTransform(_screen_rect).map(_src_point);
	}
	QTransform axisToScreenTransform(const QRectF & _screen_rect) const
	{
		QTransform _transform;
		if (x_axis && y_axis)
		{
			::jQuadToQuad(zoomer.rect(), _screen_rect, _transform);
		}
		return _transform;
	}
	QRectF axisToScreen(const QRectF & _src_rect, const QRectF & _screen_rect) const
	{
		return axisToScreenTransform(_screen_rect).mapRect(_src_rect);
	}
	QPointF axisToScreen(const QPointF & _src_point, const QRectF & _screen_rect) const
	{
		return axisToScreenTransform(_screen_rect).map(_src_point);
	}
	void setZoomBase()
	{
		if (x_axis && y_axis)
		{
			zoomer.setZoomBase(* x_axis, * y_axis);
		}
	}
	static bool itemZSort(const jItem * _item1, const jItem * _item2)
	{
		if (_item1 && _item2)
		{
			return (_item1->z() < _item2->z());
		}
		return false;
	}
	void adjustCoordinator(const QRectF & _screen_rect, const QPointF & _local_pt)
	{
		QPointF _axis_pt = screenToAxis(_screen_rect, _local_pt);
		if (hmarker.isVisible())
		{
			hmarker.setValue(_axis_pt.y());
		}
		if (vmarker.isVisible())
		{
			vmarker.setValue(_axis_pt.x());
		}
		if (coordinator.label().isVisible())
		{
			coordinator.setPos(_axis_pt);
		}
	}
	void updateViewports(const QRectF & _rect)
	{
		foreach (jItem * _item, items)
		{
			_item->updateViewport(_rect);
		}
	}
	static void render_func(QWidget * _widget, QPainter & _painter)
	{
		dynamic_cast<jView *>(_widget)->render(_painter);
	}
	void init(QWidget * _instance)
	{
		zoomer.setParent(_instance);

		QObject::connect(&zoomer, SIGNAL(zoomedIn(QRectF)), _instance, SIGNAL(viewportChanged(QRectF)));
		QObject::connect(&zoomer, SIGNAL(zoomedOut(QRectF)), _instance, SIGNAL(viewportChanged(QRectF)));
		QObject::connect(&zoomer, SIGNAL(zoomedFullView(QRectF)), _instance, SIGNAL(viewportChanged(QRectF)));
		QObject::connect(&zoomer, SIGNAL(panned(QRectF)), _instance, SIGNAL(viewportChanged(QRectF)));

		renderer = new jLazyRenderer(_instance, &Data::render_func);
		renderer->
			setMaxThreads(2).
			setEnabled(false);
		_instance->installEventFilter(renderer);

		_instance->setMouseTracking(true);
		_instance->setCursor(Qt::CrossCursor);
		_instance->setFocusPolicy(Qt::WheelFocus);
	}
};

jView::jView(QWidget * _parent)
	: JUNGLE_WIDGET_CLASS(_parent), d(new Data())
{
	d->init(this);
}

jView::jView(jAxis * _x_axis, jAxis * _y_axis, QWidget * _parent)
	: JUNGLE_WIDGET_CLASS(_parent), d(new Data())
{
	d->init(this);

	this->
		setXAxis(_x_axis).
		setYAxis(_y_axis);
}

jView::~jView()
{
	setVisible(false);
	d->zoomer.setParent(0);
	removeEventFilter(d->renderer);
	delete d->renderer;
	delete d;
}

jView & jView::setXAxis(jAxis * _axis)
{
	THREAD_SAFE(Write)
	if (d->x_axis != _axis)
	{
		d->x_axis = _axis;
		d->setZoomBase();
	}
	THREAD_UNSAFE
	return * this;
}

jAxis * jView::xAxis() const
{
	return d->x_axis;
}

jView & jView::setYAxis(jAxis * _axis)
{
	THREAD_SAFE(Write)
	if (d->y_axis != _axis)
	{
		d->y_axis = _axis;
		d->setZoomBase();
	}
	THREAD_UNSAFE
	return * this;
}

jAxis * jView::yAxis() const
{
	return d->y_axis;
}

jZoom & jView::zoomer() const
{
	return d->zoomer;
}

jView & jView::addItem(jItem * _item)
{
	THREAD_SAFE(Write)
	d->items << _item;
	THREAD_UNSAFE
	return * this;
}

jView & jView::addItems(const QVector<jItem *> & _items)
{
	THREAD_SAFE(Write)
	d->items << _items;
	THREAD_UNSAFE
	return * this;
}

jView & jView::setItem(jItem * _item)
{
	SAFE_SET(d->items, QVector<jItem *>() << _item);
	return * this;
}

jView & jView::setItems(const QVector<jItem *> & _items)
{
	SAFE_SET(d->items, _items);
	return * this;
}

jView & jView::removeItem(jItem * _item)
{
	THREAD_SAFE(Write)
	QVector<jItem *>::iterator _it = ::qFind(d->items.begin(), d->items.end(), _item);
	if (_it)
	{
		d->items.erase(_it);
	}
	THREAD_UNSAFE
	return * this;
}

jView & jView::removeItems(const QVector<jItem *> & _items)
{
	THREAD_SAFE(Write)
	foreach (jItem * _item, _items)
	{
		QVector<jItem *>::iterator _it = ::qFind(d->items.begin(), d->items.end(), _item);
		if (_it)
		{
			d->items.erase(_it);
		}
	}
	THREAD_UNSAFE
	return * this;
}

void jView::clear()
{
	THREAD_SAFE(Write)
	d->items.clear();
	THREAD_UNSAFE
}

QVector<jItem *> jView::items() const
{
	return SAFE_GET(d->items);
}

jView & jView::setBackground(const QBrush & _brush)
{
	SAFE_SET(d->background, _brush);
	return * this;
}

QBrush jView::background() const
{
	return SAFE_GET(d->background);
}

void jView::render(QPainter & _painter) const
{
	THREAD_SAFE(Read)
	QVector<jItem *> _items = d->items;
	QRectF _rect = rect();
	QRectF _zoomer_rect = d->zoomer.rect();
	jAxis * _x_axis = d->x_axis;
	jAxis * _y_axis = d->y_axis;
	QVector<jMarker *> _markers = d->markers;
	QVector<jLabel *> _labels = d->labels;
	QVector<jSelector *> _selectors = d->selectors;
	if (d->background.style() != Qt::NoBrush)
	{
		_painter.fillRect(_rect, d->background);
	}
	::qSort(_items.begin(), _items.end(), &Data::itemZSort);
	foreach (jItem * _item, _items)
	{
		_item->render(_painter, _rect, _zoomer_rect, _x_axis, _y_axis);
	}
	foreach (jSelector * _selector, _selectors)
	{
		_selector->render(_painter, _rect, _zoomer_rect);
	}
	if (_x_axis)
	{
		_x_axis->render(
			_painter, 
			_rect, 
			Qt::Horizontal,
			_zoomer_rect.left(),
			_zoomer_rect.right()
			);
	}
	if (_y_axis)
	{
		_y_axis->render(
			_painter, 
			_rect, 
			Qt::Vertical,
			_zoomer_rect.top(),
			_zoomer_rect.bottom()
			);
	}
	foreach (jMarker * _marker, _markers)
	{
		_marker->render(_painter, _rect, _zoomer_rect);
	}
	d->hmarker.render(_painter, _rect, _zoomer_rect);
	d->vmarker.render(_painter, _rect, _zoomer_rect);
	foreach (jLabel * _label, _labels)
	{
		_label->render(_painter, _rect, _zoomer_rect);
	}
	if (d->in_zoom)
	{
		d->zoomer.selector().render(_painter, _rect, _zoomer_rect);
	}
	d->coordinator.render(_painter, _rect, _zoomer_rect, d->x_axis, d->y_axis);
	THREAD_UNSAFE
}

void jView::mousePressEvent(QMouseEvent *_me)
{
	d->press_point = _me->pos();
	d->press_point.setY(rect().height() - d->press_point.y());

	d->move_point = d->press_point;
	if (_me->button() == Qt::LeftButton)
	{
		d->in_zoom = true;
		d->zoomer.selector().setRect(QRectF());
		d->zoomer.selector().setVisible(true);
	}
	if (_me->button() == Qt::RightButton)
	{
		d->before_pan_cursor = cursor();
		setCursor(Qt::OpenHandCursor);
	}
	d->renderer->rebuild();
}

void jView::mouseMoveEvent(QMouseEvent * _me)
{
	QPointF _move_point = _me->posF();
	_move_point.setY(rect().height() - _move_point.y());
	if (d->in_zoom)
	{
		d->zoomer.selector().setRect(d->zoomer.adjustRect(d->screenToAxis(rect(), QRectF(d->press_point, _move_point))));
	}
	if ((_me->buttons() & Qt::RightButton) == Qt::RightButton)
	{
		QPointF _p1 = d->screenToAxis(rect(), d->move_point);
		QPointF _p2 = d->screenToAxis(rect(), _move_point);
		d->zoomer.pan(_p1.x() - _p2.x(), _p1.y() - _p2.y());
		d->updateViewports(d->zoomer.rect());
		setCursor(Qt::ClosedHandCursor);
	}
	d->adjustCoordinator(rect(), _move_point);
	d->move_point = _move_point;
	d->renderer->rebuild();
}

void jView::mouseReleaseEvent(QMouseEvent *_me)
{
	d->release_point = _me->pos();
	d->release_point.setY(rect().height() - d->release_point.y());

	if (_me->button() == Qt::LeftButton)
	{
		d->in_zoom = false;
		d->zoomer.selector().setVisible(false);

		if (d->press_point.x() != d->release_point.x())
		{
			if ((d->press_point.x() > d->release_point.x()) && (d->press_point.y() < d->release_point.y()))
			{
				d->zoomer.zoomOut();
			}
			else
			{
				d->zoomer.zoomIn(d->screenToAxis(rect(), QRectF(d->press_point, d->release_point)));
			}
			d->updateViewports(d->zoomer.rect());
		}
	}
	if (_me->button() == Qt::RightButton)
	{
		setCursor(d->before_pan_cursor);
		if (d->press_point == d->release_point)
		{
			emit contextMenuRequested(_me->globalPos());
		}
	}
	d->adjustCoordinator(rect(), d->release_point);
	d->renderer->rebuild();
}

void jView::mouseDoubleClickEvent(QMouseEvent * _me)
{
	if (_me->button() == Qt::LeftButton)
	{
		d->zoomer.zoomFullView();
		d->updateViewports(d->zoomer.rect());
		d->renderer->rebuild();
	}
}

void jView::wheelEvent(QWheelEvent * _we)
{
	QPointF _wheel_point = _we->pos();
	_wheel_point.setY(rect().height() - _wheel_point.y());
	QPointF _axis_point = d->screenToAxis(rect(), QPointF(_we->pos().x(), 0));
	QRectF _rect;
	QRectF _zoom_rect = d->zoomer.rect();
	double _k = (_axis_point.x() - _zoom_rect.left()) / _zoom_rect.width();
	if (_we->delta() > 0)
	{
		_rect = QRectF(QPointF(_axis_point.x() - (_zoom_rect.width() * _k) / 2.0, _axis_point.y()) , 
			QSizeF(_zoom_rect.width() / 2.0, _zoom_rect.height()));
	}
	if (_we->delta() < 0)
	{
		if (_zoom_rect.width() * 2.0 > d->zoomer.maximumSize().width())
		{
			return;
		}
		_rect =	QRectF(QPointF(_axis_point.x() - (_zoom_rect.width() * _k) * 2.0, _axis_point.y()) , 
			QSizeF(_zoom_rect.width() * 2.0, _zoom_rect.height()));
	}
	d->zoomer.zoomIn(_rect);
	d->updateViewports(d->zoomer.rect());
	d->adjustCoordinator(rect(), _wheel_point);
	d->renderer->rebuild();
}

void jView::keyPressEvent(QKeyEvent * _ke)
{
	QPoint _pt_current = QCursor::pos();
	QPoint _pt_new = _pt_current;
	switch (_ke->key())
	{
	case Qt::Key_Left:
		{
			_pt_new.setX(_pt_current.x() - 1);
			break;
		}
	case Qt::Key_Right:
		{
			_pt_new.setX(_pt_current.x() + 1);
			break;
		}
	case Qt::Key_Up:
		{
			_pt_new.setY(_pt_current.y() - 1);
			break;
		}
	case Qt::Key_Down:
		{
			_pt_new.setY(_pt_current.y() + 1);
			break;
		}
	}

	if (rect().contains(mapFromGlobal(_pt_new)))
	{
		QCursor::setPos(_pt_new);
	}
	else
	{
		QPointF _p1 = d->screenToAxis(rect(), _pt_new);
		QPointF _p2 = d->screenToAxis(rect(), _pt_current);
		d->zoomer.pan(_p1.x() - _p2.x(), - _p1.y() + _p2.y());
		d->updateViewports(d->zoomer.rect());
		QPoint _local_pt = mapFromGlobal(_pt_current);
		_local_pt.setY(rect().height() - _local_pt.y());
		d->adjustCoordinator(rect(), _local_pt);
		d->renderer->rebuild();
	}
}

jView & jView::setLabels(const QVector<jLabel *> & _labels)
{
	SAFE_SET(d->labels, _labels);
	return * this;
}

QVector<jLabel *> jView::labels() const
{
	return SAFE_GET(d->labels);
}

jView & jView::setSelectors(const QVector<jSelector *> & _selectors)
{
	SAFE_SET(d->selectors, _selectors);
	return * this;
}

QVector<jSelector *> jView::selectors() const
{
	return SAFE_GET(d->selectors);
}

jCoordinator & jView::coordinator() const
{
	return d->coordinator;
}

void jView::enterEvent(QEvent *)
{
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

jView & jView::setMarkers(const QVector<jMarker *> & _markers)
{
	SAFE_SET(d->markers, _markers);
	return * this;
}

QVector<jMarker *> jView::markers() const
{
	return SAFE_GET(d->markers);
}

jMarker & jView::horizontalMarker() const
{
	return d->hmarker;
}

jMarker & jView::verticalMarker() const
{
	return d->vmarker;
}

jLazyRenderer & jView::lazyRenderer() const
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
	THREAD_SAFE(Read)
	QVector<jItem *> _items;
	foreach (jItem * _item, d->items)
	{
		if (_exclude_invisible && (_item->isVisible() == false))
		{
			continue;
		}
		if (_item->intersects(d->screenToAxis(rect(), QRectF(QPointF(_point.x() - 2, rect().height() - _point.y() - 2), QSizeF(4, 4))), d->x_axis, d->y_axis))
		{
			_items << _item;
		}
	}
	THREAD_UNSAFE
	return _items;
}

QVector<jItem *> jView::showToolTip(const QPointF & _point)
{
	THREAD_SAFE(Read)
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
	THREAD_UNSAFE
	return _items;
}

void jView::rebuild()
{
	d->renderer->rebuild();
}

QRectF jView::itemsBoundingRect(bool _exclude_invisible) const
{
	THREAD_SAFE(Read)
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
	THREAD_UNSAFE
	return _united;
}

void jView::autoScaleX()
{
	if (d->x_axis && d->y_axis)
	{
		QRectF _bounding_rect = itemsBoundingRect();
		d->x_axis->setRange(_bounding_rect.left(), _bounding_rect.right(), d->x_axis->rangeFunc());
		d->zoomer.adjustZoomBase(* d->x_axis, * d->y_axis);
	}
}

void jView::autoScaleY()
{
	if (d->x_axis && d->y_axis)
	{
		QRectF _bounding_rect = itemsBoundingRect();
		d->y_axis->setRange(_bounding_rect.top(), _bounding_rect.bottom(), d->y_axis->rangeFunc());
		d->zoomer.adjustZoomBase(* d->x_axis, * d->y_axis);
	}
}

void jView::autoScale()
{
	if (d->x_axis && d->y_axis)
	{
		QRectF _bounding_rect = itemsBoundingRect();
		d->x_axis->setRange(_bounding_rect.left(), _bounding_rect.right(), d->x_axis->rangeFunc());
		d->y_axis->setRange(_bounding_rect.top(), _bounding_rect.bottom(), d->y_axis->rangeFunc());
		d->zoomer.adjustZoomBase(* d->x_axis, * d->y_axis);
	}
}


// ------------------------------------------------------------------------

struct jPreview::Data
{
	jView * view;
	jSelector selector;
	QBrush background;
	int orientation;
	QPoint prev_point, press_point;
	jLazyRenderer * renderer;
	Data()
	{
		view = 0;
		orientation = Qt::Vertical | Qt::Horizontal;
	}
	~Data()
	{

	}
	static bool itemZSort(const jItem * _item1, const jItem * _item2)
	{
		if (_item1 && _item2)
		{
			return (_item1->z() < _item2->z());
		}
		return false;
	}
	void updateSelector()
	{
		if (view)
		{
			QRectF _rect = view->zoomer().rect();
			QRectF _base = view->zoomer().rectBase();
			if ((orientation & Qt::Vertical) == 0)
			{
				_rect.setTop(_base.top());
				_rect.setBottom(_base.bottom());
			}
			if ((orientation & Qt::Horizontal) == 0)
			{
				_rect.setLeft(_base.left());
				_rect.setRight(_base.right());
			}
			selector.setRect(_rect);
		}
	}
	QPointF previewToView(const QRectF & _rect, const QPointF & _point) const
	{
		QTransform _transform;
		if ((view == 0) || (::jQuadToQuad(_rect, view->zoomer().rectBase(), _transform) == false))
		{
			return QPointF();
		}
		return view->axisToScreen(_transform.map(QPointF(_point.x(), _rect.height() - _point.y())));
	}
	void setToPosition(const QRect & _rect, const QMouseEvent * _me)
	{
		if (view == 0)
		{
			return;
		}
		QRectF _zoomer_rect_base = view->zoomer().rectBase();
		QTransform _transform;
		if (::jQuadToQuad(_rect, _zoomer_rect_base, _transform) == false)
		{
			return;
		}
		QRectF _zoomer_rect = view->zoomer().rect();
		if ((orientation & Qt::Vertical) == 0)
		{
			_zoomer_rect.setTop(_zoomer_rect_base.top());
			_zoomer_rect.setBottom(_zoomer_rect_base.bottom());
		}
		if ((orientation & Qt::Horizontal) == 0)
		{
			_zoomer_rect.setLeft(_zoomer_rect_base.left());
			_zoomer_rect.setRight(_zoomer_rect_base.right());
		}
		QPointF _axis_pt = _transform.map(_me->pos());
		_axis_pt.setY(_zoomer_rect_base.top() + _zoomer_rect_base.bottom() - _axis_pt.y());
		if (_zoomer_rect.contains(_axis_pt))
		{
			return;
		}
		qreal _delta_x = _axis_pt.x() - (_zoomer_rect.x() + _zoomer_rect.width() / 2);
		qreal _delta_y = _axis_pt.y() - (_zoomer_rect.y() + _zoomer_rect.height() / 2);
		if ((orientation & Qt::Vertical) == 0)
		{
			_delta_y = 0; 
		}
		if ((orientation & Qt::Horizontal) == 0)
		{
			_delta_x = 0; 
		}
		view->zoomer().pan(_delta_x, _delta_y);
		view->lazyRenderer().rebuild();
	}
	void pan(const QRect & _rect, const QMouseEvent * _me)
	{
		if (view == 0)
		{
			return;
		}
		QRectF _zoomer_rect_base = view->zoomer().rectBase();
		QTransform _transform;
		if (::jQuadToQuad(_rect, _zoomer_rect_base, _transform) == false)
		{
			return;
		}
		QRectF _zoomer_rect = view->zoomer().rect();
		if ((orientation & Qt::Vertical) == 0)
		{
			_zoomer_rect.setTop(_zoomer_rect_base.top());
			_zoomer_rect.setBottom(_zoomer_rect_base.bottom());
		}
		if ((orientation & Qt::Horizontal) == 0)
		{
			_zoomer_rect.setLeft(_zoomer_rect_base.left());
			_zoomer_rect.setRight(_zoomer_rect_base.right());
		}
		QPointF _axis_pt = _transform.map(_me->pos());
		_axis_pt.setY(_zoomer_rect_base.top() + _zoomer_rect_base.bottom() - _axis_pt.y());
		QPointF _axis_prev_pt = _transform.map(prev_point);
		_axis_prev_pt.setY(_zoomer_rect_base.top() + _zoomer_rect_base.bottom() - _axis_prev_pt.y());
		qreal _delta_x = _axis_pt.x() - _axis_prev_pt.x();
		qreal _delta_y = _axis_pt.y() - _axis_prev_pt.y();
		if ((orientation & Qt::Vertical) == 0)
		{
			_delta_y = 0; 
		}
		if ((orientation & Qt::Horizontal) == 0)
		{
			_delta_x = 0; 
		}
		view->zoomer().pan(_delta_x, _delta_y);
		view->lazyRenderer().rebuild();
	}
        void zoomFullView(const QRectF &, const QMouseEvent *)
	{
		if (view == 0)
		{
			return;
		}
		view->zoomer().zoomFullView();
		view->lazyRenderer().rebuild();
	}
	void wheelScale(const QRectF & _rect, const QWheelEvent * _we)
	{
		if (view == 0)
		{
			return;
		}
		QWheelEvent * _wheel = new QWheelEvent(
			previewToView(_rect, _we->pos()).toPoint(), 
			_we->delta(),
			_we->buttons(),
			_we->modifiers(),
			_we->orientation()
			);
		QCoreApplication::postEvent(view, _wheel);
	}
	static void render_func(QWidget * _widget, QPainter & _painter)
	{
		dynamic_cast<jPreview *>(_widget)->render(_painter);
	}
};

jPreview::jPreview(QWidget * _parent)
	: JUNGLE_WIDGET_CLASS(_parent), d(new Data())
{
	d->renderer = new jLazyRenderer(this, &Data::render_func);
	d->renderer->
		setMaxThreads(1).
		setEnabled(false);
	installEventFilter(d->renderer);
}

jPreview::jPreview(jView * _view, QWidget * _parent)
	: JUNGLE_WIDGET_CLASS(_parent), d(new Data())
{
	d->renderer = new jLazyRenderer(this, &Data::render_func);
	d->renderer->
		setMaxThreads(1).
		setEnabled(false);
	installEventFilter(d->renderer);

	this->
		setView(_view);
}

jPreview::~jPreview()
{
	if (d->view)
	{
		disconnect(d->view, SIGNAL(viewportChanged(QRectF)), d->renderer, SLOT(rebuild()));
	}
	setVisible(false);
	setView(0);
	removeEventFilter(d->renderer);
	delete d->renderer;
	delete d;
}

jPreview & jPreview::setView(jView * _view)
{
	THREAD_SAFE(Write)
	if (d->view)
	{
		disconnect(d->view, SIGNAL(viewportChanged(QRectF)), d->renderer, SLOT(rebuild()));
	}
	d->view = _view;
	if (d->view)
	{
		connect(d->view, SIGNAL(viewportChanged(QRectF)), d->renderer, SLOT(rebuild()));
	}
	THREAD_UNSAFE
	return * this;
}

const jView * jPreview::view() const
{
	return d->view;
}

jSelector & jPreview::selector() const
{
	return d->selector;
}

jLazyRenderer & jPreview::lazyRenderer() const
{
	return * d->renderer;
}

void jPreview::render(QPainter & _painter) const
{
	if (d->view == 0)
	{
		return;
	}
	QRectF _rect = rect();
	if (d->background.style() != Qt::NoBrush)
	{
		_painter.fillRect(_rect, d->background);
	}
	QRectF _zoom_rect = d->view->zoomer().rectBase();
	QVector<jItem *> _items = d->view->items();
	::qSort(_items.begin(), _items.end(), &Data::itemZSort);
	const jAxis * _x_axis = d->view->xAxis();
	const jAxis * _y_axis = d->view->yAxis();
	foreach (jItem * _item, _items)
	{
		_item->renderPreview(_painter, _rect, _zoom_rect, _x_axis, _y_axis);
	}
	d->updateSelector();
	d->selector.render(_painter, _rect, _zoom_rect);
}

QSize jPreview::minimumSizeHint() const
{
	return QSize(8, 8);
}

jPreview & jPreview::setBackground(const QBrush & _brush)
{
	SAFE_SET(d->background, _brush);
	return * this;
}

QBrush jPreview::background() const
{
	return SAFE_GET(d->background);
}

jPreview & jPreview::setOrientation(int _orientation)
{
	SAFE_SET(d->orientation, _orientation);
	return * this;
}

int jPreview::orientation() const
{
	return d->orientation;
}

void jPreview::mousePressEvent(QMouseEvent * _me)
{
	d->prev_point = _me->pos();
	d->press_point = _me->pos();
}

void jPreview::mouseReleaseEvent(QMouseEvent * _me)
{
	if (d->press_point == _me->pos())
	{
		d->setToPosition(rect(), _me);
	}
}

void jPreview::mouseMoveEvent(QMouseEvent * _me)
{
	d->pan(rect(), _me);
	d->prev_point = _me->pos();
}

void jPreview::mouseDoubleClickEvent(QMouseEvent * _me)
{
	d->zoomFullView(rect(), _me);
}

void jPreview::wheelEvent(QWheelEvent * _we)
{
	d->wheelScale(rect(), _we);
}

void jPreview::rebuild()
{
	d->renderer->rebuild();
}

// ------------------------------------------------------------------------

class jTimeSync
{
	COPY_FBD(jTimeSync)
public:
	jTimeSync() {}
	~jTimeSync() {}
	QTime registerInstance()
	{
		QMutexLocker _locker(&mutex);
		QTime _time = QTime::currentTime();
		entries[_time] = true;
		return _time;
	}
	bool checkpoint(const QTime & _check_time)
	{
		QMutexLocker _locker(&mutex);
		if (entries[_check_time] == false)
		{
			return false;
		}
		foreach (const QTime & _time, entries.keys())
		{
			if (_time < _check_time)
			{
				entries[_time] = false;
			}
		}
		return true;
	}
	void unregisterInstance(const QTime & _time)
	{
		QMutexLocker _locker(&mutex);
		entries.remove(_time);
	}
private:
	QMap<QTime, bool> entries;
	QMutex mutex;
};

struct jLazyRenderer::Data
{
	QWidget * widget;
        quint64 counter, threads_started;
	bool enabled;
	QThreadPool * thread_pool;
	QImage cached_viewport;
	bool force_update;
	int priority;
	jTimeSync time_sync;
	jLazyRenderer::render_func render_func;
	QReadWriteLock * rw_lock;
	Data()
	{
		thread_pool = 0;
		recreateThreadPool(1);
		force_update = false;
		enabled = true;
		counter = 0;
		threads_started = 0;
		widget = 0;
		priority = QThread::NormalPriority;
	}
	~Data()
	{
		delete thread_pool;
	}
	void recreateThreadPool(int _max_threads, int _exp_time = 60000)
	{
		delete thread_pool;
		thread_pool = new QThreadPool();
		thread_pool->setMaxThreadCount(_max_threads);
		thread_pool->setExpiryTimeout(_exp_time);
	}
	void rebuild(QRunnable * _instance)
	{
		if (!force_update)
		{
			thread_pool->tryStart(_instance);
		}
	}
	void start(QRunnable * _instance)
	{
		QPainter _painter(widget);
		_painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing, false);
		if (enabled)
		{
			rw_lock->lockForWrite();
			rebuild(_instance);
			force_update = false;
			rw_lock->unlock();
			_painter.drawImage(0, 0, cached_viewport, 0, 0, -1, -1, Qt::ThresholdDither);
		}
		else
		{
			render_func(widget, _painter);
		}
	}
	void flush()
	{
		rw_lock->lockForWrite();
		bool _state = enabled;
		enabled = false;
		rw_lock->unlock();
		do 
		{
			QApplication::processEvents();
		} 
		while (threads_started > counter);
		rw_lock->lockForWrite();
		enabled = _state;
		rw_lock->unlock();
	}
	void accepted(const QImage & _image)
	{
		rw_lock->lockForWrite();
		if (enabled)
		{
			cached_viewport = _image;
		}
		force_update = true;
		rw_lock->unlock();
		widget->update();
	}
	void render(QPaintDevice & _device)
	{
		QPainter _painter(&_device);
		_painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing, false);
		render_func(widget, _painter);
	}
};

jLazyRenderer::jLazyRenderer(QWidget * _widget, jLazyRenderer::render_func _render_func)
	: QObject(), QRunnable(), d(new Data())
{
	d->widget = _widget;
	d->render_func = _render_func;
	d->rw_lock = & rw_lock;
	setAutoDelete(false);
	connect(this, SIGNAL(accepted(QImage)), this, SLOT(onAccepted(QImage)));
}

jLazyRenderer::~jLazyRenderer()
{
	delete d;
}

void jLazyRenderer::run()
{
	d->threads_started++;
	QThread::currentThread()->setPriority((QThread::Priority)d->priority);
	if (d->enabled)
	{
		QTime _time = d->time_sync.registerInstance();
		QImage _image(d->widget->rect().size(), QImage::Format_RGB32);
		d->render(_image);
		if (d->time_sync.checkpoint(_time))
		{
			emit accepted(_image);
		}
		d->time_sync.unregisterInstance(_time);
	}
	d->counter++;
	QThread::currentThread()->setPriority(QThread::LowestPriority);
}

quint64 jLazyRenderer::counter() const
{
	return d->counter;
}

jLazyRenderer & jLazyRenderer::setEnabled(bool _state)
{
	if (_state == false)
	{
		SAFE_SET(d->enabled, false);
		d->flush();
	}
	else
	{
		THREAD_SAFE(Write)
		d->enabled = true;
		d->force_update = false;
		d->rebuild(this);
		THREAD_UNSAFE
	}
	return * this;
}

bool jLazyRenderer::isEnabled() const
{
	return d->enabled;
}

jLazyRenderer & jLazyRenderer::setMaxThreads(unsigned int _count)
{
	THREAD_SAFE(Write)
	bool _enabled = d->enabled;
	setEnabled(false);
	d->recreateThreadPool(_count);
	setEnabled(_enabled);
	THREAD_UNSAFE
	return * this;
}

unsigned int jLazyRenderer::maxThreads() const
{
	return SAFE_GET(d->thread_pool->maxThreadCount());
}

void jLazyRenderer::flush()
{
	d->flush();
}

void jLazyRenderer::onAccepted(const QImage & _image)
{
	d->accepted(_image);
}

jLazyRenderer & jLazyRenderer::setPriority(int _priority)
{
	SAFE_SET(d->priority, _priority);
	return * this;
}

int jLazyRenderer::priority() const
{
	return d->priority;
}

bool jLazyRenderer::eventFilter(QObject * _object, QEvent * _event)
{
	if (qobject_cast<QWidget *>(_object) == d->widget)
	{
		switch ((int)_event->type())
		{
		case QEvent::Paint:
			{
				d->start(this);
				return true;
			}
		case QEvent::Hide:
			{
				flush();
				return true;
			}
		}
	}
	return QObject::eventFilter(_object, _event);
}

void jLazyRenderer::rebuild()
{
	THREAD_SAFE(Write)
	if (d->enabled)
	{
		d->rebuild(this);
		THREAD_UNSAFE
	}
	else
	{
		THREAD_UNSAFE
		d->widget->update();
	}
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
	void connectAll(jSync * _sync)
	{
		foreach (jView * _view, views)
		{
			QObject::connect(&_view->zoomer(), SIGNAL(panned(QRectF)), _sync, SLOT(onPanned(QRectF)));
			QObject::connect(&_view->zoomer(), SIGNAL(zoomedIn(QRectF)), _sync, SLOT(onZoomedIn(QRectF)));
			QObject::connect(&_view->zoomer(), SIGNAL(zoomedOut(QRectF)), _sync, SLOT(onZoomedOut(QRectF)));
			QObject::connect(&_view->zoomer(), SIGNAL(zoomedFullView(QRectF)), _sync, SLOT(onZoomedFullView(QRectF)));
		}
	}
	void disconnectAll(jSync * _sync)
	{
		foreach (jView * _view, views)
		{
			QObject::disconnect(&_view->zoomer(), SIGNAL(panned(QRectF)), _sync, SLOT(onPanned(QRectF)));
			QObject::disconnect(&_view->zoomer(), SIGNAL(zoomedIn(QRectF)), _sync, SLOT(onZoomedIn(QRectF)));
			QObject::disconnect(&_view->zoomer(), SIGNAL(zoomedOut(QRectF)), _sync, SLOT(onZoomedOut(QRectF)));
			QObject::disconnect(&_view->zoomer(), SIGNAL(zoomedFullView(QRectF)), _sync, SLOT(onZoomedFullView(QRectF)));
		}
	}
	void reconnectAll(jSync * _sync)
	{
		disconnectAll(_sync);
		connectAll(_sync);
	}
	QRectF mapRect(jView * _src, const QRectF & _src_rect, jView * _dst)
	{
		QRectF _zoom_rect(_dst->zoomer().rect());
		if (_src->xAxis()->id())
		{
			if (_dst->xAxis()->id() == _src->xAxis()->id())
			{
				_zoom_rect.setLeft(_src_rect.left());
				_zoom_rect.setRight(_src_rect.right());
			}
			if (_dst->yAxis()->id() == _src->xAxis()->id())
			{
				_zoom_rect.setTop(_src_rect.left());
				_zoom_rect.setBottom(_src_rect.right());
			}
		}
		if (_src->yAxis()->id())
		{
			if (_dst->yAxis()->id() == _src->yAxis()->id())
			{
				_zoom_rect.setTop(_src_rect.top());
				_zoom_rect.setBottom(_src_rect.bottom());
			}
			if (_dst->xAxis()->id() == _src->yAxis()->id())
			{
				_zoom_rect.setLeft(_src_rect.top());
				_zoom_rect.setRight(_src_rect.bottom());
			}
		}
		return _zoom_rect;
	}
	void update(jView * _view)
	{
		_view->lazyRenderer().rebuild();
		foreach (jPreview * _preview, previews)
		{
			if (_preview && (_preview->view() == _view))
			{
				_preview->lazyRenderer().rebuild();
			}
		}
	}
	void blockSignals(QObject * _object)
	{
		blocked_signals[_object].push(_object->signalsBlocked());
		_object->blockSignals(true);
	}
	void unblockSignals(QObject * _object)
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
		d->blockSignals(&_view->zoomer());
		_view->zoomer().pan(_zoom_rect.x() - _view->zoomer().rect().x(), _zoom_rect.y() - _view->zoomer().rect().y());
		d->unblockSignals(&_view->zoomer());
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
	foreach (jView * _view, d->views)
	{
		if ((_view == 0) || (_sender == _view))
		{
			continue;
		}
		QRectF _zoom_rect = d->mapRect(_sender, _rect, _view);
		d->blockSignals(&_view->zoomer());
		_view->zoomer().zoomIn(_zoom_rect);
		d->unblockSignals(&_view->zoomer());
		d->update(_view);
	}
}

void jSync::onZoomedOut(const QRectF &)
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
		d->blockSignals(&_view->zoomer());
		_view->zoomer().zoomOut();
		d->unblockSignals(&_view->zoomer());
		d->update(_view);
	}
}

void jSync::onZoomedFullView(const QRectF &)
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
		d->blockSignals(&_view->zoomer());
		_view->zoomer().zoomFullView();
		d->unblockSignals(&_view->zoomer());
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
	static bool itemZSort(const jItem * _item1, const jItem * _item2)
	{
		if (_item1 && _item2)
		{
			return (_item1->z() > _item2->z());
		}
		return false;
	}
	void createWidgets()
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
			_updown_scrollbar->setMaximumHeight(16);
			_updown_scrollbar->setMaximumWidth(12);
			_updown_scrollbar->setProperty("index", _idx);
			layout->addWidget(_visibility_checkbox, _idx, 0);
			layout->addWidget(_color_label, _idx, 1);
			layout->addWidget(_text_label, _idx, 2);
			layout->addWidget(_updown_scrollbar, _idx, 3);
		}
	}
	void updateWidgets()
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
		qreal _z = _item1->z();
		_item1->setZ(_item2->z());
		_item2->setZ(_z);

		::qSwap(d->visibility_checkboxes[_item1], d->visibility_checkboxes[_item2]);
		::qSwap(d->color_labels[_item1], d->color_labels[_item2]);
		::qSwap(d->text_labels[_item1], d->text_labels[_item2]);
		::qSwap(d->updown_scrollbars[_item1], d->updown_scrollbars[_item2]);

		d->updateWidgets();
	}
}
