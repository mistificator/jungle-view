#include "perf_test.h"

perf_test::perf_test(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	ui.statusBar->addWidget(fps = new QLabel());

	current_data = 0;
	for (int k = 0; k < data_count; k++)
	{
		data1d[k].resize(ui.widthValue->currentText().toInt());
		for (int i = 0; i < data1d[k].count(); i++)
		{
			data1d[k][i] = qrand();
		}
		data2d[k].resize(ui.widthValue->currentText().toInt() * ui.heightValue->currentText().toInt());
		for (int i = 0; i < data2d[k].count(); i++)
		{
			data2d[k][i] = qrand() << 8;
		}
	}

	item1d.setPen(QColor(Qt::darkGreen)).setBrush(QColor(Qt::darkGreen));
	item1d.setLineStyle(jItem::Lines);
	item2d.setScaler(& scale_func);

	ui.view1d->addItem(& item1d);
	ui.view2d->addItem(& item2d);

	ui.preview1d->selector().setVisible(true).setPen(QColor(Qt::darkGreen)).setBackground(QColor(0, 255, 0, 80));
	ui.preview1d->setView(ui.view1d);

	ui.view1d->xAxis()->setRange(0, ui.widthValue->currentText().toInt());
	ui.view1d->xAxis()->setId(1);
	ui.view2d->setXAxis(ui.view1d->xAxis());
	ui.view2d->yAxis()->setRange(0, ui.heightValue->currentText().toInt());

	item2d.setOrigin(QPointF(-100, -200));

	sync.setViews(QVector<jView *>() << ui.view1d << ui.view2d);

	frames_count = 0;
	t.start();
	startTimer(1);
}

perf_test::~perf_test()
{

}

void perf_test::timerEvent(QTimerEvent *)
{
	item1d.setData(data1d[current_data].data(), ui.widthValue->currentText().toInt());
	ui.view1d->autoScaleY();
	ui.view1d->update();

	item2d.setData(data2d[current_data].data(), ui.widthValue->currentText().toInt(), ui.heightValue->currentText().toInt());
	ui.view2d->autoScale();
	ui.view2d->update();

	ui.preview1d->update();

	current_data = (current_data + 1) % data_count;
	frames_count++;

	int elapsed = t.elapsed();
	if (elapsed > 1000)
	{
		fps->setText(QString::number(frames_count * 1000.0 / (elapsed + 1), 'f', 1) + " fps");
		frames_count = 0;
		t.start();
	}
}

QImage perf_test::scale_func(const QImage & src_image, const QRectF & src_rect, const QSizeF & dst_size, jItem2D<quint32> * item)
{
	return src_image.copy(src_rect.toRect()).scaled(dst_size.toSize(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}
