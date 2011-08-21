/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created: Sun 3. Apr 23:51:35 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Mainwindow[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      17,   12,   11,   11, 0x08,
      54,   11,   11,   11, 0x08,
      68,   11,   11,   11, 0x08,
      91,   11,   11,   11, 0x08,
     127,   11,   11,   11, 0x08,
     165,   11,   11,   11, 0x08,
     195,   11,   11,   11, 0x08,
     220,   11,   11,   11, 0x08,
     246,   11,   11,   11, 0x08,
     276,   11,   11,   11, 0x08,
     295,   11,   11,   11, 0x08,
     314,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Mainwindow[] = {
    "Mainwindow\0\0_pos\0on_view_contextMenuRequested(QPoint)\0"
    "on_fullview()\0on_mt_render_clicked()\0"
    "on_timer_interval_valueChanged(int)\0"
    "on_threads_per_view_valueChanged(int)\0"
    "on_stairway_visible_clicked()\0"
    "on_cos_visible_clicked()\0"
    "on_dots_visible_clicked()\0"
    "on_gradient_visible_clicked()\0"
    "on_x_log_clicked()\0on_y_log_clicked()\0"
    "on_show_legend_clicked()\0"
};

const QMetaObject Mainwindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_Mainwindow,
      qt_meta_data_Mainwindow, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Mainwindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Mainwindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Mainwindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Mainwindow))
        return static_cast<void*>(const_cast< Mainwindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int Mainwindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_view_contextMenuRequested((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 1: on_fullview(); break;
        case 2: on_mt_render_clicked(); break;
        case 3: on_timer_interval_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: on_threads_per_view_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: on_stairway_visible_clicked(); break;
        case 6: on_cos_visible_clicked(); break;
        case 7: on_dots_visible_clicked(); break;
        case 8: on_gradient_visible_clicked(); break;
        case 9: on_x_log_clicked(); break;
        case 10: on_y_log_clicked(); break;
        case 11: on_show_legend_clicked(); break;
        default: ;
        }
        _id -= 12;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
