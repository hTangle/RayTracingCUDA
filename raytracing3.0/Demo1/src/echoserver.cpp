#include "echoserver.h"
#include "QtWebSockets/qwebsocketserver.h"
#include "QtWebSockets/qwebsocket.h"
#include <QtCore/QDebug>
#include <QJsonObject>
#include <QJsonDocument>
//#include "mesh.h"
#include "echoserver.h"
//#include "tracer.h"
#include "QJsonArray"
QT_USE_NAMESPACE

//! [constructor]
EchoServer::EchoServer(quint16 port, bool debug, QObject *parent) :
	QObject(parent),
	m_pWebSocketServer(new QWebSocketServer(QStringLiteral("Echo Server"),
		QWebSocketServer::NonSecureMode, this)),
	m_debug(debug)
{
	if (m_pWebSocketServer->listen(QHostAddress::Any, port)) {
		if (m_debug)
			qDebug() << "Echoserver listening on port" << port;
		connect(m_pWebSocketServer, &QWebSocketServer::newConnection,
			this, &EchoServer::onNewConnection);
		connect(m_pWebSocketServer, &QWebSocketServer::closed, this, &EchoServer::closed);
	}
}
//! [constructor]

EchoServer::~EchoServer()
{
	m_pWebSocketServer->close();
	qDeleteAll(m_clients.begin(), m_clients.end());
}

//! [onNewConnection]
void EchoServer::onNewConnection()
{
	QWebSocket *pSocket = m_pWebSocketServer->nextPendingConnection();

	connect(pSocket, &QWebSocket::textMessageReceived, this, &EchoServer::processTextMessage);
	connect(pSocket, &QWebSocket::disconnected, this, &EchoServer::socketDisconnected);
	qDebug() << "socket connected:";
	m_clients << pSocket;
}
//! [onNewConnection]

//! [processTextMessage]
void EchoServer::processTextMessage(QString message)
{
	QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
	if (m_debug)
		//        qDebug() << "Message received:" << message;
		if (pClient) {
			if (message == "1") {
				//            updateBuilding();
				//            QJsonObject qjss=QJsonObject::fromVariantMap(mapMap);
				//            QJsonDocument doc(qjss);
				//            QString strJson(doc.toJson(QJsonDocument::Compact));
				//            pClient->sendTextMessage(strJson);

							//QString bound = '';
			}
			else if (message == "2") {
				//          射线追踪
				//            QString data = rayTracing();
				QString data = VPL();
				pClient->sendTextMessage(data);
			}
			else if (message == '3') {
				//          更新道路
				//            updateRoad();
				//            QJsonObject qjss=QJsonObject::fromVariantMap(mapMap);
				//            QJsonDocument doc(qjss);
				//            QString strJson(doc.toJson(QJsonDocument::Compact));
				//            pClient->sendTextMessage(strJson);
			}
			else if (message == "cordinate") {
				//            QMap<QString, QVariant> map;
				//            map.insert("xmax",xmax);
				//            map.insert("xmin",xmin);
				//            map.insert("ymax",ymax);
				//            map.insert("ymin",ymin);
				//            QJsonObject qjss = QJsonObject::fromVariantMap(map);
				//            QJsonDocument doc(qjss);
				//            QString strJson(doc.toJson(QJsonDocument::Compact));
				//            pClient->sendTextMessage(strJson);
			}
			else {//
				QJsonDocument jsonDocument = QJsonDocument::fromJson(message.toLocal8Bit().data());
				if (jsonDocument.isNull())
				{
					qDebug() << "String NULL" << message.toLocal8Bit().data();
				}
				QJsonObject jsonObject = jsonDocument.object();
				QString type = jsonObject["type"].toString();
				if (type == "vehicle") {
					qDebug() << jsonObject;
					updateVehicle(jsonObject);
				}
				else if (type == "senario") {
					sceneDate = jsonObject;
					updateScene(sceneDate);
				}
			}
		}
}
//! [processTextMessage]


