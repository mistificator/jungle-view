#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressBar>
#include "ui_mainwindow.h"
#include "../jhmi/jview.h"
#include "../jhmi/jwave.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
	~MainWindow();

private:
	Ui::MainWindowClass ui;
	QProgressBar * pb;
	jWaveFile wave_file;
	QVector<jItem *> wave_items;
	jAxis x_axis, y_axis;
	static QString range_convert(double _value, jAxis *);
	static QString time_convert(double _value, jAxis *);
	static int g_sample_rate;
private slots:
	void on_actionOpen_triggered();
	void on_actionQuit_triggered();
	void on_actionExportSnapshot_triggered();
	void onLayersAdjusted();
};

#endif // MAINWINDOW_H
