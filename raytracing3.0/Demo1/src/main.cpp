/**
  @file
  @author Stefan Frings
*/

#include <QCoreApplication>
#include <QDir>
#include "httplistener.h"
#include "templatecache.h"
#include "httpsessionstore.h"
#include "staticfilecontroller.h"
#include "filelogger.h"
#include "requestmapper.h"

#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtWebEngine/qtwebengineglobal.h>

#include "echoserver.h"


#include "MyCUDARayTracing.h"

//定义是否显示界面，如果没有显示界面则需要通过浏览器访问
#define showTheMainWindows false

using namespace stefanfrings;

/** Cache for template files */
TemplateCache* templateCache;

/** Storage for session cookies */
HttpSessionStore* sessionStore;

/** Controller for static files */
StaticFileController* staticFileController;

/** Redirects log messages to a file */
FileLogger* logger;


/** Search the configuration file */
QString searchConfigFile()
{
	QString binDir = QCoreApplication::applicationDirPath();
	QString appName = QCoreApplication::applicationName();
	QString fileName(appName + ".ini");

	QStringList searchList;
	searchList.append(binDir);
	searchList.append(binDir + "/etc");
	searchList.append(binDir + "/../etc");
	searchList.append(binDir + "/../../etc"); // for development without shadow build
	searchList.append(binDir + "/../" + appName + "/etc"); // for development with shadow build
	searchList.append(binDir + "/../../" + appName + "/etc"); // for development with shadow build
	searchList.append(binDir + "/../../../" + appName + "/etc"); // for development with shadow build
	searchList.append(binDir + "/../../../../" + appName + "/etc"); // for development with shadow build
	searchList.append(binDir + "/../../../../../" + appName + "/etc"); // for development with shadow build
	searchList.append(QDir::rootPath() + "etc/opt");
	searchList.append(QDir::rootPath() + "etc");

	foreach(QString dir, searchList)
	{
		QFile file(dir + "/" + fileName);
		if (file.exists())
		{
			// found
			fileName = QDir(file.fileName()).canonicalPath();
			qDebug("Using config file %s", qPrintable(fileName));
			return fileName;
		}
	}

	// not found
	foreach(QString dir, searchList)
	{
		qWarning("%s/%s not found", qPrintable(dir), qPrintable(fileName));
	}
	qFatal("Cannot find config file %s", qPrintable(fileName));
	return 0;
}


/**
  Entry point of the program.
*/
int main(int argc, char *argv[])
{
	//QCoreApplication::addLibraryPath("F:\\Qt\\5.11.2\\msvc2017_64\\plugins\\platforms");

	/*vector<vector<vector<double>>> grid_input;

	vector<vector<double>> grid0;

	vector<double> edge0;
	edge0.push_back(1);
	edge0.push_back(0.5);
	edge0.push_back(2);
	edge0.push_back(0.5);
	edge0.push_back(0);
	edge0.push_back(1);
	grid0.push_back(edge0);
	vector<double> edge1;
	edge1.push_back(1);
	edge1.push_back(1.5);
	edge1.push_back(2);
	edge1.push_back(1.5);
	edge1.push_back(0);
	edge1.push_back(-1);
	grid0.push_back(edge1);

	vector<double> edge2;
	edge2.push_back(1);
	edge2.push_back(0.5);
	edge2.push_back(1);
	edge2.push_back(1.5);
	edge2.push_back(1);
	edge2.push_back(0);
	grid0.push_back(edge2);
	grid_input.push_back(grid0);

	vector<vector<double>> grid1;
	vector<double> edge3;
	edge3.push_back(2);
	edge3.push_back(0.5);
	edge3.push_back(3);
	edge3.push_back(0.5);
	edge3.push_back(0);
	edge3.push_back(1);
	grid1.push_back(edge3);
	vector<double> edge4;
	edge4.push_back(2);
	edge4.push_back(1.5);
	edge4.push_back(3);
	edge4.push_back(1.5);
	edge4.push_back(0);
	edge4.push_back(-1);
	grid1.push_back(edge4);

	vector<double> edge5;
	edge5.push_back(3);
	edge5.push_back(0.5);
	edge5.push_back(3);
	edge5.push_back(1.5);
	edge5.push_back(-1);
	edge5.push_back(0);
	grid1.push_back(edge5);
	grid_input.push_back(grid1);

	vector<vector<double>> grid2;
	grid_input.push_back(grid2);

	vector<vector<double>> grid3;
	vector<double> edge6;
	edge6.push_back(6.5);
	edge6.push_back(0.5);
	edge6.push_back(6.5);
	edge6.push_back(1.5);
	edge6.push_back(1);
	edge6.push_back(0);
	grid3.push_back(edge6);
	vector<double> edge7;
	edge7.push_back(7.5);
	edge7.push_back(0.5);
	edge7.push_back(6.5);
	edge7.push_back(0.5);
	edge7.push_back(0);
	edge7.push_back(1);
	grid3.push_back(edge7);

	vector<double> edge8;
	edge8.push_back(7.5);
	edge8.push_back(0.5);
	edge8.push_back(7.5);
	edge8.push_back(1.5);
	edge8.push_back(-1);
	edge8.push_back(0);
	grid3.push_back(edge8);
	vector<double> edge9;
	edge9.push_back(7.5);
	edge9.push_back(1.5);
	edge9.push_back(6.5);
	edge9.push_back(1.5);
	edge9.push_back(0);
	edge9.push_back(-1);
	grid3.push_back(edge9);
	grid_input.push_back(grid3);

	vector<vector<double>> getResult = initCUDAInput(grid_input, 1, 3, 3, 2, 180);*/
	//for (int i = 0; i < getResult.size(); i++) {
	//	for (int j = 0; j < getResult[i].size(); j += 2) {
	//		qDebug() << i << ":[" << getResult[i][j] << "," << getResult[i][j + 1] << "]";
	//	}
	//}
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	QGuiApplication app(argc, argv);

	app.setApplicationName("Demo1");
	app.setOrganizationName("Butterfly");

	// Find the configuration file
	QString configFileName = searchConfigFile();

	// Configure logging into a file
	/*
	QSettings* logSettings=new QSettings(configFileName,QSettings::IniFormat,&app);
	logSettings->beginGroup("logging");
	FileLogger* logger=new FileLogger(logSettings,10000,&app);
	logger->installMsgHandler();
	*/

	// Configure template loader and cache
	QSettings* templateSettings = new QSettings(configFileName, QSettings::IniFormat, &app);
	templateSettings->beginGroup("templates");
	templateCache = new TemplateCache(templateSettings, &app);

	// Configure session store
	QSettings* sessionSettings = new QSettings(configFileName, QSettings::IniFormat, &app);
	sessionSettings->beginGroup("sessions");
	sessionStore = new HttpSessionStore(sessionSettings, &app);

	// Configure static file controller
	QSettings* fileSettings = new QSettings(configFileName, QSettings::IniFormat, &app);
	fileSettings->beginGroup("docroot");
	staticFileController = new StaticFileController(fileSettings, &app);

	// Configure and start the TCP listener
	QSettings* listenerSettings = new QSettings(configFileName, QSettings::IniFormat, &app);
	listenerSettings->beginGroup("listener");
	new HttpListener(listenerSettings, new RequestMapper(&app), &app);

	qWarning("Application has started");

	if (showTheMainWindows) {
		QtWebEngine::initialize();
		QQmlApplicationEngine engine;
		engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
	}


	EchoServer *server = new EchoServer(1234, true);

	QObject::connect(server, &EchoServer::closed, &app, &QGuiApplication::quit);
	app.exec();

	qWarning("Application has stopped");
}
