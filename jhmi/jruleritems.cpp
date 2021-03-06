#include "jruleritems.h"

// ------------------------------------------------------------------------

struct jHRulerItem::Data
{
	qreal x1_val, x2_val;
	qreal y_val;
	jHRulerItem * item;
	Data(jHRulerItem * _item)
	{
		x1_val = 0;
		x2_val = 0;
		y_val = 0;
		item = _item;
	}
	~Data()
	{
	}
	void update()
	{
		jHRulerItem::Point _points[2];
		_points[0].x = x1_val; _points[0].y = y_val;
		_points[1].x = x2_val; _points[1].y = y_val;
		item->jFigureItem<qreal>::setData(_points, 2, true);
	}
};

jHRulerItem::jHRulerItem() : jFigureItem<qreal>()
{
	d = new Data(this);
	setLineStyle(jHRulerItem::Lines);
	inputPattern().setEnabled(true);
}

jHRulerItem::~jHRulerItem()
{
}

jHRulerItem & jHRulerItem::setRange(const qreal & _x1, const qreal & _x2)
{
	d->x1_val = _x1;
	d->x2_val = _x2;
	d->update();
	return * this;
}

qreal jHRulerItem::x1() const
{
	return d->x1_val;
}

qreal jHRulerItem::x2() const
{
	return d->x2_val;
}

qreal jHRulerItem::dx() const
{
	return (d->x2_val - d->x1_val);
}


jHRulerItem & jHRulerItem::setY(const qreal & _y)
{
	d->y_val = _y;
	d->update();
	return * this;
}

qreal jHRulerItem::y() const
{
	return d->y_val;
}

void jHRulerItem::moveHorizontal(const qreal & dx)
{
	d->x1_val += dx;
	d->x2_val += dx;
	d->update();
}

void jHRulerItem::moveVertical(const qreal & dy)
{
	d->y_val += dy;
	d->update();
}

void jHRulerItem::moveTo(jHRulerItem::Point _pt)
{
	qreal _dx = d->x2_val - d->x1_val;
	d->x1_val = _pt.x;
	d->x2_val = d->x1_val + _dx;
	d->y_val = _pt.y;
	d->update();
}

QRectF jHRulerItem::boundingRect(const jAxis * _x_axis, const jAxis * _y_axis) const
{
	const unsigned int _width = size().width();
	if (_width == 0)
	{
		return QRectF();
	}
	const qreal _offset_x = origin().x();
	const qreal _offset_y = origin().y();
	qreal _left = d->x1_val - 1, _right = d->x2_val + 1;
	qreal _top = d->y_val + 1, _bottom = d->y_val - 1;

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
	return QRectF(QPointF(_left, _top), QPointF(_right, _bottom));
}

bool jHRulerItem::intersects(const QRectF & _rect, const jAxis * _x_axis, const jAxis * _y_axis) const
{
	QRectF _br = boundingRect(_x_axis, _y_axis);
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
	return _adj_rect.intersects(_br);
}

bool jHRulerItem::userCommand(int _action, int _method, int _buttons, int _modifier, QPointF _mpos, QWidget * _w) // jInputPattern::Action, jInputPattern::Method, buttons or key, modifiers or delta, mouse position
{
	jView * _view = dynamic_cast<jView *>(_w);
	if (_view == 0)
	{
		return false;
	}
	switch (_action)
	{
	case jInputPattern::MoveItemLeft:
		moveHorizontal(-1.0);
		_view->rebuild();
		return true;
	case jInputPattern::MoveItemRight:
		moveHorizontal(1.0);
		_view->rebuild();
		return true;
	case jInputPattern::MoveItemUp:
		moveVertical(1.0);
		_view->rebuild();
		return true;
	case jInputPattern::MoveItemDown:
		moveVertical(-1.0);
		_view->rebuild();
		return true;
	case jInputPattern::ItemMenuRequested:
		itemControl()->emitContextMenuRequested(_w->mapToGlobal(_mpos.toPoint()));
		return true;
	case jInputPattern::ItemPanStart:
	case jInputPattern::ItemPanMove:
	case jInputPattern::ItemPanEnd:
		return true;
	}
	return false;
}

// ------------------------------------------------------------------------

