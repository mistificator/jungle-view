#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "ui_mainwindow.h"
#include "../jhmi/jview.h"
#include "../jhmi/jwave.h"
#include "../jhmi/jcacheditems.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
	~MainWindow();

private:
	Ui::MainWindowClass ui;
//	jWaveFile wave_file;
	jFileStorage<qint16, qreal> wave_file;
	QVector<jItem *> wave_items;
	jAxis x_axis, y_axis;
private slots:
	void on_actionOpen_triggered();
	void on_actionQuit_triggered();
	void onLayersAdjusted();
};

#endif // MAINWINDOW_H
