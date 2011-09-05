#include "jruleritems.h"

// ------------------------------------------------------------------------

jHRulerItem::jHRulerItem() : jFigureItem<qreal>()
{
	x1_val = 0;
	x2_val = 0;
	y_val = 0;
	setLineStyle(jHRulerItem::Lines);
}

jHRulerItem::~jHRulerItem()
{
}

void jHRulerItem::update()
{
	jHRulerItem::Point _points[2];
	_points[0].x = x1_val; _points[0].y = y_val;
	_points[1].x = x2_val; _points[1].y = y_val;
	jFigureItem<qreal>::setData(_points, 2, true);
}

jHRulerItem & jHRulerItem::setRange(const qreal & _x1, const qreal & _x2)
{
	THREAD_SAFE(Write)
	x1_val = _x1;
	x2_val = _x2;
	THREAD_UNSAFE
	update();
	return * this;
}

qreal jHRulerItem::x1() const
{
	return x1_val;
}

qreal jHRulerItem::x2() const
{
	return x2_val;
}

qreal jHRulerItem::dx() const
{
	return SAFE_GET(x2_val - x1_val);
}


jHRulerItem & jHRulerItem::setY(const qreal & _y)
{
	SAFE_SET(y_val, _y);
	update();
	return * this;
}

qreal jHRulerItem::y() const
{
	return y_val;
}

void jHRulerItem::moveHorizontal(const qreal & dx)
{
	THREAD_SAFE(Write)
	x1_val += dx;
	x2_val += dx;
	THREAD_UNSAFE
	update();
}

void jHRulerItem::moveVertical(const qreal & dy)
{
	THREAD_SAFE(Write)
	y_val += dy;
	THREAD_UNSAFE
	update();
}

void jHRulerItem::moveTo(jHRulerItem::Point _pt)
{
	THREAD_SAFE(Write)
	qreal _dx = x2_val - x1_val;
	x1_val = _pt.x;
	x2_val = x1_val + _dx;
	y_val = _pt.y;
	THREAD_UNSAFE
	update();
}

QRectF jHRulerItem::boundingRect(const jAxis * _x_axis, const jAxis * _y_axis) const
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
	qreal _left = x1_val - 1, _right = x2_val + 1;
	qreal _top = y_val + 1, _bottom = y_val - 1;

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
	return QRectF(QPointF(_left, _top), QPointF(_right, _bottom));
}

bool jHRulerItem::intersects(const QRectF & _rect, const jAxis * _x_axis, const jAxis * _y_axis) const
{
	QRectF _br = boundingRect(_x_axis, _y_axis);
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
	THREAD_UNSAFE
	return _adj_rect.intersects(_br);
}

void jHRulerItem::userCommand(int _action, int _method, int _buttons, int _modifier, QPointF _mpos) // jInputPattern::Action, jInputPattern::Method, buttons or key, modifiers or delta, mouse position
{

}

// ------------------------------------------------------------------------

