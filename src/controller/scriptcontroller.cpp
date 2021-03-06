#include "scriptcontroller.h"
#include <QFileInfo>
#include <QDir>
#include <QProcess>
#include "bridgecontroller.h"

ScriptController::ScriptController(QSettings* settings) : HttpRequestHandler()
{
    encoding=settings->value("encoding","UTF-8").toString();
    docroot=settings->value("path",".").toString();
    // Convert relative path to absolute, based on the directory of the config file.
#ifdef Q_OS_WIN32
    if (QDir::isRelativePath(docroot) && settings->format()!=QSettings::NativeFormat)
#else
    if (QDir::isRelativePath(docroot))
#endif
    {
        QFileInfo configFile(settings->fileName());
        docroot=QFileInfo(configFile.absolutePath(),docroot).absoluteFilePath();
    }
    //qDebug("ScriptController: docroot=%s, encoding=%s",qPrintable(docroot),qPrintable(encoding));
}


void ScriptController::service(HttpRequest& request, HttpResponse& response)
{
    //Path always starts with "/scripts"
    QByteArray path = request.getPath();

    // Forbid access to files outside the docroot directory
    if (path.startsWith("/scripts/.."))
    {
        qWarning("ScriptController: somebody attempted to access a file outside the docroot directory");
        response.setStatus(403,"forbidden");
        response.write("403 Forbidden",true);
        return;
    }

    // If the filename is a directory, we stop
    if (QFileInfo(docroot+path).isDir())
    {
        qWarning("ScriptController: we cannot execute a directory");
        response.setStatus(503, "service unavailable");
        response.write("Cannot execute a directory",true);
        return;
    }

    // 404 redirection
    QFile file(docroot+path);
    if (!file.exists())
    {
        // Redirect to homepage
        path = "/index.html";
        QFile anotherFile(docroot+path);

        // Even the homepage is missing!! Then we throw a 404 error
        if (!anotherFile.exists())
        {
            response.setStatus(404, "not found");
            response.write("404 Not found",true);
            return;
        }

        // Error opening the file
        if (!anotherFile.open(QIODevice::ReadOnly))
        {
            qWarning("StaticFileController: Cannot open existing file %s for reading",qPrintable(anotherFile.fileName()));
            response.setStatus(403, "forbidden");
            response.write("403 Forbidden",true);
            return;
        }

        BridgeController::SetContentType(path,response);
        // Return the file content, do not store in cache
        while (!anotherFile.atEnd() && !anotherFile.error())
            response.write(anotherFile.read(65536));
        anotherFile.close();
        return;
    }

#if defined (Q_WS_WIN)
    if (path.endsWith(".sh"))
    {
        qWarning("ScriptController: we cannot execute a shell script on Windows");
        response.setStatus(503, "service unavailable");
        response.write("Cannot execute a shell script on Windows",true);
        return;
    }
#else
    if (path.endsWith(".bat"))
    {
        qWarning("ScriptController: we cannot execute a batch script on Unix");
        response.setStatus(503, "service unavailable");
        response.write("Cannot execute a batch script on Unix",true);
        return;
    }
    else if (path.endsWith(".exe"))
    {
        qWarning("ScriptController: we cannot execute a Windows executable binary on Unix");
        response.setStatus(503, "service unavailable");
        response.write("Cannot execute a Windows executable binary on Unix",true);
        return;
    }
#endif

    //Construct the parameters for shell script
    //Example full request "/scripts/somescriptname.sh?param1=abc&param2=def"
    QMultiMap<QByteArray,QByteArray> params = request.getParameterMap();
    QMapIterator<QByteArray, QByteArray> i(params);
    QStringList arguments;
    while (i.hasNext()) {
         i.next();
         arguments.push_front(i.value());
    }

    // Execute the file
    QProcess *newProc = new QProcess();
    newProc->start(docroot+path, arguments);
    newProc->waitForFinished();

    // Print the output to HTML response
    response.write(newProc->readAllStandardOutput());
    newProc->close();
    delete newProc;
    newProc = NULL;
    arguments.clear();
}
