/****************************************************************************
** Meta object code from reading C++ file 'jview.h'
**
** Created: Sun 3. Apr 23:51:36 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../jview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'jview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_jZoom[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
       7,    6,    6,    6, 0x05,
      24,    6,    6,    6, 0x05,
      42,    6,    6,    6, 0x05,
      65,    6,    6,    6, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_jZoom[] = {
    "jZoom\0\0zoomedIn(QRectF)\0zoomedOut(QRectF)\0"
    "zoomedFullView(QRectF)\0panned(QRectF)\0"
};

const QMetaObject jZoom::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_jZoom,
      qt_meta_data_jZoom, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &jZoom::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *jZoom::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *jZoom::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_jZoom))
        return static_cast<void*>(const_cast< jZoom*>(this));
    return QObject::qt_metacast(_clname);
}

int jZoom::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: zoomedIn((*reinterpret_cast< QRectF(*)>(_a[1]))); break;
        case 1: zoomedOut((*reinterpret_cast< QRectF(*)>(_a[1]))); break;
        case 2: zoomedFullView((*reinterpret_cast< QRectF(*)>(_a[1]))); break;
        case 3: panned((*reinterpret_cast< QRectF(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void jZoom::zoomedIn(QRectF _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void jZoom::zoomedOut(QRectF _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void jZoom::zoomedFullView(QRectF _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void jZoom::panned(QRectF _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
static const uint qt_meta_data_jView[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
       7,    6,    6,    6, 0x05,
      36,    6,    6,    6, 0x05,

 // slots: signature, parameters, type, tag, flags
      60,    6,    6,    6, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_jView[] = {
    "jView\0\0contextMenuRequested(QPoint)\0"
    "viewportChanged(QRectF)\0rebuild()\0"
};

const QMetaObject jView::staticMetaObject = {
    { &JUNGLE_WIDGET_CLASS::staticMetaObject, qt_meta_stringdata_jView,
      qt_meta_data_jView, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &jView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *jView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *jView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_jView))
        return static_cast<void*>(const_cast< jView*>(this));
    return JUNGLE_WIDGET_CLASS::qt_metacast(_clname);
}

int jView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = JUNGLE_WIDGET_CLASS::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: contextMenuRequested((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 1: viewportChanged((*reinterpret_cast< QRectF(*)>(_a[1]))); break;
        case 2: rebuild(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void jView::contextMenuRequested(QPoint _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void jView::viewportChanged(QRectF _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
static const uint qt_meta_data_jPreview[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_jPreview[] = {
    "jPreview\0\0rebuild()\0"
};

const QMetaObject jPreview::staticMetaObject = {
    { &JUNGLE_WIDGET_CLASS::staticMetaObject, qt_meta_stringdata_jPreview,
      qt_meta_data_jPreview, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &jPreview::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *jPreview::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *jPreview::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_jPreview))
        return static_cast<void*>(const_cast< jPreview*>(this));
    return JUNGLE_WIDGET_CLASS::qt_metacast(_clname);
}

int jPreview::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = JUNGLE_WIDGET_CLASS::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: rebuild(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_jLazyRenderer[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      32,   14,   14,   14, 0x0a,
      40,   14,   14,   14, 0x0a,
      50,   14,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_jLazyRenderer[] = {
    "jLazyRenderer\0\0accepted(QImage)\0flush()\0"
    "rebuild()\0onAccepted(QImage)\0"
};

const QMetaObject jLazyRenderer::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_jLazyRenderer,
      qt_meta_data_jLazyRenderer, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &jLazyRenderer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *jLazyRenderer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *jLazyRenderer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_jLazyRenderer))
        return static_cast<void*>(const_cast< jLazyRenderer*>(this));
    if (!strcmp(_clname, "QRunnable"))
        return static_cast< QRunnable*>(const_cast< jLazyRenderer*>(this));
    return QObject::qt_metacast(_clname);
}

int jLazyRenderer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: accepted((*reinterpret_cast< QImage(*)>(_a[1]))); break;
        case 1: flush(); break;
        case 2: rebuild(); break;
        case 3: onAccepted((*reinterpret_cast< const QImage(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void jLazyRenderer::accepted(QImage _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_jSync[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
       7,    6,    6,    6, 0x0a,
      17,    6,    6,    6, 0x08,
      36,    6,    6,    6, 0x08,
      56,    6,    6,    6, 0x08,
      81,    6,    6,    6, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_jSync[] = {
    "jSync\0\0rebuild()\0onZoomedIn(QRectF)\0"
    "onZoomedOut(QRectF)\0onZoomedFullView(QRectF)\0"
    "onPanned(QRectF)\0"
};

const QMetaObject jSync::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_jSync,
      qt_meta_data_jSync, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &jSync::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *jSync::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *jSync::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_jSync))
        return static_cast<void*>(const_cast< jSync*>(this));
    return QObject::qt_metacast(_clname);
}

int jSync::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: rebuild(); break;
        case 1: onZoomedIn((*reinterpret_cast< const QRectF(*)>(_a[1]))); break;
        case 2: onZoomedOut((*reinterpret_cast< const QRectF(*)>(_a[1]))); break;
        case 3: onZoomedFullView((*reinterpret_cast< const QRectF(*)>(_a[1]))); break;
        case 4: onPanned((*reinterpret_cast< const QRectF(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}
static const uint qt_meta_data_jLegend[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
       9,    8,    8,    8, 0x0a,
      19,    8,    8,    8, 0x08,
      41,    8,    8,    8, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_jLegend[] = {
    "jLegend\0\0rebuild()\0onVisibilityChanged()\0"
    "onZOrderChanged(int)\0"
};

const QMetaObject jLegend::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_jLegend,
      qt_meta_data_jLegend, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &jLegend::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *jLegend::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *jLegend::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_jLegend))
        return static_cast<void*>(const_cast< jLegend*>(this));
    return QWidget::qt_metacast(_clname);
}

int jLegend::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: rebuild(); break;
        case 1: onVisibilityChanged(); break;
        case 2: onZOrderChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
