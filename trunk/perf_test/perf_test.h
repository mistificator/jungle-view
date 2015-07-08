#ifndef PERF_TEST_H
#define PERF_TEST_H

#include <QMainWindow>
#include <QTime>
#include <QLabel>
#include "ui_perf_test.h"
#include "./../jhmi/jitems.h"

class perf_test : public QMainWindow
{
	Q_OBJECT

public:
    perf_test(QWidget *parent = 0);
	~perf_test();

private:
	Ui::perf_testClass ui;

	jItem1D<float> item1d;
	jItem2D<quint32> item2d;
	jSync sync;

	enum {data_count = 10};
	QVector<float> data1d[data_count];
	QVector<quint32> data2d[data_count];
	int current_data;

	QTime t;
	int frames_count;
	QLabel * fps;

	void timerEvent(QTimerEvent *);
	static QImage scale_func(const QImage & src_image, const QRectF & src_rect, const QSizeF & dst_size, jItem2D<quint32> * item);
};

#endif // PERF_TEST_H
