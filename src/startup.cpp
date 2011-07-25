#include <QScreen>
#include <QColor>
#include <QDir>
#include "startup.h"
#include "static.h"
#include "httplistener.h"
#include "flashpolicyserver.h"
#include "tcpsocketserver.h"
#include "udpsocketserver.h"
#include "requestmapper.h"

#define UNIMPLEMENTED       "Un1mPl3m3nT3D"

Startup::Startup(QObject* parent) : QObject(parent)
{
    start();
}

void Startup::start()
{
    // Initialize the core application
    QString configFileName=Static::getConfigDir()+"/"+APPNAME+".ini";

    // Configure session store
    QSettings* sessionSettings=new QSettings(configFileName,QSettings::IniFormat);
    sessionSettings->beginGroup("sessions");
    Static::sessionStore=new HttpSessionStore(sessionSettings, this);

    // Configure static file controller
    QSettings* fileSettings=new QSettings(configFileName,QSettings::IniFormat);
    fileSettings->beginGroup("docroot");
    Static::staticFileController=new StaticFileController(fileSettings);

    // Configure script controller
    Static::scriptController=new ScriptController(fileSettings);

    // Configure cursor controller
#ifdef CURSOR_CONTROLLER
    Static::cursorController=new CursorController(fileSettings);
#endif

    // Configure framebuffer controller
    QSettings* fbSettings=new QSettings(configFileName,QSettings::IniFormat);
    fbSettings->beginGroup("framebuffer-controller");
    Static::framebufferController=new FramebufferController(fbSettings);

    // Configure bridge controller
    Static::bridgeController=new BridgeController(fileSettings,this);

    // Configure Flash policy server
    QSettings* flashpolicySettings=new QSettings(configFileName,QSettings::IniFormat);
    flashpolicySettings->beginGroup("flash-policy-server");
    new FlashPolicyServer(flashpolicySettings, this);

    RequestMapper *requestMapper = new RequestMapper();

    // Configure and start the TCP listener
    QSettings* listenerSettings=new QSettings(configFileName,QSettings::IniFormat);
    listenerSettings->beginGroup("listener");
    new HttpListener(listenerSettings, requestMapper, this);

    // Configure TCP socket server
    QSettings* tcpServerSettings=new QSettings(configFileName,QSettings::IniFormat);
    tcpServerSettings->beginGroup("tcp-socket-server");
    Static::tcpSocketServer=new TcpSocketServer(tcpServerSettings, requestMapper, this);

    // Configure UDP socket server
    QSettings* udpServerSettings=new QSettings(configFileName,QSettings::IniFormat);
    udpServerSettings->beginGroup("udp-socket-server");
    Static::udpSocketServer=new UdpSocketServer(udpServerSettings, requestMapper, this);

    // DBus monitor
#ifdef ENABLE_DBUS_STUFF
    DBusMonitor *dbusMonitor = new DBusMonitor(this);
    Static::dbusMonitor = dbusMonitor;
    QObject::connect(dbusMonitor, SIGNAL(signal_StateChanged(uint)), Static::bridgeController, SLOT(slot_StateChanged(uint)));
    QObject::connect(dbusMonitor, SIGNAL(signal_PropertiesChanged(QByteArray,QByteArray)), Static::bridgeController, SLOT(slot_PropertiesChanged(QByteArray,QByteArray)));
    QObject::connect(dbusMonitor, SIGNAL(signal_DeviceAdded(QByteArray)), Static::bridgeController, SLOT(slot_DeviceAdded(QByteArray)));
    QObject::connect(dbusMonitor, SIGNAL(signal_DeviceRemoved(QByteArray)), Static::bridgeController, SLOT(slot_DeviceRemoved(QByteArray)));
#endif

    // Input listener
    InputListener *inputListener = new InputListener(this, "/dev/input/event1");
    Static::inputListener = inputListener;
    QObject::connect(inputListener, SIGNAL(signal_keyInput(Qt::Key,bool,bool)), Static::bridgeController, SLOT(slot_keyInput(Qt::Key,bool,bool)));

    printf("NeTVServer has started");
}

