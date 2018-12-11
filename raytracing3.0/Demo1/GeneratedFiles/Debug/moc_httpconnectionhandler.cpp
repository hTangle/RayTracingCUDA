/****************************************************************************
** Meta object code from reading C++ file 'httpconnectionhandler.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../QtWebApp/httpserver/httpconnectionhandler.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'httpconnectionhandler.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_stefanfrings__HttpConnectionHandler_t {
    QByteArrayData data[8];
    char stringdata0[119];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_stefanfrings__HttpConnectionHandler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_stefanfrings__HttpConnectionHandler_t qt_meta_stringdata_stefanfrings__HttpConnectionHandler = {
    {
QT_MOC_LITERAL(0, 0, 35), // "stefanfrings::HttpConnectionH..."
QT_MOC_LITERAL(1, 36, 16), // "handleConnection"
QT_MOC_LITERAL(2, 53, 0), // ""
QT_MOC_LITERAL(3, 54, 17), // "tSocketDescriptor"
QT_MOC_LITERAL(4, 72, 16), // "socketDescriptor"
QT_MOC_LITERAL(5, 89, 11), // "readTimeout"
QT_MOC_LITERAL(6, 101, 4), // "read"
QT_MOC_LITERAL(7, 106, 12) // "disconnected"

    },
    "stefanfrings::HttpConnectionHandler\0"
    "handleConnection\0\0tSocketDescriptor\0"
    "socketDescriptor\0readTimeout\0read\0"
    "disconnected"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_stefanfrings__HttpConnectionHandler[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x0a /* Public */,
       5,    0,   37,    2, 0x08 /* Private */,
       6,    0,   38,    2, 0x08 /* Private */,
       7,    0,   39,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void stefanfrings::HttpConnectionHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        HttpConnectionHandler *_t = static_cast<HttpConnectionHandler *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->handleConnection((*reinterpret_cast< tSocketDescriptor(*)>(_a[1]))); break;
        case 1: _t->readTimeout(); break;
        case 2: _t->read(); break;
        case 3: _t->disconnected(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject stefanfrings::HttpConnectionHandler::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_stefanfrings__HttpConnectionHandler.data,
      qt_meta_data_stefanfrings__HttpConnectionHandler,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *stefanfrings::HttpConnectionHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *stefanfrings::HttpConnectionHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_stefanfrings__HttpConnectionHandler.stringdata0))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int stefanfrings::HttpConnectionHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
