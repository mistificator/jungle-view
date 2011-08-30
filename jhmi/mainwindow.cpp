#include "mainwindow.h"

QString customRangeFunc(double _value, jAxis *)
{
	return QString::number(_value, '.', 2);
}

Mainwindow::Mainwindow(QWidget * _parent, Qt::WFlags _flags)
	: QMainWindow(_parent, _flags)
{
	ui.setupUi(this);

	QBoxLayout * _layout = new QVBoxLayout();
	view = new jView();
	another_view = new jView();
	another_view->setWindowTitle("Sync Demo");
	preview = new jPreview(view);
	preview->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	preview->setMinimumHeight(16);
	preview->
		setBackground(QBrush(Qt::black)).
		setOrientation(Qt::Horizontal);

	_layout->addWidget(preview);
	_layout->addWidget(view);
	ui.demowidget->setLayout(_layout);

	another_view->show();

	const int _sz_2d = 524288;
	double * _gfx2d = new double[_sz_2d];
	for (int _idx = 0; _idx < _sz_2d; _idx++)
	{
		_gfx2d[_idx] = _idx * 256;
	}

	item2d.
		setImageConversionFlags(Qt::ThresholdDither).
		setData(_gfx2d, _sz_2d / 256, 256, true).
		setOrigin(QPointF(100, -2)).
		setZ(0).
		setPen(QPen(Qt::green, 5, Qt::SolidLine)).
		setBrush(Qt::red).
		setToolTip("<b>item2d</b>");
	delete [] _gfx2d;

	short _gfx1d[4096];
	for (unsigned int _idx = 0; _idx < sizeof(_gfx1d) / sizeof(_gfx1d[0]); _idx++)
	{
		_gfx1d[_idx] = (_idx / 8) - 16;
	}

	qreal _gfx_cos[5000];
	for (unsigned int _idx = 0; _idx < sizeof(_gfx_cos) / sizeof(_gfx_cos[0]); _idx++)
	{
		_gfx_cos[_idx] = 100.0*::cosf(_idx/500.0);
	}

	::memset(gfx_dots, 0, sizeof(gfx_dots));

	item1d.
		setPen(QPen(Qt::red, 5, Qt::SolidLine)).
		setBrush(QBrush(Qt::green, Qt::Dense5Pattern)).
		setData(_gfx1d, sizeof(_gfx1d) / sizeof(_gfx1d[0]), 1, true).
		setOrigin(QPointF(0, 7)).
		setZ(1).
		setToolTip("<i>item1d</i>");

	item1d.
		setLineStyle(jItem1D<short>::Lines);

	item_cos.
		setZ(-1).
		setBrush(Qt::black).
		setOrigin(QPointF(-300, 100)).
		setData(_gfx_cos, sizeof(_gfx_cos) / sizeof(_gfx_cos[0]), 1, true).
		setToolTip("item_cos");

	item_cos.
		setLineStyle(jItem1D<qreal>::Ticks);

	item_dots.
		setPen(QPen(Qt::green, 7, Qt::SolidLine, Qt::RoundCap)).
		setBrush(QBrush(Qt::green, Qt::SolidPattern)).
		setOrigin(QPointF(500, 200)).
		setToolTip("item_dots").
		setData(gfx_dots, sizeof(gfx_dots) / sizeof(gfx_dots[0]));

	item_dots.
		setLineStyle(jItem1D<short>::Dots).
		setDataModel(jItem1D<qreal>::PointData);

	highlight.
		setVisible(false).
		setPen(QPen(Qt::red)).
		setBackground(QColor(255, 0, 0, 100));

	view->
		setXAxis(
			&x_axis.
			setLog10Multiplier(1000.0).
			setId(1).
			setRange(-333, 5060).
			setPen(QPen(Qt::blue, 3)).
			setStep(30, 25.0).
			setBackground(QBrush(QColor(255, 255, 255, 80))).
			setGridPen(QPen(Qt::blue, 1, Qt::DotLine))
		).
		setYAxis(
			&y_axis.
			setLog10Multiplier(100.0).
			setId(2).
			setRange(-11, 400, &customRangeFunc).
			setPen(QPen(Qt::blue, 3)).
			setStep(30, 1.25).
			setBackground(QBrush(QColor(255, 255, 255, 80))).
			setGridPen(QPen(Qt::blue, 1, Qt::DotLine))
		).
		setBackground(
			Qt::gray
		).
		setItems(
			QVector<jItem *>() << &item_dots << &item2d << &item1d << &item_cos
		).
		setSelectors(
			QVector<jSelector *>() << &highlight
		);

	view->coordinator().label().
		setPen(QPen(Qt::yellow)).
		setBackground(QBrush(QColor(0, 0, 0, 80), Qt::SolidPattern));
	view->viewport().
		setZoomOrientation(Qt::Horizontal | Qt::Vertical).
		setMinimumSize(QSizeF(0.5, 0.5)).
		setMaximumSize(QSizeF(1e38, 1e38));
	view->viewport().selector().
		setPen(QPen(Qt::green, 3, Qt::DotLine)).
		setBackground(QColor(255, 255, 255, 40));
	view->horizontalMarker().
		setVisible(true).
		setPen(QPen(Qt::black, 1, Qt::DotLine));
	view->verticalMarker().
		setVisible(true).
		setPen(QPen(Qt::black, 1, Qt::DotLine));

	preview->selector().
		setPen(QPen(Qt::yellow, 2, Qt::DotLine)).
		setBackground(QColor(0, 255, 0, 200));

	menu.addAction("Full view", this, SLOT(on_fullview()));
	connect(view, SIGNAL(contextMenuRequested(QPoint)), this, SLOT(on_view_contextMenuRequested(QPoint)));

	another_view->
		setXAxis(&x_axis).
		setYAxis(&y_axis).
		setBackground(
			Qt::white
		).
		setItems(
			QVector<jItem *>() << &item_cos
		);
	another_view->coordinator().label().
		setPen(QPen(Qt::black)).
		setBackground(QBrush(Qt::white));

	sync.
		setViews(QVector<jView *>() << view << another_view).
		setPreviews(QVector<jPreview *>() << preview);

	view->lazyRenderer().
		setEnabled(ui.mt_render->isChecked()).
		setMaxThreads(ui.threads_per_view->value());
	preview->lazyRenderer().
		setEnabled(ui.mt_render->isChecked());
	another_view->lazyRenderer().
		setEnabled(ui.mt_render->isChecked()).
		setMaxThreads(ui.threads_per_view->value());

	view_legend = new jLegend(view);
	view_legend->
		setItems(QVector<jItem *>() << &item_dots << &item_cos << &item1d).
		setBehavior(jLegend::Complex);
	view_legend->show();

	view->inputPattern()->
                addAction(jInputPattern::ZoomFullView, jInputPattern::MousePress, Qt::MidButton).
                addAction(jInputPattern::ZoomDelta, jInputPattern::MousePress, Qt::MidButton, Qt::ShiftModifier);

	const quint64 _arr_sz = 100000000;
	qint32 * _arr = new qint32[_arr_sz];
	for (quint64 _idx = 0; _idx < _arr_sz; _idx++)
	{
		_arr[_idx] = _idx;
	}
	storage = new jMemoryStorage<qint32>(_arr, _arr_sz);
//	storage = new jFileStorage<qint16>("c:/temp/compress_test.random");

	connect(storage->storageControl(), SIGNAL(finished(quint64)), this, SLOT(on_storage_finished(quint64)));
	storage->setSegmentSize();
	storage->setProcessedItemsHint();
	storage->setChannels(2);
	storage->startProcessing();

	cached_item1d.
			setStorage(storage).
			setVisible(ui.hugedata_visible->isChecked()).
			setToolTip("item_huge_data");
	view->addItem(&cached_item1d);

	step = 10;
	frames_count = 0;
	prev_count = 0;
	prev_rendered_count = 0;
	fast_timer = startTimer(ui.timer_interval->value());
	slow_timer = startTimer(1000);

}

