#ifndef JVIEW_H
#define JVIEW_H

#include "jdefs.h"

bool jQuadToQuad(const QRectF & _from, const QRectF & _to, QTransform & _transform);

//! Class jAxis represents the axis object. 
/*!
 Axis has range, pen and font customizing. Axis could be dynamically attached to or detached from jView objects.
 One axis could be shared among several views.
 \sa jView
*/
class jAxis
{
	PDATA
	DECL_PROPERTIES(jAxis)
public:
	//!
	/*!
	Constructor.
	*/
	jAxis();
	//!
	/*!
	Virtual destructor.
	*/
	virtual ~jAxis();

	jAxis(const jAxis & _other);
	jAxis & operator = (const jAxis & _other);

	//! Prototype for axis value-to-label converter function (range function).
	/*!
	See default_range_convert() for parameters explanation.
	\sa default_range_convert(), setRange(), lo(), hi(), rangeFunc(), setRangeFunc()
	*/
	typedef QString (*range_func)(double, jAxis *);
	//! Default value-to-label function (range function).
	/*!
	\param _value value of axis tick
	\param _axis pointer to axis
	\return formatted text
	\sa range_func, setRange(), lo(), hi(), rangeFunc(), setRangeFunc()
	*/
	static QString default_range_convert(double _value, jAxis *);
	//! Sets range of values for the axis and range function.
	/*!
	\param _lo low boundary of interval
	\param _hi high boundary of interval
	\param _range_func pointer to function
	\return reference
	\sa range_func, default_range_convert(), lo(), hi(), rangeFunc(), setRangeFunc()
	*/
	jAxis & setRange(double _lo, double _hi, range_func _range_func = &default_range_convert);
	//! Sets range function for the axis.
	/*!
	\param _range_func pointer to function
	\return reference
	\sa range_func, default_range_convert(), setRange(), lo(), hi(), rangeFunc()
	*/
	jAxis & setRangeFunc(range_func _range_func = &default_range_convert);
	//! Returns low boundary of interval.
	/*!
	\return low boundary of interval
	\sa range_func, default_range_convert(), setRange(), hi(), rangeFunc(), setRangeFunc()
	*/
	double lo() const;
	//! Returns high boundary of interval.
	/*!
	\return high boundary of interval
	\sa range_func, default_range_convert(), setRange(), lo(), rangeFunc(), setRangeFunc()
	*/
	double hi() const;
	//! Returns pointer to current value-to-label converter function (range function).
	/*!
	\return pointer to function
	\sa range_func, default_range_convert(), setRange(), lo(), hi(), setRangeFunc()
	*/
	range_func rangeFunc() const;

	//! Sets expected number of ticks and ticks step.
	/*!
	For example, if range is [0..5], _count_hint = 3 and _aligment = 2.5 then ticks will be drawn for values 0.0, 2.5 and 5.0.
	Actual number of ticks drawn could be less than expected number. It could be obtained with count() method.
	\param _count_hint expected number of ticks
	\param _alignment minimum distance between two neighbouring ticks
	\return reference
	\sa countHint(), count(), alignment()
	*/
	jAxis & setStep(unsigned int _count_hint = 10, double _alignment = 1.0);
	//! Returns expected number of ticks.
	/*!
	\return number of ticks
	\sa setStep(), count(), alignment()
	*/
	unsigned int countHint() const;
	//! Returns actual number of ticks.
	/*!
	\return number of ticks
	\sa setStep(), countHint(), alignment()
	*/
	unsigned int count() const;
	//! Returns ticks alignment.
	/*!
	In this case, alignment is a minimum distance between neighbouring ticks.
	\return alignment
	\sa setStep(), countHint(), count()
	*/
	double alignment() const;

	jAxis & setAlignmentOffset(double _offset);
	double alignmentOffset() const;

	//! Sets tick length.
	/*!
	Tick length is measured in pixels.
	\param _length length
	\return reference
	*/
	jAxis & setTickLength(unsigned int _length);
	//! Returns length of tick.
	/*!
	\return length
	*/
	unsigned int tickLength() const;

	//! Renders axis on a QPainter.
	/*!
	Axis does not store its orientation. Orientation has meaning only when rendering.
	\param _painter reference to QPainter
	\param _dst_rect rectangle area on a QPaintDevice
	\param _orientation orientation of axis (could be Qt::Vertical or Qt::Horizontal)
	\param _lo low boundary of area to be rendered (_lo >= lo())
	\param _hi high boundary of area to be rendered (_hi <= hi())
        \param _draw_grid grid visibility flag
	\sa lo(), hi()
	*/
    virtual void render(QPainter & _painter, const QRectF & _dst_rect, int _orientation, double _lo, double _hi, bool _draw_grid);

	//! Sets axis pen.
	/*!
	\param _pen QPen object
	\return reference
	\sa pen()
	*/
	jAxis & setPen(const QPen & _pen);
	//! Acquires axis pen.
	/*!
	\return QPen object
	\sa setPen()
	*/
	QPen pen() const;
	//! Sets axis font.
	/*!
	\param _font QFont object
	\return reference
	\sa font()
	*/
	jAxis & setFont(const QFont & _font);
	//! Acquires axis font.
	/*!
	\return QFont object
	\sa setFont()
	*/
	QFont font() const;