void Startup::receiveArgs(const QString &argsString)
{
    QStringList argsList = argsString.split(ARGS_SPLIT_TOKEN);
    int argCount = argsList.count();
    if (argCount < 2)
        return;

    QString execPath = argsList[0];
    QString command = argsList[1];
    argsList.removeFirst();
    argsList.removeFirst();
    argCount = argsList.count();

    printf("Received argument: %s", command.toLatin1().constData());

    QByteArray string = processStatelessCommand(command.toLatin1(), argsList);
    if (string != UNIMPLEMENTED)            printf("NeTVServer: %s", string.constData());
    else                                    printf("NeTVServer: Invalid argument");
}

void Startup::windowEvent ( QWSWindow * window, QWSServer::WindowEvent eventType )
{
    Q_UNUSED(window);
    Q_UNUSED(eventType);

#ifdef ENABLE_QWS_STUFF
    QWSServer *qserver = QWSServer::instance();
    const QList<QWSWindow *> winList = qserver->clientWindows();
    if (winList.count() > 0)
    {
        qDebug("NeTVServer: painting is enabled [%d windows]", winList.count());
        qserver->enablePainting(true);
    }
    else
    {
        qDebug("NeTVServer: painting is disabled [%d windows]", winList.count());
        qserver->enablePainting(false);
    }
#endif
}

QByteArray Startup::processStatelessCommand(QByteArray command, QStringList argsList)
{
    //command name
    if (command.length() < 0)
        return UNIMPLEMENTED;
    command = command.toUpper();

    //arguments
    int argCount = argsList.count();

    if (command == "HELLO")
    {
        //Just a dummy command
        qDebug("argCount %d", argCount);
    }

    //----------------------------------------------------

#ifdef ENABLE_QWS_STUFF
    else if (command == "REFRESH")
    {
        //redraw the entire screen
        QWSServer::instance()->refresh();
    }

    else if (command == "SETRESOLUTION" && argCount == 1)
    {
        //comma-separated arguments
        QString args = argsList[0];
        QStringList argsLs = args.split(",");
        if (argsLs.count() < 3)
            return UNIMPLEMENTED;

        int w = argsLs[0].toInt();
        int h = argsLs[1].toInt();
        int depth = argsLs[2].toInt();

        //resize the screen driver
        QWSServer *qserver = QWSServer::instance();
        qserver->enablePainting(false);
        QScreen::instance()->setMode(w,h,depth);
        qserver->enablePainting(true);

        //forward to browser
        if (Static::tcpSocketServer != NULL)
            Static::tcpSocketServer->broadcast("<xml><cmd>Show</cmd></xml>", "netvbrowser");

        //redraw the entire screen
        qserver->refresh();

        return QString("%1 %2 %3 %4").arg(command.constData()).arg(w).arg(h).arg(depth).toLatin1();
    }

    else if (command == "SETRESOLUTION" && argCount >= 3)
    {
        //space-separated arguments
        int w = argsList[0].toInt();
        int h = argsList[1].toInt();
        int depth = argsList[2].toInt();

        //resize the screen driver
        QWSServer *qserver = QWSServer::instance();
        qserver->enablePainting(false);
        QScreen::instance()->setMode(w,h,depth);
        qserver->enablePainting(true);

        //forward to browser
        if (Static::tcpSocketServer != NULL)
            Static::tcpSocketServer->broadcast("<xml><cmd>Show</cmd></xml>", "netvbrowser");

        //redraw the entire screen
        qserver->refresh();

        return QString("%1 %2 %3 %4").arg(command.constData()).arg(w).arg(h).arg(depth).toLatin1();
    }
#endif

    return UNIMPLEMENTED;
}
