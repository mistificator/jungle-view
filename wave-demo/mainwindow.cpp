#include "mainwindow.h"
#include <QFileDialog>

int MainWindow::g_sample_rate = 8000;
jWaveFile MainWindow::g_wave_file;
jFigureItem<qint64> * MainWindow::g_highlight = 0;
jItem1D<qreal> * MainWindow::g_highlight_line = 0;

QString MainWindow::range_convert(double _value, jAxis *)
{
	qint64 _value_i64 = _value;
	QString _label;
	int _count = 0;
	while (_value_i64 != 0)
	{
		qint64 _rem = _value_i64 % 10;
		_label = QString::number(qAbs(_rem)) + _label;
		_value_i64 /= 10;
		_count++;
		if ((_count % 3 == 0) && (_value_i64 != 0))
		{
			_label = " " + _label;
		}
	}
	if (_value < 0)
	{
		_label = "-" + _label;
	}
	return _label;
}

QString MainWindow::time_convert(double _value, jAxis *)
{
	QString _time_str = QTime(0, 0, 0).addMSecs((_value >= 0.0 ?_value : -_value) * 1000.0 / g_sample_rate).toString(_value >= 0.0 ? QString("mm:ss.zzz") : QString("-mm:ss.zzz"));
	return _time_str;
}

QString MainWindow::coordinator_format(double _x, double _y, jAxis * _x_axis, jAxis * /*_y_axis */, jCoordinator *)
{
	double usecs = (((_x >= 0.0 ? _x : -_x) * 1000.0 / g_sample_rate) - (qint64)((_x >= 0.0 ?_x : - _x) * 1000.0 / g_sample_rate));

	QString _x_str = _x_axis->rangeFunc()(_x, _x_axis) + QString::number(usecs, 'f', 6).mid(1);
	qint64 _y_val = g_wave_file.item(qMax<qint64>(_x, 0), g_wave_file.channels() - 1);
	QString _y_str = "y=" + QString::number(_y_val);

	jFigureItem<qint64>::Point _point;
	_point.x = _x; _point.y = _y_val;
	g_highlight->setData(&_point, 1, true);

	jItem1D<qreal>::Point _point_line[3];
	_point_line[0].x = _x; _point_line[0].y = _y;
	_point_line[1].x = _point.x; _point_line[1].y = _y;
	_point_line[2].x = _point.x; _point_line[2].y = _y_val;
	g_highlight_line->setData(_point_line, 3, true);

	return _x_str + "\n" + _y_str;
}

MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	ui.statusBar->addWidget(pb = new QProgressBar());
	pb->setVisible(false);

	connect(& ui.view->viewport(), SIGNAL(panned(QRectF)), this, SLOT(onViewportChanged(QRectF)));
	connect(& ui.view->viewport(), SIGNAL(zoomedIn(QRectF)), this, SLOT(onViewportChanged(QRectF)));
	connect(& ui.view->viewport(), SIGNAL(zoomedOut(QRectF)), this, SLOT(onViewportChanged(QRectF)));
	connect(& ui.view->viewport(), SIGNAL(zoomedFullView(QRectF)), this, SLOT(onViewportChanged(QRectF)));

	ui.view->setBackground(Qt::lightGray);
	ui.view->lazyRenderer().
			setMaxThreads(2).
			setEnabled(false);
	ui.view->setXAxis(
		&x_axis.
		setGridPen(QPen(Qt::black, 1, Qt::DotLine)).
		setFont(QFont("Arial Narrow")).
		setBackground(QColor(255, 255, 255, 80)).
		setRangeFunc(&time_convert)
		);
	ui.view->setYAxis(
		&y_axis.
		setGridPen(QPen(Qt::black, 1, Qt::DotLine)).
		setFont(QFont("Arial Narrow")).
		setBackground(QColor(255, 255, 255, 80)).
		setRangeFunc(&range_convert)
		);
	ui.view->viewport().selector().
		setPen(QPen(Qt::green, 2, Qt::DotLine)).
		setBackground(QColor(255, 255, 255, 60));
	ui.view->coordinator().label().
		setPen(QPen(Qt::white)).
		setBackground(QColor(0, 0, 0, 120)).
		setFont(QFont("Arial Narrow"));
	ui.view->coordinator().
		setFormat(&coordinator_format);

	ui.preview->lazyRenderer().
		setEnabled(false);
	ui.preview->selector().
		setPen(QPen(Qt::yellow, 2, Qt::DotLine)).
		setBackground(QColor(0, 255, 0, 100));

	ui.preview->
		setBackground(Qt::lightGray).
		setView(ui.view).
		setXAxisVisible(true);

	g_highlight = new jFigureItem<qint64>();
	g_highlight->
		setZ(100).
		setPen(QPen(Qt::blue)).
		setBrush(Qt::blue).
		setVisible(false);
	g_highlight->
		setEllipseSymbol(QRectF(0, 0, 5, 5));

	g_highlight_line = new jItem1D<qreal>();
	g_highlight_line->
		setZ(99).
		setPen(QPen(Qt::blue, 1, Qt::DotLine)).
		setVisible(false);
}