	//! Sets axis grid pen.
	/*!
	\param _pen QPen object
	\return reference
	\sa gridPen()
	*/
	jAxis & setGridPen(const QPen & _grid_pen);
	//! Acquires axis grid pen.
	/*!
	\return QPen object
	\sa setGridPen()
	*/
	QPen gridPen() const;

	//! Sets axis background brush for text labels.
	/*!
	\param _brush QBrush object
	\return reference
	\sa background()
	*/
	jAxis & setBackground(const QBrush & _brush);
	//! Acquires axis background brush for text labels.
	/*!
	\return QBrush object
	\sa setBackground()
	*/
	QBrush background() const;

	//! Sets visibility state.
	/*!
	\param _state visibility flag
	\return reference
	\sa isVisible()
	*/
	jAxis & setVisible(bool _state);
	//! Checks item visibility.
	/*!
	\return visibility state
	\sa setVisible()
	*/
	bool isVisible() const;

	//! Sets axis id.
	/*!
	Axes with the same id != 0 that attached to different views have the opportunity of synchronizing.
	id = 0 by default. For further materials about synchronizing read the jSync article.
	\param _id id of axis
	\return reference
	\sa id(), jSync
	*/
	jAxis & setId(int _id);
	//! Returns axis id.
	/*!
	\return id of axis
	\sa setId()
	*/
	int id() const;

	//! Sets log10 flag.
	/*!
	jItem object checks log10 flag to correctly render itself.
	\param _state state of log10 flag
	\return reference
	\sa setLog10Multiplier(), isLog10ScaleEnabled()
	*/
	jAxis & setLog10ScaleEnabled(bool _state);
	//! Returns log10 flag.
	/*!
	\return log10 flag state
	\sa setLog10ScaleEnabled()
	*/
	bool isLog10ScaleEnabled() const;

	//! Sets multiplier for log10 scale.
	/*!
	For example, multiplier "10" is for decibel scale (10 * log10(x)).
	\param _mpy multiplier value
	\return reference
	\sa log10Multiplier(), setLog10ScaleEnabled()
	*/
	jAxis & setLog10Multiplier(double _mpy);
	//! Returns multiplier for log10 scale.
	/*!
	\return multiplier value
	\sa setLog10Multiplier(), setLog10ScaleEnabled()
	*/
	double log10Multiplier() const;
	//! Helper method to convert from linear to log10 scale.
	/*!
	\param _value value to be converted
	\param _minimum value to replace negative input values
	\return converted value
	\sa fromLog10(), setLog10Multiplier(), setLog10ScaleEnabled()
	*/
	double toLog10(double _value, double _minimum = 0.0) const;
	//! Helper method to convert from log10 to linear scale.
	/*!
	\param _value value to be converted
	\return converted value
	\sa toLog10(), setLog10Multiplier(), setLog10ScaleEnabled()
	*/
	double fromLog10(double _value) const;

	//! Maps value from this axis to another
	/*!
	\param _value value from this axis range
	\param _dst destination axis
	\return value mapped to destination axis range
	\sa mapFromAxis()
	*/
	double mapToAxis(double _value, const jAxis & _dst) const;
	//! Maps value to this axis from another
	/*!
	\param _value value from source axis range
	\param _src source axis
	\return value mapped from source axis range
	\sa mapToAxis()
	*/
	double mapFromAxis(double _value, const jAxis & _src) const;
	//! Converts value from linear scale to log scale if it enabled, or does nothing if scale is linear already
	/*!
	\param _value value to be converted
	\param _minimum value to replace negative input values
	\return value normalized by toLog10
	\sa toLog10(), isLog10ScaleEnabled()
	*/
	double normalizeToScale(double _value, double _minimum = 0.0) const;
	//! Converts value from log scale to linear scale if it enabled, or does nothing if scale is linear already
	/*!
	\param _value value to be converted
	\return value normalized by fromLog10
	\sa fromLog10(), isLog10ScaleEnabled()
	*/
	double normalizeFromScale(double _value) const;

	static double normalizeToScale(const jAxis * _axis, double _value, double _minimum = 0.0);
	static double normalizeFromScale(const jAxis * _axis, double _value);
};

class jItem;

class jSelector
{
	PDATA
	COPY_FBD(jSelector)
	DECL_PROPERTIES(jSelector)
public:
	jSelector();
	~jSelector();

	jSelector & setPen(const QPen & _pen);
	QPen pen() const;

	jSelector & setRect(const QRectF & _rect);
	QRectF rect() const;

	jSelector & setVisible(bool _state);
	bool isVisible() const;

	jSelector & setBackground(const QBrush & _brush);
	QBrush background() const;

	void render(QPainter & _painter, const QRectF & _dst_rect, const QRectF & _src_rect);
	void renderPreview(QPainter & _painter, const QRectF & _dst_rect, const QRectF & _src_rect);

	jSelector & setPreviewEnabled(bool _state);
	bool previewEnabled() const;

	jItem & internalItem() const;
};

