/****************************************************************************
** Meta object code from reading C++ file 'echoserver.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/echoserver.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'echoserver.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_EchoServer_t {
    QByteArrayData data[16];
    char stringdata0[187];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_EchoServer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_EchoServer_t qt_meta_stringdata_EchoServer = {
    {
QT_MOC_LITERAL(0, 0, 10), // "EchoServer"
QT_MOC_LITERAL(1, 11, 6), // "closed"
QT_MOC_LITERAL(2, 18, 0), // ""
QT_MOC_LITERAL(3, 19, 15), // "onNewConnection"
QT_MOC_LITERAL(4, 35, 18), // "processTextMessage"
QT_MOC_LITERAL(5, 54, 7), // "message"
QT_MOC_LITERAL(6, 62, 18), // "socketDisconnected"
QT_MOC_LITERAL(7, 81, 11), // "updateScene"
QT_MOC_LITERAL(8, 93, 10), // "jsonObject"
QT_MOC_LITERAL(9, 104, 13), // "updateVehicle"
QT_MOC_LITERAL(10, 118, 3), // "VPL"
QT_MOC_LITERAL(11, 122, 11), // "generateMap"
QT_MOC_LITERAL(12, 134, 14), // "runGenerateMap"
QT_MOC_LITERAL(13, 149, 13), // "updateMapData"
QT_MOC_LITERAL(14, 163, 14), // "sendLogMessage"
QT_MOC_LITERAL(15, 178, 8) // "sendInfo"

    },
    "EchoServer\0closed\0\0onNewConnection\0"
    "processTextMessage\0message\0"
    "socketDisconnected\0updateScene\0"
    "jsonObject\0updateVehicle\0VPL\0generateMap\0"
    "runGenerateMap\0updateMapData\0"
    "sendLogMessage\0sendInfo"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_EchoServer[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   69,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   70,    2, 0x08 /* Private */,
       4,    1,   71,    2, 0x08 /* Private */,
       6,    0,   74,    2, 0x08 /* Private */,
       7,    1,   75,    2, 0x08 /* Private */,
       9,    1,   78,    2, 0x08 /* Private */,
      10,    0,   81,    2, 0x08 /* Private */,
      11,    0,   82,    2, 0x08 /* Private */,
      12,    0,   83,    2, 0x08 /* Private */,
      13,    1,   84,    2, 0x08 /* Private */,
      14,    1,   87,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QJsonObject,    8,
    QMetaType::Void, QMetaType::QJsonObject,    8,
    QMetaType::QString,
    QMetaType::QString,
    QMetaType::QString,
    QMetaType::QString, QMetaType::QJsonObject,    8,
    QMetaType::Void, QMetaType::QString,   15,

       0        // eod
};

void EchoServer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        EchoServer *_t = static_cast<EchoServer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->closed(); break;
        case 1: _t->onNewConnection(); break;
        case 2: _t->processTextMessage((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->socketDisconnected(); break;
        case 4: _t->updateScene((*reinterpret_cast< QJsonObject(*)>(_a[1]))); break;
        case 5: _t->updateVehicle((*reinterpret_cast< QJsonObject(*)>(_a[1]))); break;
        case 6: { QString _r = _t->VPL();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 7: { QString _r = _t->generateMap();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 8: { QString _r = _t->runGenerateMap();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 9: { QString _r = _t->updateMapData((*reinterpret_cast< QJsonObject(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 10: _t->sendLogMessage((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (EchoServer::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&EchoServer::closed)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject EchoServer::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_EchoServer.data,
      qt_meta_data_EchoServer,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *EchoServer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *EchoServer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_EchoServer.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int EchoServer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void EchoServer::closed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