Mainwindow::~Mainwindow()
{
	cached_item1d.setStorage(0);
	delete storage;
}

void Mainwindow::on_view_contextMenuRequested(QPoint _pos)
{
	menu.popup(_pos);
}

void Mainwindow::on_fullview()
{
	view->viewport().zoomFullView();
	view->rebuild();
}

void Mainwindow::timerEvent(QTimerEvent * _te)
{
	if (_te->timerId() == fast_timer)
	{
		QPointF _origin = item2d.origin();
		if ((_origin.x() > 3000) || (_origin.x() < 0))
		{
			step *= -1;
		}
		item2d.setOrigin(QPointF(_origin.x() + step, _origin.y()));
		::qsrand(_origin.x());
		for (unsigned int _idx = 0; _idx < sizeof(gfx_dots) / sizeof(gfx_dots[0]); _idx++)
		{
			gfx_dots[_idx].x = ::qrand() / 8.0;
			gfx_dots[_idx].y = ::qrand() / 256.0;
		}
		view->rebuild();
		preview->rebuild();
		frames_count++;
	}
	if (_te->timerId() == slow_timer)
	{
		int _counter = view->lazyRenderer().counter(); 
		quint64 _rendered_counter = item1d.counter() + item2d.counter() + item_cos.counter() + item_dots.counter();
		ui.statusBar->showMessage(
			QString::number(
				view->lazyRenderer().isEnabled() ? _counter - prev_count : frames_count
				) + " frames per second, " + 
			QString::number(
				((double)_rendered_counter - (double)prev_rendered_count) / (double)1000000.0, '.', 1
				) + " millions primitives per second", 
				10000);
		view->autoScale();
		view_legend->rebuild();
		frames_count = 0;
		prev_count = _counter;
		prev_rendered_count = _rendered_counter;

//		JDEBUG("processed" << 100.0 * storage->itemsProcessed() / storage->storageSize());
	}
}