//! [socketDisconnected]
void EchoServer::socketDisconnected()
{
	QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
	if (m_debug)
		qDebug() << "socketDisconnected:" << pClient;
	if (pClient) {
		m_clients.removeAll(pClient);
		pClient->deleteLater();
	}
}
//! [socketDisconnected]

QString EchoServer::VPL() {
	
	return rayTracingCUDA->beginCUDA();
	/*QJsonObject json;
	json.insert("type", QString("output"));
	QJsonArray paths;
	QJsonObject pointTemp;
	pointTemp.insert("x", rayTracingCUDA->Tx_x);
	pointTemp.insert("y", rayTracingCUDA->Tx_y);
	pointTemp.insert("z", 0);

	for (int i = 0; i < getResult.size(); i++) {
		QJsonObject path;
		path.insert("pathloss", -117.8);
		QJsonArray nodeList;
		nodeList.push_back(pointTemp);
		for (int j = 0; j < getResult[i].size(); j += 2) {
			QJsonObject point;
			point.insert("x", getResult[i][j]);
			point.insert("y", getResult[i][j + 1]);
			point.insert("z", 0);
			qDebug() << getResult[i][j] << "," << getResult[i][j + 1];
			nodeList.push_back(point);
		}
		path.insert("nodeList", nodeList);
		paths.push_back(path);
	}
	json.insert("paths", paths);
	QJsonDocument doc(json);
	QString strJson(doc.toJson(QJsonDocument::Compact));
	return strJson;*/
	//return "";
	//updateScene(sceneDate);
	//qDebug() << "scene size : " << scene->objList.size();
	//Node* rx = new Node(receivedRx->x, receivedRx->y, 0.0, true);
	//Mesh* mesh = new Mesh(30, scene, rx);
	//Node* source = new Node(receivedTx->x * mesh->size, receivedTx->y * mesh->size, 0);
	//source->type = NodeType::Tx;
	////    射线追踪器
	//Tracer*  tracer = new Tracer(mesh, source, carList);
	//tracer->verticalPlane(source);
	//if (vehicleAdded) {
	//	carList.pop_back();
	//	vehicleAdded = false;
	//}
	//qDebug() << tracer->allPath.size() << " paths has been accepted";
	////    QMap<QString,QVariant> data;
	//QJsonObject json;
	//json.insert("type", QString("output"));
	//QJsonArray paths;
	//for (int i = 0; i < tracer->allPath.size(); i++) {
	//	QJsonObject path;
	//	Path* p = tracer->allPath[i];
	//	path.insert("pathloss", p->channelGain(0));
	//	QJsonArray nodeList;
	//	for (int j = 0; j < p->nodeSet.size(); j++) {
	//		Node* node = p->nodeSet[j];
	//		QJsonObject point;
	//		point.insert("x", node->x);
	//		point.insert("y", node->y);
	//		point.insert("z", node->z);
	//		nodeList.insert(j, point);
	//	}
	//	path.insert("nodeList", nodeList);
	//	paths.insert(i, path);
	//}
	//json.insert("paths", paths);
	//QJsonDocument doc(json);
	//QString strJson(doc.toJson(QJsonDocument::Compact));
	//return strJson;
}