class jViewport: public QObject
{
	Q_OBJECT
	PDATA
	friend class jSync;
public:
	jViewport();
	virtual ~jViewport();

	jViewport & setBase(const QRectF & _rect);
	jViewport & setBase(const jAxis & _x_axis, const jAxis & _y_axis);
	QRectF rectBase() const;
	jViewport & adjustBase(const QRectF & _rect);
	jViewport & adjustBase(const jAxis & _x_axis, const jAxis & _y_axis);

	jViewport & setZoomFullView(const QRectF & _rect);
	jViewport & setZoomFullView(const jAxis & _x_axis, const jAxis & _y_axis);
	jViewport & adjustZoomFullView(const QRectF & _rect);
	jViewport & adjustZoomFullView(const jAxis & _x_axis, const jAxis & _y_axis);

	QRectF rect() const;

	QVector<QRectF> history() const;
	void clearHistory();

	jSelector & selector() const;

	jViewport & setZoomOrientation(int _orientation);
	int zoomOrientation() const;

	QRectF adjustRect(const QRectF & _rect, bool _to_orientation_only = false) const;

	jViewport & setMinimumSize(const QSizeF & _size);
	jViewport & setMinimumWidth(double _width);
	jViewport & setMinimumHeight(double _height);
	QSizeF minimumSize() const;
	jViewport & setMaximumSize(const QSizeF & _size);
	jViewport & setMaximumWidth(double _width);
	jViewport & setMaximumHeight(double _height);
	QSizeF maximumSize() const;
public slots:
	void zoomIn(const QRectF & _rect);
	void zoomOut();
	void zoomFullView();
	void pan(double _dx, double _dy);
signals:
	void zoomedIn(QRectF);
	void zoomedOut(QRectF);
	void zoomedFullView(QRectF);
	void panned(QRectF);
};

class jLabel
{
	PDATA
	COPY_FBD(jLabel)
	DECL_PROPERTIES(jLabel)
public:
	jLabel(const QString & _text = QString());
	virtual ~jLabel();

	jLabel & setText(const QString & _text);
	QString text() const;

	jLabel & setPen(const QPen & _pen);
	QPen pen() const;
	jLabel & setFont(const QFont & _font);
	QFont font() const;

	jLabel & setPos(const QPointF & _pos);
	QPointF pos() const;

	jLabel & setSize(const QSizeF & _size);
	QSizeF size() const;
	QSizeF sizeHint() const;

	jLabel & setVisible(bool _state);
	bool isVisible() const;

	jLabel & setOptions(const QTextOption & _options);
	QTextOption options() const;

	jLabel & setBackground(const QBrush & _brush);
	QBrush background() const;

	jLabel & setAutoSize(bool _state);
	bool autoSize() const;

	virtual void render(QPainter & _painter, const QRectF & _dst_rect, const QRectF & _src_rect);
	virtual void renderPreview(QPainter & _painter, const QRectF & _dst_rect, const QRectF & _src_rect);

	jLabel & setPreviewEnabled(bool _state);
	bool previewEnabled() const;
};

//! Class jCoordinator represents tooltip near cursor placed on a view.
/*!
 Position of coordinator and offset from position could be tuned by setPos() and setOffset() methods.
 jCoordinator could be rendered on any QPaintDevice object via render() method.
 All methods of this class are reentrant and thread safe.
 \sa jLabel
*/
class jCoordinator
{
	PDATA
	COPY_FBD(jCoordinator)
	DECL_PROPERTIES(jCoordinator)
public:
	//!
	/*!
	Constructor.
	*/
	jCoordinator();
	//!
	/*!
	Destructor.
	Note that destructor is not virtual.
	*/
	~jCoordinator();

	//! Prototype for format function.
	/*!
	Format function is used for printing formatted position in user-defined style.
	See default_format() article for parameters explanation.
	\sa setFormat, formatFunc, default_format
	*/
	typedef QString (*format_func)(double, double, jAxis *, jAxis *, jCoordinator *);
	//! Base format function.
	/*!
	Base format function prints position of cursor as described in axes implementation.
	\param _x x coordinate
	\param _y y coordinate
	\param _x_axis pointer to jAxis object that represents horizontal axis
	\param _y_axis pointer to jAxis object that represents vertical axis
	\param _coordinator pointer to jCoordinator object that calls this format function
	\return formatted string
	\sa setFormat, formatFunc, format_func
	*/
	static QString default_format(double _x, double _y, jAxis * _x_axis, jAxis * _y_axis, jCoordinator * _coordinator);
	//! Sets format function.
	/*!
	Format function is used for printing formatted position in user-defined style. By default default_format() is installed as format function.
	\param _format_func pointer to format function
	\return reference
	\sa format_func, formatFunc, default_format
	*/
	jCoordinator & setFormat(format_func _format_func = &default_format);
	//! Returns pointer to format function.
	/*!
	\return pointer to format function
	\sa setFormat, format_func, default_format
	*/
	format_func formatFunc() const;

	//! Returns pointer to internal jLabel object which displays text of coordinator.
	/*!
	\return jLabel object
	\sa jLabel
	*/
	jLabel & label() const;

