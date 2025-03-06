//
// Created by Stalker7274 on 05.03.2025.
//

#include "FileFinder.h"
#include <QStandardPaths>
#include <QDirIterator>
#include <QStorageInfo>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>

FileFinder::FileFinder(QObject *parent) : QObject(parent) {

    QString configDir = QCoreApplication::applicationDirPath() + "/Configs";
    QDir().mkpath(configDir);  // Создаём папку, если её нет

    QString configPath = configDir + "/FoundedFiles.ini";
    settings = new QSettings(configPath, QSettings::IniFormat, this);

    if (!QFile::exists(configPath)) {
        qWarning() << "File do not create automaticly";
    } else {
        qDebug() << "Config creating: " << configPath;
    }

    QFile configFile(QCoreApplication::applicationDirPath() + "/Configs/FoundedFiles.ini");
    if (!configFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
        qWarning() << "Access denied";
    } else {
        qDebug() << "Can write";
        configFile.close();
    }
}

void FileFinder::Scan(const QStringList &extensions, enum FindType LFindType)
{
    QStringList rootPaths;

    FindType = LFindType;

    for (const QStorageInfo &storage : QStorageInfo::mountedVolumes()) {
        if (storage.isValid() && storage.isReady() && !storage.isReadOnly()) {
            rootPaths.append(storage.rootPath());
        }
    }

    if (rootPaths.isEmpty()) {
        qWarning() << "Не удалось получить корневые пути";
        return;
    }

    settings->beginGroup("Files");
    settings->remove("");

    for (const QString &rootPath : rootPaths) {
        scanDirectory(rootPath, extensions);
    }

    settings->endGroup();
    settings->sync();
    qDebug() << "Scan completed";

}

void FileFinder::scanDirectory(const QString &path, const QStringList &extensions)
{
    QDirIterator it(path, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString filePath = it.next();
        QString fileName = it.fileName();
        QString fileExtension = QFileInfo(filePath).suffix().toLower();


        if(!filePath.contains("U:")){
            break;
        }

        if (filePath.contains("Binaries")){

            /*QMutex mutex;

            mutex.lock();
            qDebug() << "Current folder: " << filePath;
            mutex.unlock();*/

            switch (FindType) {
                case FileName:
                    if (extensions.isEmpty() || extensions.contains(fileName)) {
                        settings->setValue(fileName, filePath);
                        settings->sync();
                    }
                    break;
                case Extention:
                    if (extensions.isEmpty() || extensions.contains(fileExtension)) {
                        settings->setValue(fileName, filePath);
                        settings->sync();
                    }
                    break;
            }
        }
    }
}

void FileFinder::StartScan(const QStringList &extensions) {

    //QFuture<void> future = QtConcurrent::run(this, &FileFinder::Scan, extensions);
}