void EchoServer::updateScene(QJsonObject jsonObject) {
	rayTracingCUDA->clear();//需要更新地图数据时，先进行清空处理
	QJsonArray features = jsonObject["features"].toArray();
	QJsonArray bbox = jsonObject["bbox"].toArray();
	xmin = bbox[0].toDouble();
	ymin = bbox[1].toDouble();
	xmax = bbox[2].toDouble();
	ymax = bbox[3].toDouble();
	factor = (xmax - xmin) > (ymax - ymin) ? (xmax - xmin) : (ymax - ymin);
	qDebug() << xmin << ymin << xmax << ymax;
	for (QJsonValue feature : features) {
		//Object* obj = new Object();
		QJsonArray coordinates = feature["coordinates"].toArray();
		double z = feature["z"].toString().toDouble();
		//obj->z = z;
		//qDebug() << z << "";
		//按照顺时针方向排列的
		//double preX=
		double preX = (coordinates[0].toArray()[0].toDouble() - xmin) / factor * 100;
		double preY = (coordinates[0].toArray()[1].toDouble() - ymin) / factor * 100;
		for (int i = 1; i < coordinates.count(); i++) {
			double x = (coordinates[i].toArray()[0].toDouble() - xmin) / factor * 100;
			double y = (coordinates[i].toArray()[1].toDouble() - ymin) / factor * 100;
			rayTracingCUDA->addEdgeToGrids(preX, preY, x, y);
			preX = x;
			preY = y;
		}
	}
	rayTracingCUDA->printVector();
}


void EchoServer::updateVehicle(QJsonObject vehicle) {
	QString vehicleType = vehicle["vehicleType"].toString();
	bool dynamic = vehicle["dynamic"].toBool();
	int speed = vehicle["speed"].toString().toInt();
	qDebug() << "speed" << speed;
	if (vehicleType == "tx") {
		if (!dynamic) {
			QJsonArray location = vehicle["location1"].toArray();
			double longitude = location[0].toDouble();
			double latitude = location[1].toDouble();
			double x = (longitude - xmin) / (factor);
			double y = (latitude - ymin) / (factor);
			qDebug() << "x:" << x * 100 << "y:" << y * 100;
			rayTracingCUDA->Tx_x = x * 100;
			rayTracingCUDA->Tx_y = y * 100;
			//receivedTx = new Node(x, y);
		}
		else {
			QJsonArray location1 = vehicle["location1"].toArray();
			QJsonArray location2 = vehicle["location2"].toArray();

		}

	}
	else if (vehicleType == "rx") {
		if (!dynamic) {
			QJsonArray location = vehicle["location1"].toArray();
			double longitude = location[0].toDouble();
			double latitude = location[1].toDouble();
			double x = (longitude - xmin) / (factor);
			double y = (latitude - ymin) / (factor);
			qDebug() << "x:" << x * 100 << "y:" << y * 100;
			rayTracingCUDA->Rx_x = x * 100;
			rayTracingCUDA->Rx_y = y * 100;
			//receivedRx = new Node(x, y);
		}
		else {

		}
	}
	else {//障碍车
		/*if (vehicleAdded) {
			carList.clear();
			vehicleAdded = false;
		}
		qDebug() << vehicle;
		Object* obstaclCar = new Object();
		obstaclCar->type = "vehicle";
		double z = vehicle["height"].toDouble();
		QJsonArray bbox = vehicle["bbox"].toArray();
		for (QJsonValue coor : bbox) {
			QJsonObject point = coor.toObject();
			double lng = point["x"].toDouble();
			double lat = point["y"].toDouble();
			double x = (lng - xmin) / (factor);
			double y = (lat - ymin) / (factor);
			obstaclCar->pointList.push_back(new Point(x, y, z));
		}
		obstaclCar->setEdgeList(obstaclCar->pointList);
		carList.push_back(obstaclCar);
		vehicleAdded = true;
		qDebug() << carList.size();
		for (int i = 0; i < carList[0]->pointList.size(); i++) {
			qDebug() << carList[0]->pointList[i]->x * 30;
			qDebug() << carList[0]->pointList[i]->y * 30;
			qDebug() << carList[0]->pointList[i]->z;
		}
		qDebug() << carList[0]->pointList.size();
		qDebug() << carList[0]->edgeList.size();*/
	}

}



