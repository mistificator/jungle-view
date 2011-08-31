#include "../wave-demo/mainwindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
//	ui.view->setBackground(Qt::gray);
	ui.view->setXAxis(
		&x_axis.
		setId(1)
		);
	ui.view->setYAxis(
		&y_axis.
		setId(2)
		);

	connect(wave_file.storageControl(), SIGNAL(layersAdjusted()), this, SLOT(onLayersAdjusted()));
	connect(wave_file.storageControl(), SIGNAL(finished(quint64)), this, SLOT(onLayersAdjusted()));
	connect(wave_file.storageControl(), SIGNAL(stopped()), this, SLOT(onLayersAdjusted()));
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
	ui.view->clear();
	for (int _idx = 0; _idx < wave_items.count(); _idx++)
	{
		delete wave_items[_idx];
	}
	wave_file.setStorageFile(_file_name);
	wave_file.setOffset(44);
	wave_file.setChannels(2);
	wave_file.startProcessing();
	wave_items.resize(wave_file.channels());
	for (int _idx = 0; _idx < wave_items.count(); _idx++)
	{
//		wave_items[_idx] = new jCachedItem1D<qint64>(& wave_file, _idx);
		wave_items[_idx] = new jCachedItem1D<qint16, qreal>(& wave_file, _idx);
		wave_items[_idx]->
			setPen(QPen((Qt::GlobalColor)(Qt::darkRed + _idx), 1, Qt::SolidLine));
	}
	ui.view->addItems(wave_items);
}

void MainWindow::on_actionQuit_triggered()
{
	QApplication::quit();
}

void MainWindow::onLayersAdjusted()
{
	ui.view->autoScale();
	ui.view->rebuild();
}