	//! Sets position of top left corner.
	/*!
	Top left corner is usually position of cursor.
	\param _pos position measured in axes coordinates
	\sa pos(), setOffset(), offset()
	*/
	jCoordinator & setPos(const QPointF & _pos);
	//! Returns position of top left corner.
	/*!
	\return position measured in axes coordinates
	\sa setPos(), setOffset(), offset()
	*/
	QPointF pos() const;

	//! Sets offset from top left corner position.
	/*!
	Offset helps to avoid overlaying of cursor by coordinator's label. Default offset is (8, 8).
	\param _offset offset measured in pixels
	\sa offset(), setPos(), pos()
	*/
	jCoordinator & setOffset(const QPointF & _offset);
	//! Returns offset from top left corner position.
	/*!
	\return offset measured in pixels
	\sa setOffset(), setPos(), pos()
	*/
	QPointF offset() const;

	//! Renders coordinator onto jView widget or any acceptable QPaintDevice object according to its position, offset, format function and label properties.
	/*!
	\param _painter reference to QPainter object
	\param _dst_rect physical rectangle on which QPainter will draw item (measured in pixels if user paints on a widget)
	\param _src_rect rectangle measured in axes units (fragment of item that will be rendered)
	\param _x_axis pointer to jAxis object that represents horizontal axis
	\param _y_axis pointer to jAxis object that represents vertical axis
	\sa jView, jAxis, jViewportm, jLabel
	*/
	void render(QPainter & _painter, const QRectF & _dst_rect, const QRectF & _src_rect, const jAxis * _x_axis = 0, const jAxis * _y_axis = 0);
};

class jItemHandler;
class jInputPattern;

//! Class jItem represents one graphical item of jView widget.
/*!
 Class jItem is an abstract class with pure virtual method render() which renders item on jView widget.
 User tunes pen with setPen(), brush with setBrush() and z-order with setZ() method.
 Input data incomes through setData(); optionally in deep copy mode.
 All methods of this class are reentrant and thread safe.
 \sa jView, jPreview
*/
class jItem
{
	PDATA
	COPY_FBD(jItem)
	DECL_PROPERTIES(jItem)
public:
    enum LineStyle {Lines = 1, Ticks = 2, Bars = 3, Dots = 4};
    enum DataType {FlatData = 1, PointData = 2, RadialData = 3};

	//! 
	/*!
	Constructor.
	*/
	jItem();
	//! 
	/*!
	Destructor.
	*/
	virtual ~jItem();

	//! Sets item main pen.
	/*!
	\param _pen QPen object
	\return reference
	\sa pen(), brush(), setBrush()
	*/
	jItem & setPen(const QPen & _pen);
	//! Acquires item main pen.
	/*!
	\return QPen object
	\sa setPen(), brush(), setBrush()
	*/
	QPen pen() const;

	//! Sets item main brush.
	/*!
	\param _brush QBrush object
	\return reference
	\sa setPen(), pen(), setBrush()
	*/
	jItem & setBrush(const QBrush & _brush);
	//! Acquires item main brush.
	/*!
	\return QBrush object
	\sa setPen(), pen(), brush()
	*/
	QBrush brush() const;

	//! Sets visibility state.
	/*!
	\param _state visibility flag
	\return reference
	\sa isVisible()
	*/
	jItem & setVisible(bool _state);
	//! Checks item visibility.
	/*!
	\return visibility state
	\sa setVisible()
	*/
	bool isVisible() const;

	//! Sets origin (of coordinates). 
	/*!
	Default origin is (0, 0).
	\param _state visibility flag
	\return reference
	\sa origin()
	*/
	jItem & setOrigin(const QPointF & _origin);
	//! Gets origin (of coordinates). 
	/*!
	\return origin of coordinates (offset)
	\sa setOrigin()
	*/
	QPointF origin() const;

	//! Sets z coordinate (z-order). 
	/*!
	Default z value is 0. The higher value brings item to front of view.
	\param _z value of z coordinate
	\return reference
	\sa z()
	*/
	jItem & setZ(double _z);
	//! Returns z coordinate (z-order). 
	/*!
	\return z coordinate
	\sa setZ()
	*/
	double z() const;

	//! Sets tool tip text. 
	/*!
	Tool tips become visible when jView::showToolTip() is called.
	\param _text tool tip text
	\return reference
	\sa toolTip(), jView::showToolTip()
	*/
	jItem & setToolTip(const QString & _text);
	//! Returns current tool tip text.
	/*!
	\return tool tip text
	\sa setToolTip()
	*/
	QString toolTip() const;

	//! Returns pointer to data.
	/*!
	\return untyped pointer
	\sa setData()
	*/
	virtual const void * data() const;
	//! Returns measurements (width and height) of data.
	/*!
	\return size
	\sa setData()
	*/
	virtual QSize size() const;
	//! Returns size of one element (sample or point) of data.
	/*!
	\return size of one data element measured in bytes
	\sa setData(), setBytesPerItem()
	*/
	unsigned int bytesPerItem() const;
	//! Returns flag that describes data copying method.
	/*!
	"true" means data was copied to internal buffer and "false" means not.
	\return flag of deep copying
	\sa setData()
	*/
	bool isDeepCopy() const;

