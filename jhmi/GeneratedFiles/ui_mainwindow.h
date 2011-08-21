/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Sun 3. Apr 22:58:20 2011
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QStatusBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_mainwindowClass
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *hboxLayout;
    QCheckBox *mt_render;
    QFrame *line;
    QLabel *label_3;
    QSpinBox *threads_per_view;
    QFrame *line_2;
    QLabel *label;
    QSpinBox *timer_interval;
    QLabel *label_2;
    QSpacerItem *spacerItem;
    QHBoxLayout *horizontalLayout;
    QLabel *label_4;
    QCheckBox *stairway_visible;
    QCheckBox *cos_visible;
    QCheckBox *dots_visible;
    QCheckBox *gradient_visible;
    QFrame *line_3;
    QCheckBox *x_log;
    QCheckBox *y_log;
    QSpacerItem *horizontalSpacer;
    QPushButton *show_legend;
    QWidget *demowidget;
    QMenuBar *menuBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *mainwindowClass)
    {
        if (mainwindowClass->objectName().isEmpty())
            mainwindowClass->setObjectName(QString::fromUtf8("mainwindowClass"));
        mainwindowClass->resize(614, 414);
        centralWidget = new QWidget(mainwindowClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        hboxLayout = new QHBoxLayout();
        hboxLayout->setSpacing(6);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        mt_render = new QCheckBox(centralWidget);
        mt_render->setObjectName(QString::fromUtf8("mt_render"));

        hboxLayout->addWidget(mt_render);

        line = new QFrame(centralWidget);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Sunken);

        hboxLayout->addWidget(line);

        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        hboxLayout->addWidget(label_3);

        threads_per_view = new QSpinBox(centralWidget);
        threads_per_view->setObjectName(QString::fromUtf8("threads_per_view"));
        threads_per_view->setEnabled(false);
        threads_per_view->setMinimum(1);
        threads_per_view->setMaximum(10);
        threads_per_view->setValue(2);

        hboxLayout->addWidget(threads_per_view);

        line_2 = new QFrame(centralWidget);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setFrameShape(QFrame::VLine);
        line_2->setFrameShadow(QFrame::Sunken);

        hboxLayout->addWidget(line_2);

        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);

        hboxLayout->addWidget(label);

        timer_interval = new QSpinBox(centralWidget);
        timer_interval->setObjectName(QString::fromUtf8("timer_interval"));
        timer_interval->setMinimum(1);
        timer_interval->setMaximum(1000);
        timer_interval->setValue(30);

        hboxLayout->addWidget(timer_interval);

        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        sizePolicy.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy);

        hboxLayout->addWidget(label_2);

        spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem);


        verticalLayout->addLayout(hboxLayout);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_4 = new QLabel(centralWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        horizontalLayout->addWidget(label_4);

        stairway_visible = new QCheckBox(centralWidget);
        stairway_visible->setObjectName(QString::fromUtf8("stairway_visible"));
        stairway_visible->setChecked(true);

        horizontalLayout->addWidget(stairway_visible);

        cos_visible = new QCheckBox(centralWidget);
        cos_visible->setObjectName(QString::fromUtf8("cos_visible"));
        cos_visible->setChecked(true);

        horizontalLayout->addWidget(cos_visible);

        dots_visible = new QCheckBox(centralWidget);
        dots_visible->setObjectName(QString::fromUtf8("dots_visible"));
        dots_visible->setChecked(true);

        horizontalLayout->addWidget(dots_visible);

        gradient_visible = new QCheckBox(centralWidget);
        gradient_visible->setObjectName(QString::fromUtf8("gradient_visible"));
        gradient_visible->setChecked(true);

        horizontalLayout->addWidget(gradient_visible);

        line_3 = new QFrame(centralWidget);
        line_3->setObjectName(QString::fromUtf8("line_3"));
        line_3->setFrameShape(QFrame::VLine);
        line_3->setFrameShadow(QFrame::Sunken);

        horizontalLayout->addWidget(line_3);

        x_log = new QCheckBox(centralWidget);
        x_log->setObjectName(QString::fromUtf8("x_log"));

        horizontalLayout->addWidget(x_log);

        y_log = new QCheckBox(centralWidget);
        y_log->setObjectName(QString::fromUtf8("y_log"));

        horizontalLayout->addWidget(y_log);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        show_legend = new QPushButton(centralWidget);
        show_legend->setObjectName(QString::fromUtf8("show_legend"));

        horizontalLayout->addWidget(show_legend);


        verticalLayout->addLayout(horizontalLayout);

        demowidget = new QWidget(centralWidget);
        demowidget->setObjectName(QString::fromUtf8("demowidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(demowidget->sizePolicy().hasHeightForWidth());
        demowidget->setSizePolicy(sizePolicy1);

        verticalLayout->addWidget(demowidget);

        mainwindowClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(mainwindowClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 614, 20));
        mainwindowClass->setMenuBar(menuBar);
        statusBar = new QStatusBar(mainwindowClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        mainwindowClass->setStatusBar(statusBar);

        retranslateUi(mainwindowClass);
        QObject::connect(mt_render, SIGNAL(clicked(bool)), threads_per_view, SLOT(setEnabled(bool)));

        QMetaObject::connectSlotsByName(mainwindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *mainwindowClass)
    {
        mainwindowClass->setWindowTitle(QApplication::translate("mainwindowClass", "JungleView Demo", 0, QApplication::UnicodeUTF8));
        mt_render->setText(QApplication::translate("mainwindowClass", "Multithreaded rendering", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("mainwindowClass", "Threads per view", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("mainwindowClass", "Timer interval", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("mainwindowClass", "msecs", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("mainwindowClass", "Visible items:", 0, QApplication::UnicodeUTF8));
        stairway_visible->setText(QApplication::translate("mainwindowClass", "Stairway", 0, QApplication::UnicodeUTF8));
        cos_visible->setText(QApplication::translate("mainwindowClass", "Cos", 0, QApplication::UnicodeUTF8));
        dots_visible->setText(QApplication::translate("mainwindowClass", "Crazy dots", 0, QApplication::UnicodeUTF8));
        gradient_visible->setText(QApplication::translate("mainwindowClass", "Gradient", 0, QApplication::UnicodeUTF8));
        x_log->setText(QApplication::translate("mainwindowClass", "x log", 0, QApplication::UnicodeUTF8));
        y_log->setText(QApplication::translate("mainwindowClass", "y log", 0, QApplication::UnicodeUTF8));
        show_legend->setText(QApplication::translate("mainwindowClass", "Show legend", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class mainwindowClass: public Ui_mainwindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
