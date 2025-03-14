//
// Created by Stalker7274 on 05.03.2025.
//

#include <QObject>
#include <QCoreApplication>
#include <QSettings>
#include <QThread>

#ifndef UEBUILDER_FILEFINDER_H
#define UEBUILDER_FILEFINDER_H

enum FindType {
    FileName,
    Extention
};

class FileFinder : public QObject {
    Q_OBJECT
public:

   explicit FileFinder(QObject* parent = nullptr);

    void Scan(const QStringList &extensions, FindType LFindType = FileName);

private:

    QSettings *settings;

    FindType FindType = FileName;

    void scanDirectory(const QString &path, const QStringList &extensions);

    void SetupConfig();
};

class FileScannerWorker : public QObject
{
Q_OBJECT

public:
    explicit FileScannerWorker(const QStringList &extensions, QObject *parent = nullptr)
            : QObject(parent), extensions(extensions) {}

public slots:
    void process()
    {
        FileFinder scanner;
        scanner.Scan(extensions);
        emit finished();
    }

signals:
    void finished();

private:
    QStringList extensions;
};

class FileScannerThread : public QThread
{
Q_OBJECT

public:
    explicit FileScannerThread(const QStringList &extensions, QObject *parent = nullptr)
            : QThread(parent), extensions(extensions) {}

    void run() override
    {
        FileScannerWorker *worker = new FileScannerWorker(extensions);
        connect(worker, &FileScannerWorker::finished, this, &QThread::quit);
        connect(worker, &FileScannerWorker::finished, worker, &QObject::deleteLater);
        worker->process();
    }

private:
    QStringList extensions;
};


#endif //UEBUILDER_FILEFINDER_H
