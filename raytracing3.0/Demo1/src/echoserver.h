#ifndef ECHOSERVER_H
#define ECHOSERVER_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QByteArray>
#include <QtCore/QVariant>
#include <QMap>
#include <QList>
#include <QPointF>
#include <QVariant>
#include <QJsonObject>

#include "RayTracingCUDA.h"

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)

class EchoServer : public QObject
{
    Q_OBJECT
public:
	RayTracingCUDA *rayTracingCUDA=new RayTracingCUDA();
    QMap<QString,QVariant> mapMap;
    double xmax, xmin, ymax, ymin;
    double factor;
    //Scene* scene;
    QJsonObject sceneDate;
// {"0":[{}]}
    explicit EchoServer(quint16 port, bool debug = false, QObject *parent = nullptr);
    ~EchoServer();

Q_SIGNALS:
    void closed();

private Q_SLOTS:
    void onNewConnection();
    void processTextMessage(QString message);
    void socketDisconnected();
//    void updateBuilding();
//    void updateRoad();
    void updateScene(QJsonObject jsonObject);
    void updateVehicle(QJsonObject jsonObject);
    QString VPL();
	QString generateMap();
	QString runGenerateMap();
	QString updateMapData(QJsonObject jsonObject);
	void sendLogMessage(QString sendInfo);
private:
    QWebSocketServer *m_pWebSocketServer;
    QList<QWebSocket *> m_clients;
    bool m_debug;
    //Node* receivedTx;
    //Node* receivedRx;
    //vector<Object*> carList;
    bool vehicleAdded = false;
};

#endif //ECHOSERVER_H
