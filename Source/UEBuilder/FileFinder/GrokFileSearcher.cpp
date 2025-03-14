//
// Created by Stalker7274 on 08.03.2025.
//

#include "GrokFileSearcher.h"

#include <QDirIterator>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QCoreApplication>

FileSearcher::FileSearcher(QObject* parent)
        : QObject(parent), m_processedDirs(0), m_totalDirs(0) {
}

FileSearcher::~FileSearcher() {
    QThreadPool::globalInstance()->waitForDone(); // Ждем завершения всех потоков
}

void FileSearcher::searchFiles(const QString& searchTerm, bool byExtension) {
    QThreadPool* pool = QThreadPool::globalInstance();
    m_processedDirs = 0;
    m_totalDirs = 0;

    // Опционально: очищаем предыдущие результаты перед новым поиском
    // Можно сделать это опциональным через параметр функции
    // clearFoundFilesInConfig();

    QFileInfoList drives = QDir::drives();

    for (const QFileInfo& drive : drives) {
        if (isPathIgnored(drive.absoluteFilePath())){
            continue;
        }
        processDirectory(drive.absoluteFilePath(), searchTerm, byExtension, pool);
    }
}

void FileSearcher::processDirectory(const QString& path, const QString& searchTerm,
                                    bool byExtension, QThreadPool* pool) {
    if (isPathIgnored(path)) {
        m_processedDirs++;
        emit progressUpdated((m_processedDirs * 100) / m_totalDirs);
        return;
    }

    SearchWorker* worker = new SearchWorker(path, searchTerm, byExtension,
                                            m_ignorePaths, this);
    pool->start(worker);
}

bool FileSearcher::loadConfig(const QString& configPath) {
    QFile configFile(configPath);
    if (!configFile.open(QIODevice::ReadOnly)) return false;
    QJsonDocument doc = QJsonDocument::fromJson(configFile.readAll());
    m_config = doc.object();
    QStringList List;
    QJsonArray Array = m_config["ignorePaths"].toArray();

    for(QJsonValue Value : Array){
        List.append(Value.toString());
    }

    m_ignorePaths = List;
    configFile.close();
    return true;
}

bool FileSearcher::saveConfig(const QString& configPath) {
    m_config["ignorePaths"] = QJsonArray::fromStringList(m_ignorePaths);
    QFile configFile(configPath);
    if (!configFile.open(QIODevice::WriteOnly)) return false;
    QJsonDocument doc(m_config);
    configFile.write(doc.toJson());
    configFile.close();
    return true;
}

void FileSearcher::addIgnorePath(const QString& path) {
    QMutexLocker locker(&m_mutex);
    if (!m_ignorePaths.contains(path)) m_ignorePaths.append(path);
}

void FileSearcher::removeIgnorePath(const QString& path) {
    QMutexLocker locker(&m_mutex);
    m_ignorePaths.removeAll(path);
}

bool FileSearcher::isPathIgnored(const QString& path) const {
    for(const QString& ignored : m_ignorePaths) {
        if (path.startsWith(ignored)) return true;
    }
    return false;
}

void FileSearcher::emitFileResult(const QFileInfo& fileInfo) {
    FileResult result{fileInfo.fileName(), fileInfo.absoluteFilePath(), fileInfo.size()};

    updateFoundFilesInConfig(result);

    emit fileFound(result);
}

void FileSearcher::updateFoundFilesInConfig(const FileSearcher::FileResult &result) {
    QMutexLocker locker(&m_mutex);

    // Получаем текущий массив найденных файлов или создаем новый
    QJsonArray foundFilesArray;
    if (m_config.contains("foundFiles")) {
        foundFilesArray = m_config["foundFiles"].toArray();
    }

    // Создаем объект с информацией о файле
    QJsonObject fileObject;
    fileObject["name"] = result.fileName;
    fileObject["path"] = result.filePath;
    /*fileObject["size"] = static_cast<double>(result.fileSize);
    fileObject["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);*/

    // Добавляем информацию о файле в массив
    foundFilesArray.append(fileObject);

    // Обновляем конфиг
    m_config["foundFiles"] = foundFilesArray;
    saveConfig("Configs/Config.json");
}

// SearchWorker
FileSearcher::SearchWorker::SearchWorker(const QString& path, const QString& searchTerm,
                                         bool byExt, const QStringList& ignorePaths,
                                         QPointer<FileSearcher> parent)
        : m_path(path), m_searchTerm(searchTerm.toLower()), m_byExtension(byExt),
          m_ignorePaths(ignorePaths), m_parent(parent) {
}

void FileSearcher::SearchWorker::run() {
    if (!m_parent) return; // Проверка на валидность родителя

    QDirIterator it(m_path, QDir::Files | QDir::NoDotAndDotDot,
                    QDirIterator::Subdirectories);

    while (it.hasNext() && m_parent) {
        it.next();
        QFileInfo info = it.fileInfo();

        if (m_parent->isPathIgnored(info.absoluteFilePath())) continue;

        QString compareStr = m_byExtension ? info.suffix().toLower()
                                           : info.fileName().toLower();
        if (compareStr.contains(m_searchTerm)) {
            emit m_parent->emitFileResult(info);
        }
    }

    if (m_parent) {

        if (m_parent->m_processedDirs == m_parent->m_totalDirs) {

            emit m_parent->searchFinished();
        }
    }
}