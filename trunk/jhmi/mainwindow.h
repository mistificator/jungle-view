#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "jview.h"
#include "jitems.h"

class Mainwindow : public QMainWindow
{
	Q_OBJECT

public:
	Mainwindow(QWidget * _parent = 0, Qt::WFlags _flags = 0);
	~Mainwindow();

private:
	Ui::mainwindowClass ui;

	jView * view, * another_view;
	jSync sync;
	jPreview * preview;
	jLegend * view_legend;
	jItem1D<short> item1d;
	jItem2D<double> item2d;
	jItem1D<float> item_cos;
	jItem1D<float> item_dots;
	jItem1D<float>::Point gfx_dots[100];
	jSelector highlight;
	jAxis x_axis, y_axis;

	QMenu menu;

	int fast_timer, slow_timer, frames_count, step, prev_count, prev_rendered_count;
	void timerEvent(QTimerEvent * _te);
        bool event (QEvent * _ev);
	void closeEvent(QCloseEvent *);

private slots:
	void on_view_contextMenuRequested(QPoint _pos);
	void on_fullview();
	void on_mt_render_clicked();
	void on_timer_interval_valueChanged(int);
	void on_threads_per_view_valueChanged(int);
	void on_stairway_visible_clicked();
	void on_cos_visible_clicked();
	void on_dots_visible_clicked();
	void on_gradient_visible_clicked();
	void on_x_log_clicked();
	void on_y_log_clicked();
	void on_show_legend_clicked();
};

#endif // MAINWINDOW_H