//void EchoServer::updateRoad(){
//    mapMap.clear();
//    FilePoint *filePoint = new FilePoint();
//    FileManager *fileManager = new FileManager("./R.shp","./R.dbf");
//    fileManager->readRoadDbf(filePoint);
//    fileManager->readRoadShp(filePoint);
//    qDebug() <<"xmax:"<<filePoint->Xmax;
//    qDebug() <<"xmin"<<filePoint->Xmin;
//    qDebug() <<"ymax"<<filePoint->Ymax;
//    qDebug() <<"ymin"<<filePoint->Ymin;
//    QMap<QString,QVariant> boundMap;
//    filePoint->uniformlize(80,0,xmax - xmin);//(0,1)
//    Scene *scene = new Scene(filePoint->allPointList, filePoint->index);
//    QJsonObject qjs;
//    for(int i = 0; i < scene->objList.size(); i++){
//        QMap<QString,QVariant> myMap;
//        QList<QVariant> posList;
//        for(int j=0;j<scene->objList[i]->pointList.size();j++){
//            QMap<QString,QVariant> tempMap;
//            tempMap.insert(QString("x"),QVariant((scene->objList[i]->pointList[j]->x )));
//            tempMap.insert(QString("y"),QVariant((scene->objList[i]->pointList[j]->y )));
//            tempMap.insert(QString("x"),QVariant((scene->objList[i]->pointList[j]->x - 0.5)*80));
//           tempMap.insert(QString("y"),QVariant((scene->objList[i]->pointList[j]->y - 0.5)*80));
//            tempMap.insert(QString("z"),QVariant(0) );
//            posList.append(QVariant(tempMap));
//        }

//        mapMap.insert(QString::number(i,10),QVariant(posList));
//    }
//}




//void EchoServer::updateBuilding(){
//    mapMap.clear();
//    FilePoint *filePoint = new FilePoint();
//    FileManager *fileManager = new FileManager("E:/QT5_11_1/raytracing1.1/rayTracing1.1/RayTracing/BUILDING_nanjing.shp",
//                                               "E:/QT5_11_1/raytracing1.1/rayTracing1.1/RayTracing/BUILDING_nanjing.dbf");
//    fileManager->readDbfFile(filePoint);
//    fileManager->readShpFile(filePoint);
//    qDebug() <<"xmax:"<<filePoint->Xmax;
//    qDebug() <<"xmin"<<filePoint->Xmin;
//    qDebug() <<"ymax"<<filePoint->Ymax;
//    qDebug() <<"ymin"<<filePoint->Ymin;
//    QMap<QString,QVariant> boundMap;
//    xmax =filePoint->Xmax;
//    xmin =filePoint->Xmin;
//    ymax =filePoint->Ymax;
//    ymin =filePoint->Ymin;
//    filePoint->uniformlize(80,50,xmax - xmin);//(0,1)
//    Scene *scene = new Scene(filePoint->allPointList, filePoint->index);
//    QJsonObject qjs;
//    for(int i = 0; i < scene->objList.size(); i++){
//        QMap<QString,QVariant> myMap;
//        QList<QVariant> posList;
//        for(int j=0;j<scene->objList[i]->pointList.size();j++){
//            QMap<QString,QVariant> tempMap;
//            tempMap.insert(QString("x"),QVariant((scene->objList[i]->pointList[j]->x )));
//            tempMap.insert(QString("y"),QVariant((scene->objList[i]->pointList[j]->y)));
//            tempMap.insert(QString("z"),QVariant(scene->objList[i]->pointList[j]->z ));
//           tempMap.insert(QString("x"),QVariant((scene->objList[i]->pointList[j]->x - 0.5)*80));
//           tempMap.insert(QString("y"),QVariant((scene->objList[i]->pointList[j]->y - 0.5)*80));
//          tempMap.insert(QString("z"),QVariant(scene->objList[i]->pointList[j]->z *10) );
//            posList.append(QVariant(tempMap));
//        }

//        mapMap.insert(QString::number(i,10),QVariant(posList));
//    }

//}