	//! Renders item onto jView widget or any acceptable QPaintDevice object according to its measurements, pen, brush, visibility and z-order.
	/*!
	render() is a pure virtual method meant to be reimplemented in subclasses.
	\param _painter reference to QPainter object
	\param _dst_rect physical rectangle on which QPainter will draw item (measured in pixels if user paints on a widget)
	\param _src_rect rectangle measured in axes units (fragment of item that will be rendered)
	\param _x_axis x axis
	\param _y_axis y axis
	\sa jView, jAxis, jViewport, renderPreview()
	*/
	virtual void render(QPainter & _painter, const QRectF & _dst_rect, const QRectF & _src_rect, const jAxis * _x_axis = 0, const jAxis * _y_axis = 0) = 0;
	//! Calculates intersection of specified rectangle and area occupied by item.
	/*!
	\param _rect rectangle measured in axes units
	\param _x_axis x axis
	\param _y_axis y axis
	\return fact of intersection
	*/
	virtual bool intersects(const QRectF & _rect, const jAxis * _x_axis = 0, const jAxis * _y_axis = 0) const;

	//! This method is called by jView widget when current state of zoom is changed.
	/*!
	Default implementation of updateViewport() is empty. It may be useful to reimplement this method when input data has a huge size
	and should be partially loaded when user scrolls or zoomes viewport.
	\param _rect rectangle measured in axes units
	\sa jView, jViewport
	*/
	virtual void updateViewport(const QRectF & _rect);
	//! Renders item preview onto jPreview widget or any acceptable QPaintDevice.
	/*!
	Default implementation of renderPreview() is equal to render() but with disabled symbols rendering.
	It may be useful to reimplement this method when input data has a huge size and it's impossible to render whole data at once.
	\param _painter reference to QPainter object
	\param _dst_rect physical rectangle on which QPainter will draw item (measured in pixels if user paints on a widget)
	\param _src_rect rectangle measured in axes units (fragment of item that will be rendered)
	\param _x_axis x axis
	\param _y_axis y axis
	\sa jPreview, jView, jAxis, jViewport, render()
	*/
	virtual void renderPreview(QPainter & _painter, const QRectF & _dst_rect, const QRectF & _src_rect, const jAxis * _x_axis = 0, const jAxis * _y_axis = 0);

	//!Returns total number of rendered primitives (points).
	/*!
	\return number of rendered primitives
	*/
	virtual quint64 counter() const;

	//!Returns bounding rectangle.
	/*!
	In common case, bounding rectangle is not equal QRectF(origin(), size()), because size() is size of data.
	\param _x_axis x axis
	\param _y_axis y axis
	\return rectangle
	*/
	virtual QRectF boundingRect(const jAxis * _x_axis = 0, const jAxis * _y_axis = 0) const;

	jItem & setInputPattern(const jInputPattern & _pattern);
	jInputPattern & inputPattern() const;
	jItemHandler * itemControl() const;

    // it must return "true" if you don't need to break the event filtering chain
	virtual bool userCommand(int, int, int, int, QPointF, QWidget *); // jInputPattern::Action, jInputPattern::Method, buttons or key, modifiers or delta, mouse position

	jItem & setSymbol(const QImage & _img);
	QImage symbol() const;

	jItem & setPreviewEnabled(bool _state);
	bool previewEnabled() const;

	void clear();
protected:
	//! Directly sets pointer to input data with its measurements.
	/*!
	Remember that total number of elements (samples or points) of data must be not less than _width * _height.
	\param _data untyped pointer to input data
	\param _width width of data (measured in points or samples)
	\param _height height of data (measured in points or samples)
	\param _deep_copy flag that tells to make or not a deep copy of input data
	\return reference
	\sa data(), width(), height(), bytesPerItem(), isDeepCopy()
	*/
	virtual jItem & setData(void * _data, unsigned int _width, unsigned int _height = 1, bool _deep_copy = false);

	//! Sets size of one element (sample or point) of data.
	/*!
	This method is usually called from constructors of jItem sublasses.
	\param _bytes_per_item size of one data element measured in bytes
	\return reference
	\sa setData(), bytesPerItem()
	*/
	jItem & setBytesPerItem(unsigned int _bytes_per_item);
	//! Adds number of rendered primitives to internal counter.
	/*!
	\param _count number of primitives
	*/
	void addCounter(quint64 _count);
};

class jItemHandler : public QObject
{
	Q_OBJECT
	PDATA
public:
	jItemHandler(jItem * _item);
	~jItemHandler();

	jItem * item() const;

	jItemHandler & setPatternFilter(int _from, int _to); // default is from ItemActionGroupBegin to ItemActionGroupEnd
	int patternFilterFrom() const;
	int patternFilterTo() const;

	jItemHandler & setSensitiveArea(const QSize & _size);
	QSize sensitiveArea() const;

	void emitContextMenuRequested(QPoint);
public slots:
	void actionAccepted(int, int, int, int, QPointF, QWidget *); // jInputPattern::Action, jInputPattern::Method, buttons or key, modifiers or delta, mouse position
signals:
	void contextMenuRequested(QPoint);
};