bool Mainwindow::event(QEvent * _ev)
{
	if (_ev->type() == QEvent::ToolTip)
	{
		QVector<jItem *> _items = view->showToolTip(view->mapFromGlobal(reinterpret_cast<QHelpEvent *>(_ev)->globalPos()));
		if (_items.count() > 0)
		{
			highlight.setVisible(true);
			highlight.setRect(_items[0]->boundingRect(view->xAxis(), view->yAxis()));
		}
		else
		{
			highlight.setVisible(false);
		}
		sync.rebuild();
		return true;
	}
	return QWidget::event(_ev);
}

void Mainwindow::closeEvent(QCloseEvent *)
{
	view->lazyRenderer().
		setEnabled(false);
	preview->lazyRenderer().
		setEnabled(false);
	another_view->lazyRenderer().
		setEnabled(false);

	sync.reset();
	delete another_view;
}

void Mainwindow::on_mt_render_clicked()
{
	view->lazyRenderer().
		setEnabled(ui.mt_render->isChecked());
	preview->lazyRenderer().
		setEnabled(ui.mt_render->isChecked());
	another_view->lazyRenderer().
		setEnabled(ui.mt_render->isChecked());
}

void Mainwindow::on_timer_interval_valueChanged(int _value)
{
	killTimer(fast_timer);
	fast_timer = startTimer(_value);
}

void Mainwindow::on_threads_per_view_valueChanged(int _value)
{
	view->lazyRenderer().setMaxThreads(_value);
	another_view->lazyRenderer().setMaxThreads(_value);
}

void Mainwindow::on_stairway_visible_clicked()
{
	item1d.setVisible(ui.stairway_visible->isChecked());
	sync.rebuild();
}

void Mainwindow::on_cos_visible_clicked()
{
	item_cos.setVisible(ui.cos_visible->isChecked());
	sync.rebuild();
}

void Mainwindow::on_dots_visible_clicked()
{
	item_dots.setVisible(ui.dots_visible->isChecked());
	sync.rebuild();
}

void Mainwindow::on_gradient_visible_clicked()
{
	item2d.setVisible(ui.gradient_visible->isChecked());
	sync.rebuild();
}

void Mainwindow::on_hugedata_visible_clicked()
{
	cached_item1d.setVisible(ui.hugedata_visible->isChecked());
	sync.rebuild();
}

void Mainwindow::on_x_log_clicked()
{
	x_axis.setLog10ScaleEnabled(ui.x_log->isChecked());
	sync.rebuild();
}

void Mainwindow::on_y_log_clicked()
{
	y_axis.setLog10ScaleEnabled(ui.y_log->isChecked());
	sync.rebuild();
}

void Mainwindow::on_show_legend_clicked()
{
	view_legend->show();
	view_legend->raise();
}

void Mainwindow::on_storage_finished(quint64 _msecs)
{
	QMessageBox::information(this, "Storage info", "Layers building finished at " +
				 QString::number(_msecs / 1000.0, '.', 1) + "s, " +
				 QString::number(storage->storageSize() * storage->itemSize() * 1000.0 / (_msecs * 1024.0 * 1024.0), '.', 1) + "MB/s");
}
