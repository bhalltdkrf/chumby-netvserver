#ifndef DBUSMONITOR_H
#define DBUSMONITOR_H

#include "nm_interface.h"
#include "nm_ap_interface.h"

class DBusMonitor : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(DBusMonitor)
public:

    /** Constructor */
    DBusMonitor(QObject* parent = 0);

    /** Destructor */
    virtual ~DBusMonitor();

private:

    org::freedesktop::NetworkManagerInterface *nm_interface;
    org::freedesktop::NetworkManagerAPInterface *nm_ap_interface;

signals:

    void signal_StateChanged(uint state);
    void signal_PropertiesChanged(QByteArray prop_name, QByteArray prop_value);
    void signal_DeviceAdded(QByteArray objPath);
    void signal_DeviceRemoved(QByteArray objPath);

private slots:

    void StateChanged(uint state);
    void PropertiesChanged(QVariantMap changed);
    void DeviceAdded(QDBusObjectPath objPath);
    void DeviceRemoved(QDBusObjectPath objPath);
};

#endif // DBUSMONITOR_H
