#include <QCoreApplication>
#include <QDebug>
#include <QtCore>
#include <QFile>
#include <QDir>
#include <QSettings>

const QString clientSettingsFileName = "LaucnherSettings.ini";
const QString serverSettingsFileName = "ServerSettings.ini";

const QString versionFieldName = "Verstion";
const QString clientServerPathFieldName = "ServerPath";
const QString clientExecutableFieldName = "Executable";

const QString serverTrackingFilesFieldName = "TrackingFiles";

int parceVersion(QString text){
    QList<QString> values = text.split(".");

    switch (values.count()) {
    case 0:
        return 0;
    case 1:
        return values.at(0).toInt()*1000000;
    case 2:
        return values.at(0).toInt()*1000000 + values.at(1).toInt()*1000;
    case 3:
        return values.at(0).toInt()*1000000 + values.at(1).toInt()*1000 + values.at(2).toInt();
    default:
        return 0;
    }
}

int main(int argc, char *argv[])
{
    Q_UNUSED(argc)
    QCoreApplication::setOrganizationName("BINP");
    QCoreApplication::setApplicationName("Launcher");
    if(!QFileInfo(clientSettingsFileName).isFile()){
        qDebug() << "No settings file";
        return -1;
    }
    QSettings clientSettings(clientSettingsFileName,QSettings::IniFormat);
    QString serverPath = clientSettings.value(clientServerPathFieldName).toString();
    if(QDir(serverPath).isReadable()){
        if(QFileInfo(serverPath + serverSettingsFileName).isFile()){
            QSettings serverSettings(serverPath + serverSettingsFileName,QSettings::IniFormat);


            QString serverVersionStr = serverSettings.value(versionFieldName).toString();
            int serverVersion = parceVersion(serverVersionStr);
            int clientVersion = parceVersion(clientSettings.value(versionFieldName).toString());
            if(serverVersion > clientVersion){
                QStringList trackingFiles = serverSettings.value(serverTrackingFilesFieldName).toString().split("|");
                for(int i = 0; i < trackingFiles.count(); i++){
                    if(QFile::exists(trackingFiles.at(i))){
                        QFile::remove(trackingFiles.at(i));
                    }
                    QFile::copy(serverPath + trackingFiles.at(i),trackingFiles.at(i));
                }
                clientSettings.setValue(versionFieldName,serverVersionStr);
                clientSettings.sync();
            }
        }
    }
    execv(clientSettings.value(clientExecutableFieldName).toString().toLatin1().data(),argv);
    return 0;
}
