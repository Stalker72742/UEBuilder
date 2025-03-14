//
// Created by Stalker7274 on 05.03.2025.
//

#include "FileFinder.h"
#include <QStandardPaths>
#include <QDirIterator>
#include <QStorageInfo>
#include <QFuture>

FileFinder::FileFinder(QObject *parent) : QObject(parent) {

    QString configDir = QCoreApplication::applicationDirPath() + "/Configs";
    QDir().mkpath(configDir);

    QString configPath = configDir + "/FoundedFiles.ini";
    settings = new QSettings(configPath, QSettings::IniFormat, this);

    if (!QFile::exists(configPath)) {

        qWarning() << "Config does not exist. Creating...";

        SetupConfig();
    } else {
        qDebug() << "Config exist." << configPath;
    }
}

void FileFinder::SetupConfig()
{
    QFile configFile(QCoreApplication::applicationDirPath() + "/Configs/FoundedFiles.ini");

    settings->beginGroup("Find settings");
    settings->remove("");

    QStringList Paths  = {"c:", "d:","e:"};

    settings->setValue("FindIgnore", Paths);

    settings->endGroup();
    settings->sync();

    qDebug() << "Config crated and has default find settings";
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
