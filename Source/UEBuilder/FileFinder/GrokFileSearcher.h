//
// Created by Stalker7274 on 08.03.2025.
//
#include <QObject>
#include <QDir>
#include <QFileInfo>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QThreadPool>
#include <QRunnable>
#include <QMutex>
#include <QAtomicInt>
#include <QPointer>

#ifndef UEBUILDER_GROKFILESEARCHER_H
#define UEBUILDER_GROKFILESEARCHER_H


class FileSearcher : public QObject {
Q_OBJECT

public:
    explicit FileSearcher(QObject* parent = nullptr);
    ~FileSearcher() override;

    struct FileResult {
        QString fileName;
        QString filePath;
        qint64 fileSize;
    };

    void searchFiles(const QString& searchTerm, bool byExtension = false);
    bool loadConfig(const QString& configPath);
    bool saveConfig(const QString& configPath);
    void addIgnorePath(const QString& path);
    void removeIgnorePath(const QString& path);

    QJsonArray getFoundFilesFromConfig() const {
        return m_config["foundFiles"].toArray();
    };

    void clearFoundFilesInConfig() {
        QMutexLocker locker(&m_mutex);
        m_config["foundFiles"] = QJsonArray();
    }

signals:
    void fileFound(const FileResult& result);
    void searchFinished();
    void progressUpdated(int percentage);

private:
    class SearchWorker : public QRunnable {
    public:
        SearchWorker(const QString& path, const QString& searchTerm,
                     bool byExt, const QStringList& ignorePaths,
                     QPointer<FileSearcher> parent);
        void run() override;

    private:
        QString m_path;
        QString m_searchTerm;
        bool m_byExtension;
        QStringList m_ignorePaths;
         // Безопасный указатель
         QPointer<FileSearcher> m_parent;
    };

    void processDirectory(const QString& path, const QString& searchTerm,
                          bool byExtension, QThreadPool* pool);

    bool isPathIgnored(const QString& path) const;

    void emitFileResult(const QFileInfo& fileInfo);

    void updateFoundFilesInConfig(const FileResult& result);

    QStringList m_ignorePaths;
    QMutex m_mutex;
    QAtomicInt m_processedDirs;
    QAtomicInt m_totalDirs;
public:
    QJsonObject m_config;
};

#endif //UEBUILDER_GROKFILESEARCHER_H
