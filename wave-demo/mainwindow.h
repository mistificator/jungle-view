#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressBar>
#include "ui_mainwindow.h"
#include "../jhmi/jview.h"
#include "../jhmi/jwave.h"
#include "../jhmi/jitems.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
	~MainWindow();

private:
	Ui::MainWindowClass ui;
	QProgressBar * pb;
	QVector<jItem *> wave_items;
	jAxis x_axis, y_axis;
	static QString range_convert(double _value, jAxis *);
	static QString time_convert(double _value, jAxis *);
	static QString coordinator_format(double _x, double _y, jAxis * _x_axis, jAxis * _y_axis, jCoordinator *);

	qint32 sample_rate;
	QFile file;
	jWaveFile wave_file;
	jFigureItem<qint64> * highlight;
	jItem1D<qreal> * highlight_line;
private slots:
	void on_actionOpen_triggered();
	void on_actionQuit_triggered();
	void on_actionExportSnapshot_triggered();
	void onLayersAdjusted();
	void onViewportChanged(QRectF);
};

#endif // MAINWINDOW_H