MainWindow::~MainWindow()
{
	for (int _idx = 0; _idx < wave_items.count(); _idx++)
	{
		delete wave_items[_idx];
	}
	ui.view->clear();
	delete g_highlight;
	delete g_highlight_line;
}

void MainWindow::on_actionOpen_triggered()
{
	QString _file_name = QFileDialog::getOpenFileName(this, "Open wave file", QDir::currentPath(), "*.wav");
	if (_file_name.isEmpty())
	{
		return;
	}
	ui.actionExportSnapshot->setEnabled(false);
	pb->setVisible(true);
	pb->setValue(0);
	ui.view->clear();
	for (int _idx = 0; _idx < wave_items.count(); _idx++)
	{
		delete wave_items[_idx];
	}
	g_wave_file.setFile(_file_name, false);
	disconnect(this, SLOT(onLayersAdjusted()));
	connect(g_wave_file.storageControl(), SIGNAL(layersAdjusted()), this, SLOT(onLayersAdjusted()));
	connect(g_wave_file.storageControl(), SIGNAL(finished(quint64)), this, SLOT(onLayersAdjusted()));
	connect(g_wave_file.storageControl(), SIGNAL(stopped()), this, SLOT(onLayersAdjusted()));
	
	g_sample_rate = g_wave_file.sampleRate();
	x_axis.setStep(10, g_sample_rate / 1000.0);
	y_axis.setStep(10, 1);
	ui.view->viewport().setMinimumSize(QSize(x_axis.alignment(), y_axis.alignment()));
	g_wave_file.storage()->setSegmentSize(1024 * g_wave_file.channels());
	wave_items.resize(g_wave_file.channels());
	for (int _idx = 0; _idx < wave_items.count(); _idx++)
	{
		wave_items[_idx] = g_wave_file.createItem(_idx);
		wave_items[_idx]->
			setZ(_idx).
			setPen(QPen((Qt::GlobalColor)(Qt::darkRed + _idx), 1, Qt::SolidLine));
	}
	ui.view->addItems(wave_items);
	ui.view->addItem(g_highlight);
	ui.view->addItem(g_highlight_line);

	bool _snapshot_loaded = false;
	if (QFile(g_wave_file.fileName() + ".snapshot").exists())
	{
		QFile _snapshot_file(g_wave_file.fileName() + ".snapshot");
		_snapshot_file.open(QFile::ReadOnly);
		_snapshot_loaded = g_wave_file.storage()->importLayers(_snapshot_file.readAll());
		_snapshot_file.close();
	}
	if (!_snapshot_loaded)
	{
		g_wave_file.storage()->startProcessing();
	}
}

void MainWindow::on_actionQuit_triggered()
{
	QApplication::quit();
}

void MainWindow::on_actionExportSnapshot_triggered()
{
	QFile _snapshot_file(g_wave_file.fileName() + ".snapshot");
	_snapshot_file.open(QFile::WriteOnly);
	_snapshot_file.write(g_wave_file.storage()->exportLayers());
	_snapshot_file.close();
}

void MainWindow::onLayersAdjusted()
{
	pb->setValue(g_wave_file.storage()->itemsProcessed() * 100.0 / g_wave_file.storage()->storageSize());
	pb->setVisible(pb->value() < 100);
	ui.actionExportSnapshot->setEnabled(pb->value() == 100);
	ui.view->autoScale(0.005, 0.05);
	ui.view->rebuild();
	ui.preview->rebuild();
}

void MainWindow::onViewportChanged(QRectF _rect)
{
	qint64 _count = g_wave_file.storage()->processedItemsHint();
	bool _condition_visibility = (_rect.width() <= 10 * _count);
	bool _condition_symbol = (_rect.width() <= _count);
	g_highlight->setVisible(_condition_visibility);
	g_highlight_line->setVisible(_condition_visibility);
	for (int _idx = 0; _idx < wave_items.count(); _idx++)
	{
		QImage _symbol_img;
		if (_condition_symbol)
		{
			jFigureItem<qreal> _fig_item;
			_fig_item.
				setZ(98).
				setPen(QPen(wave_items[_idx]->pen().color())).
				setBrush(Qt::NoBrush);
			_symbol_img = _fig_item.
							setEllipseSymbol(QRectF(0, 0, 5, 5)).symbol();

		}
		wave_items[_idx]->setSymbol(_symbol_img);
	}
}