class jMarker
{
	PDATA
	COPY_FBD(jMarker)
	DECL_PROPERTIES(jMarker)
public:
	jMarker();
	~jMarker();

	jMarker & setPen(const QPen & _pen);
	QPen pen() const;

	jMarker & setValue(double _value);
	inline double value() const;

	jMarker & setVisible(bool _state);
	bool isVisible() const;

	jMarker & setOrientation(int _orientation);
	int orientation() const;

	void render(QPainter & _painter, const QRectF & _dst_rect, const QRectF & _src_rect);
	void renderPreview(QPainter & _painter, const QRectF & _dst_rect, const QRectF & _src_rect);

	jMarker & setPreviewEnabled(bool _state);
	bool previewEnabled() const;

	jItem & internalItem() const;
};

class jInputPattern : public QObject
{
	Q_OBJECT
	PDATA
public:
	jInputPattern(QObject * _parent = 0);
	jInputPattern(const jInputPattern & _other, QObject * _parent = 0);
	jInputPattern & operator = (const jInputPattern & _other);
	~jInputPattern();

	jInputPattern & setDefaultPattern();

	enum Action 
	{
		UnknownAction = -1,

		WidgetActionGroupBegin = 0,
			MoveCursorLeft =		WidgetActionGroupBegin + 0,
			MoveCursorRight =		WidgetActionGroupBegin + 1,
			MoveCursorUp =			WidgetActionGroupBegin + 2,
			MoveCursorDown =		WidgetActionGroupBegin + 3,
			ZoomStart =				WidgetActionGroupBegin + 4,
			ZoomMove =				WidgetActionGroupBegin + 5,
			ZoomEnd =				WidgetActionGroupBegin + 6,
			ZoomFullView =			WidgetActionGroupBegin + 7,
			ZoomDeltaVertical =		WidgetActionGroupBegin + 8,
			ZoomDeltaHorizontal =	WidgetActionGroupBegin + 9,
            ZoomDelta =             ZoomDeltaVertical,
			PanStart =				WidgetActionGroupBegin + 10,
			PanMove =				WidgetActionGroupBegin + 11,
			PanEnd =				WidgetActionGroupBegin + 12,
			ContextMenuRequested =	WidgetActionGroupBegin + 13,
			PreviewPanStart =		WidgetActionGroupBegin + 14,
			PreviewPanMove =		WidgetActionGroupBegin + 15,
			PreviewPanEnd =			WidgetActionGroupBegin + 16,
			PreviewFullView =		WidgetActionGroupBegin + 17,
			PreviewDeltaVertical =	WidgetActionGroupBegin + 18,
			PreviewDeltaHorizontal =WidgetActionGroupBegin + 19,
			MoveViewportLeft =		WidgetActionGroupBegin + 20,
			MoveViewportRight =		WidgetActionGroupBegin + 21,
			MoveViewportUp =		WidgetActionGroupBegin + 22,
			MoveViewportDown =		WidgetActionGroupBegin + 23,
		WidgetActionGroupEnd =	WidgetActionGroupBegin + 999,

		ItemActionGroupBegin =	WidgetActionGroupBegin + 1000,
			MoveItemLeft =			ItemActionGroupBegin + 0,
			MoveItemRight =			ItemActionGroupBegin + 1,
			MoveItemUp =			ItemActionGroupBegin + 2,
			MoveItemDown =			ItemActionGroupBegin + 3,
			ItemPanStart =			ItemActionGroupBegin + 4,
			ItemPanMove =			ItemActionGroupBegin + 5,
			ItemPanEnd =			ItemActionGroupBegin + 6,
			ItemMenuRequested =		ItemActionGroupBegin + 7,
            ItemSelected =          ItemActionGroupBegin + 8,
        ItemActionGroupEnd =	ItemActionGroupBegin + 999,

        UserAction = 100000
	};
	enum Method
	{
        UnknownMethod =         -1,
        KeyPress =              QEvent::KeyPress,
        KeyRelease =            QEvent::KeyRelease,
        MousePress =            QEvent::MouseButtonPress,
        MouseRelease =          QEvent::MouseButtonRelease,
        MouseDoubleClick =      QEvent::MouseButtonDblClick,
        WheelVertical =         QEvent::Wheel,
        WheelHorizontal =       -QEvent::Wheel,
        Wheel =                 WheelVertical,
        MouseMove =             QEvent::MouseMove
	};

	jInputPattern & addAction(int _action, int _method, int _code = 0, int _modifier = 0);
	jInputPattern & removeAction(int _action);
	jInputPattern & removeAction(int _action, int _method);
	jInputPattern & removeAction(int _action, int _method, int _code);
	jInputPattern & removeAction(int _action, int _method, int _code, int _modifier);

	void clear();

	QVector<int> actions() const;
	QVector<int> actionMethods(int _action) const;
	QVector<int> actionCodes(int _action, int _method, QVector<int> & _modifiers) const;
	QVector<int> actionModifiers(int _action, int _method) const;

