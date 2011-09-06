#ifndef __JRULERITEMS_H__
#define __JRULERITEMS_H__

#include "jview.h"
#include "jitems.h"

// ------------------------------------------------------------------------

class jHRulerItem : public jFigureItem<qreal>
{
	PDATA
	COPY_FBD(jHRulerItem)
public:
	jHRulerItem();
	~jHRulerItem();

	jHRulerItem & setRange(const qreal & _x1, const qreal & _x2);
	qreal x1() const;
	qreal x2() const;
	qreal dx() const;

	jHRulerItem & setY(const qreal & _y);
	qreal y() const;

	void moveHorizontal(const qreal & dx);
	void moveVertical(const qreal & dy);
	void moveTo(jHRulerItem::Point _pt);

	bool userCommand(int, int, int, int, QPointF, QWidget *); // jInputPattern::Action, jInputPattern::Method, buttons or key, modifiers or delta, mouse position

	QRectF boundingRect(const jAxis * _x_axis = 0, const jAxis * _y_axis = 0) const;
	bool intersects(const QRectF & _rect, const jAxis * _x_axis, const jAxis * _y_axis) const;
protected:
	jItem1D<qreal> & setData(jItem1D<qreal>::Flat *, unsigned int, bool)			{ return * this; }
	jItem1D<qreal> & setData(jItem1D<qreal>::Flat *, qreal *, unsigned int, bool)		{ return * this; }
	jItem1D<qreal> & setData(jItem1D<qreal>::Point * , unsigned int, bool)			{ return * this; }
	jItem1D<qreal> & setData(jItem1D<qreal>::Radial *, unsigned int, bool)			{ return * this; }
};

// ------------------------------------------------------------------------

#endif
