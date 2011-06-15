
#ifndef SocketResponse_H
#define SocketResponse_H

#include <QMap>
#include <QString>
#include <QAbstractSocket>

class SocketResponse
{
    Q_DISABLE_COPY(SocketResponse)
public:

    /**
      Constructor.
      @param socket used to write the response
    */
    SocketResponse(QAbstractSocket* socket);
    ~SocketResponse();

    void setStatus(QByteArray statusName);
    void setStatus(int statusCode);

    void setParameter(QByteArray paramName, int paramValue);
    void setParameter(QByteArray paramName, float paramValue);
    void setParameter(QByteArray paramName, QByteArray paramValue);
    QMap<QByteArray,QByteArray>& getParameters();
    int getParametersCount();

    void write();

private:

    /** Socket for writing output */
    QAbstractSocket* socket;

    QByteArray statusText;

    QMap<QByteArray,QByteArray> parameters;

    void writeToSocket(QByteArray data);
};

#endif // SocketResponse_H
