#include "mainwindow.h"
#include <QFileDialog>

QString MainWindow::range_convert(double _value, jAxis *)
{
	qint64 _value_i64 = _value + 0.5;
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

MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	ui.statusBar->addWidget(pb = new QProgressBar());
	pb->setVisible(false);

	ui.view->setBackground(Qt::lightGray);
	ui.view->lazyRenderer().
			setMaxThreads(2).
			setEnabled(false);
	ui.view->setXAxis(
		&x_axis.
		setGridPen(QPen(Qt::black, 1, Qt::DotLine)).
		setFont(QFont("Arial Narrow")).
		setBackground(QColor(255, 255, 255, 60)).
		setRangeFunc(&range_convert)
		);
	ui.view->setYAxis(
		&y_axis.
		setGridPen(QPen(Qt::black, 1, Qt::DotLine)).
		setFont(QFont("Arial Narrow")).
		setBackground(QColor(255, 255, 255, 60)).
		setRangeFunc(&range_convert)
		);
	ui.view->viewport().selector().
		setPen(QPen(Qt::green, 2, Qt::DotLine)).
		setBackground(QColor(255, 255, 255, 60));
	ui.view->coordinator().label().
		setPen(QPen(Qt::white)).
		setBackground(QColor(0, 0, 0, 120)).
		setFont(QFont("Arial Narrow"));

	ui.preview->lazyRenderer().
		setEnabled(false);
	ui.preview->selector().
		setPen(QPen(Qt::yellow, 1, Qt::DotLine)).
		setBackground(QColor(0, 255, 0, 100));

	ui.preview->
		setBackground(Qt::lightGray).
		setView(ui.view).
		setXAxisVisible(true);
}

MainWindow::~MainWindow()
{
	for (int _idx = 0; _idx < wave_items.count(); _idx++)
	{
		delete wave_items[_idx];
	}
	ui.view->clear();
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
	wave_file.setFile(_file_name, false);
	connect(wave_file.storageControl(), SIGNAL(layersAdjusted()), this, SLOT(onLayersAdjusted()));
	connect(wave_file.storageControl(), SIGNAL(finished(quint64)), this, SLOT(onLayersAdjusted()));
	connect(wave_file.storageControl(), SIGNAL(stopped()), this, SLOT(onLayersAdjusted()));
	
	wave_file.storage()->setSegmentSize(1024 * wave_file.channels());
	wave_items.resize(wave_file.channels());
	for (int _idx = 0; _idx < wave_items.count(); _idx++)
	{
		wave_items[_idx] = wave_file.createItem(_idx);
		wave_items[_idx]->
			setZ(_idx).
			setPen(QPen((Qt::GlobalColor)(Qt::darkRed + _idx), 1, Qt::SolidLine));
	}
	ui.view->addItems(wave_items);

	bool _snapshot_loaded = false;
	if (QFile(wave_file.fileName() + ".snapshot").exists())
	{
		QFile _snapshot_file(wave_file.fileName() + ".snapshot");
		_snapshot_file.open(QFile::ReadOnly);
		_snapshot_loaded = wave_file.storage()->importLayers(_snapshot_file.readAll());
		_snapshot_file.close();
	}
	if (!_snapshot_loaded)
	{
		wave_file.storage()->startProcessing();
	}
}

void MainWindow::on_actionQuit_triggered()
{
	QApplication::quit();
}

void MainWindow::on_actionExportSnapshot_triggered()
{
	QFile _snapshot_file(wave_file.fileName() + ".snapshot");
	_snapshot_file.open(QFile::WriteOnly);
	_snapshot_file.write(wave_file.storage()->exportLayers());
	_snapshot_file.close();
}

void MainWindow::onLayersAdjusted()
{
	pb->setValue(wave_file.storage()->itemsProcessed() * 100.0 / wave_file.storage()->storageSize());
	pb->setVisible(pb->value() < 100);
	ui.actionExportSnapshot->setEnabled(pb->value() == 100);
	ui.view->autoScale(0.005, 0.05);
	ui.view->rebuild();
	ui.preview->rebuild();
}