	QPointF lastMousePosition() const;
	int lastMouseButtons() const;
	int lastKeyboardKey() const;
	int lastModifiers() const;
	int lastDelta() const;

	jInputPattern & setEnabled(bool _state);
	bool isEnabled() const;

	static void setAccepted(QObject *, bool);
	static void accepted(QObject *);
	static void rejected(QObject *);
signals:
	void actionAccepted(int, int, int, int, QPointF, QWidget *);		// action, method, code, modifier, mouse position
protected:
	bool eventFilter(QObject * _object, QEvent * _event);
};

class jRenderer;

class jView : public JUNGLE_WIDGET_CLASS
{
	Q_OBJECT
	PDATA
public:
	jView(QWidget * _parent = 0);
	jView(jAxis * _x_axis, jAxis * _y_axis, QWidget * _parent = 0);
	jView(const jAxis & _x_axis, const jAxis & _y_axis, QWidget * _parent = 0);
	~jView();

	jView & setXAxis(jAxis * _axis);
	jAxis * xAxis() const;

	jView & setYAxis(jAxis * _axis);
	jAxis * yAxis() const;

	jView & setZoomFullViewMaximized(bool _state);
	bool isZoomFullViewMaximized() const;

	jView & setXAxisVisibleOverride(bool _state);
	jView & setYAxisVisibleOverride(bool _state);
	bool isXAxisVisibleOverride() const;
	bool isYAxisVisibleOverride() const;
	bool isXAxisVisible() const;
	bool isYAxisVisible() const;

    jView & setGridEnabled(bool _draw_grid);
    bool gridEnabled() const;

	enum {AxesInForeplane = 0, AxesInBackplane = 1};
	jView & setAxesPlane(int _plane);
	int axesPlane() const;

	inline jViewport & viewport() const;
	jCoordinator & coordinator() const;
	jMarker & horizontalMarker() const;
	jMarker & verticalMarker() const;
	jRenderer & renderer() const;

	jView & addItem(jItem * _item);
	jView & addItems(const QVector<jItem *> & _items);
	jView & setItem(jItem * _item);
	jView & setItems(const QVector<jItem *> & _items);
	jView & removeItem(jItem * _item);
	jView & removeItems(const QVector<jItem *> & _items);
	QVector<jItem *> items() const;

	void clear();

	jView & setBackground(const QBrush & _brush);
	QBrush background() const;

	jView & addLabel(jLabel * _label);
	jView & addLabels(const QVector<jLabel *> & _labels);
	jView & setLabel(jLabel * _label);
	jView & setLabels(const QVector<jLabel *> & _labels);
	jView & removeLabel(jLabel * _label);
	jView & removeLabels(const QVector<jLabel *> & _labels);
	QVector<jLabel *> labels() const;

	jView & addSelector(jSelector * _selector);
	jView & addSelectors(const QVector<jSelector *> & _selectors);
	jView & setSelector(jSelector * _selector);
	jView & setSelectors(const QVector<jSelector *> & _selectors);
	jView & removeSelector(jSelector * _selector);
	jView & removeSelectors(const QVector<jSelector *> & _selectors);
	QVector<jSelector *> selectors() const;

	jView & setMarkers(const QVector<jMarker *> & _markers);
	QVector<jMarker *> markers() const;

	QTransform screenToAxisTransform() const;
	QRectF screenToAxis(const QRectF & _src_rect) const;
	QPointF screenToAxis(const QPointF & _src_point) const;
	QTransform axisToScreenTransform() const;
	QRectF axisToScreen(const QRectF & _src_rect) const;
	QPointF axisToScreen(const QPointF & _src_point) const;

	virtual void render(QPainter & _painter) const;

	QVector<jItem *> itemsAt(const QPointF & _point, bool _exclude_invisible = true) const;
	QVector<jItem *> showToolTip(const QPointF & _point);

	QRectF itemsBoundingRect(bool _exclude_invisible = true) const;
	void autoScaleX(double _margin_x = 0.05); // 5%
	void autoScaleY(double _margin_y = 0.05); // 5%
	void autoScale(double _margin_x = 0.05, double _margin_y = 0.05);  // 5%

	jView & setInputPattern(const jInputPattern & _pattern);
	jInputPattern & inputPattern() const;

	virtual bool userCommand(int, int, int, int, QPointF, QWidget *); // jInputPattern::Action, jInputPattern::Method, buttons or key, modifiers or delta, mouse position

	QPointF cursorPos() const;
	void restoreCursorBeforePan();

	static bool itemZSort(const jItem * _item1, const jItem * _item2);  // _item1->z() < _item2->z()
	static bool markerZSort(const jMarker * _marker1, const jMarker * _marker2);
	static bool selectorZSort(const jSelector * _selector1, const jSelector * _selector2);
	static bool itemZSortReversed(const jItem * _item1, const jItem * _item2);  // _item1->z() < _item2->z()
	static bool markerZSortReversed(const jMarker * _marker1, const jMarker * _marker2);
	static bool selectorZSortReversed(const jSelector * _selector1, const jSelector * _selector2);
public slots:
	void rebuild();
	void actionAccepted(int, int, int, int, QPointF, QWidget *); // jInputPattern::Action, jInputPattern::Method, buttons or key, modifiers or delta, mouse position
protected:
	void mouseMoveEvent(QMouseEvent *);
	void enterEvent(QEvent *);
	void leaveEvent(QEvent *);
    void resizeEvent(QResizeEvent *);
signals:
	void contextMenuRequested(QPoint);
	void viewportChanged(QRectF);
};

class jPreview: public JUNGLE_WIDGET_CLASS
{
	Q_OBJECT
	PDATA
public:
	jPreview(QWidget * _parent = 0);
	jPreview(jView * _view, QWidget * _parent = 0);
	~jPreview();

	jPreview & setView(jView * _view);
	const jView * view() const;

	jPreview & setBackground(const QBrush & _brush);
	QBrush background() const;

	jPreview & setOrientation(int _orientation);
	int orientation() const;

	jSelector & selector() const;
	jRenderer & renderer() const;

	virtual void render(QPainter & _painter) const;

	QSize minimumSizeHint() const;

	jPreview & setInputPattern(const jInputPattern & _pattern);
	jInputPattern & inputPattern() const;

	jPreview & setXAxisVisible(bool _state);
	bool isXAxisVisible() const;
	jPreview & setYAxisVisible(bool _state);
	bool isYAxisVisible() const;
	virtual bool userCommand(int, int, int, int, QPointF, QWidget *); // jInputPattern::Action, jInputPattern::Method, buttons or key, modifiers or delta, mouse position

	jPreview & setPanCursor(const QCursor & _pan_cursor);
	QCursor panCursor() const;

	QPointF previewToViewScreen(const QPointF & _point) const;
	QPointF viewToPreviewScreen(const QPointF & _point) const;

	jPreview & setMinimumSelectorSize(int _min_dim);
	int minimumSelectorSize() const;
public slots:
	void rebuild();
	void actionAccepted(int, int, int, int, QPointF, QWidget *); // jInputPattern::Action, jInputPattern::Method, buttons or key, modifiers or delta, mouse position
protected:
    void resizeEvent(QResizeEvent *);
protected slots:
	void viewDestroyed(QObject *);
};

//! Class jRenderer is a 2-D rendering engine.
/*!
 Lazy renderer is automatically integrated to jView and jPreview. Renderer has two states. 
 The first state - disabled - is for classic one-threaded direct rendering in main GUI thread.
 The second state - enabled - is for multi-threaded rendering. In this mode pictures prepared by rendering threads are placed on a widget, so there's no direct drawing.
 All methods of this class are reentrant and thread safe.
 \sa setEnabled(), jView, jPreview
*/
class jRenderer: public QObject
{
	Q_OBJECT
	PDATA
public:
	//! Prototype for rendering function.
	/*!
	Rendering function should have two parameters: pointer to widget that will render image and reference to painter which should be used for rendering.
	\sa jRenderer()
	*/
	typedef void (* render_func)(QWidget *, QPainter &);
	//! Constructor.
	/*!
	\param _widget pointer to widget
	\param _render_func pointer to function for rendering on widget
	\sa render_func
	*/
	jRenderer(QWidget * _widget, render_func _render_func);
	//!
	/*!
	Destructor.
	*/
	~jRenderer();

	//! Counter of rendered frames.
	/*!
	\return number of frames
	*/
    quint64 counter() const;

	jRenderer & setHighlightColor(const QColor &);
	QColor highlightColor() const;

	enum HighlightMode {HighlightDisabled = 0, HighlightOnFocus = 1, HighlightEnabled = 2, HighlightPermanent = HighlightEnabled};
	jRenderer & setHighlightMode(HighlightMode);
	HighlightMode highlightMode();

public slots:
	//! Initiates rendering and QWidget::update().
	/*!
	*/
	void rebuild();
protected:
	bool eventFilter(QObject *, QEvent *);
};

class jSync: public QObject
{
	Q_OBJECT
	PDATA
public:
	jSync();
	~jSync();

	jSync & setViews(const QVector<jView *> & _views);
	const QVector<jView *> & views() const;

	jSync & setPreviews(const QVector<jPreview *> & _previews);
	const QVector<jPreview *> & previews() const;

	void reset();
public slots:
	void rebuild();
private slots:
	void onZoomedIn(const QRectF &);
	void onZoomedOut(const QRectF &);
	void onZoomedFullView(const QRectF &);
	void onPanned(const QRectF &);
};

class jLegend: public QWidget
{
	Q_OBJECT
	PDATA
public:
	jLegend(QWidget * _parent = 0, Qt::WindowFlags _flags = Qt::Tool);
	~jLegend();

	jLegend & setItems(const QVector<jItem *> & _items);
	QVector<jItem *> items() const;

	enum {Simple = 0, ShowHidden = 1, VisibilityControl = 2, UpDownControl = 4, Complex = ShowHidden | VisibilityControl | UpDownControl};
	jLegend & setBehavior(int _behavior);
	int behavior() const;
public slots:
	void rebuild();
private slots:
	void onVisibilityChanged();
	void onZOrderChanged(int);
};

#endif // JVIEW_H
