//
// Created by Stalker7274 on 23.02.2025.
//

#include "mainwindow.h"
#include "ui_MainWindow.h"
#include "Widgets/TitleBar/customtitlebar.h"
#include <QProcess>
#include <QStringDecoder>
#include <Boost/include/boost-1_87/boost/locale.hpp>
#include "FileFinder/FileFinder.h"
#include "FileFinder/GrokFileSearcher.h"
#include "Widgets/custombutton.h"
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);

    CustomTitleBar* titleBar = new CustomTitleBar(this);

    setMenuWidget(titleBar);

    connect(titleBar, &CustomTitleBar::minimizeClicked, this, &MainWindow::showMinimized);
    connect(titleBar, &CustomTitleBar::closeClicked, this, &MainWindow::close);

    setAutoFillBackground(true);

    //ui->horizontalLayoutWidget->setStyleSheet("background-color: black;");

    ui->Build->setStyleSheet("background-color: transparent; border: none; color: white;");
    ui->Rebuild->setStyleSheet("background-color: transparent; border: none; color: white;");
    ui->Update->setStyleSheet("background-color: transparent; border: none; color: white;");

    ui->OutputLog->setStyleSheet("background-color: black; border: white; color: white");

    process = new QProcess(this);

    connect(ui->Build, &QPushButton::clicked, this, &MainWindow::onBuildClicked);
    connect(ui->Update, &QPushButton::clicked, this, &MainWindow::onUpdateClicked);
    connect(ui->LaunchProject, &QPushButton::clicked, this, &MainWindow::onLaunchProject);

    connect(process, &QProcess::readyReadStandardOutput, this, &MainWindow::readOutput);
    connect(process, &QProcess::readyReadStandardError, this, &MainWindow::readError);
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
           this, &MainWindow::onFinished);

    ui->scrollArea->verticalScrollBar()->setStyleSheet(
            "QScrollBar:vertical {"
            "    background: transparent;"
            "    border: none;"
            "    width: 10px;"
            "    margin: 0px 0px 0px 0px;"
            "}"
            "QScrollBar::handle:vertical {"
            "    background: #171717;"
            "    min-height: 20px;"
            "}"
            "QScrollBar::add-line:vertical {"
            "    height: 0px;"
            "}"
            "QScrollBar::sub-line:vertical {"
            "    height: 0px;"
            "}"
    );

    ui->scrollArea->verticalScrollBar()->setStyle(new QCommonStyle);

    UpdateScrollBox();
}

MainWindow::~MainWindow() {
    delete process;
    delete ui;
}

void MainWindow::onBuildClicked()
{
    ui->OutputLog->clear();

    QString program = "U:/Engines/UE_5.4/Engine/Build/BatchFiles/Build.bat";
    QStringList arguments;
    arguments <<"DoomEditor" << "Win64" << "Development" << "-project=U:/UE_Projects/Doom/Doom.uproject";
    process->start(program, arguments);
}

void MainWindow::onLaunchProject() {

    QString Engine = "U:/Engines/UE_5.4/Engine/Binaries/Win64/UnrealEditor.exe";

    QStringList Args = {CurrentUProject.ProjectPath};

    qDebug() << CurrentUProject.UEditorPath;

    return;

    bool success = QProcess::startDetached(Engine, Args);

    if (success) {
        qDebug() << "Success";
    } else {
        qDebug() << "Error";
    }

    qDebug() << "Trying start project: " + CurrentUProject.ProjectPath;
}

void MainWindow::onUpdateClicked() {

    /*FileScannerThread *scannerThread = new FileScannerThread(QStringList{"UnrealEditor.exe"});
    scannerThread->start();*/

    Searher.loadConfig("search_config.json");

    Searher.addIgnorePath("C:/Windows");
    Searher.addIgnorePath("C:/Program Files");
    Searher.addIgnorePath("C:/");

    QObject::connect(&Searher, &FileSearcher::fileFound,
                     [](const FileSearcher::FileResult& result) {
                         qDebug() << "Found:" << result.filePath << "Size:" << result.fileSize;
                     });

    connect(&Searher, &FileSearcher::searchFinished,
                     [this]() {
                         qDebug() << "Search completed!";

                         QMetaObject::invokeMethod(qApp, [this]() {

                            UpdateScrollBox();

                         }, Qt::QueuedConnection);

                     });

    Searher.searchFiles("uproject", true);
}

void MainWindow::UpdateScrollBox() {

    QFile configFile("Configs/Config.json");
    if (!configFile.open(QIODevice::ReadOnly)) return;
    QJsonDocument Config = QJsonDocument::fromJson(configFile.readAll());

    QJsonArray Array = Config["foundFiles"].toArray();

    for (QJsonValue Value : Array) {

        UProjects.append(UProject(Value["path"].toString()));
    }

    if (CurrentUProject.ProjectName == ""){

        CurrentUProject = UProjects[1];
    }

    QWidget *contentWidget1 = new QWidget;
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget1);

    contentLayout->setSpacing(0);

    for (UProject Project : UProjects) {

        CustomButton *button = new CustomButton(QString(Project.ProjectName + " UE%1").arg(Project.UEditorVersion).remove(".uproject"));

        button->setStyleSheet("color: white; background-color: #333;");

        button->setMinimumHeight(90);

        //button->setMaximumWidth(200);

        button->setFixedSize(200, 90);

        button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);

        contentLayout->addWidget(button);

        qDebug() << "Slot executed in:" << QThread::currentThread() << " (main thread:"
                 << QThread::currentThread()->isMainThread() << ")";
        qDebug() << QString(Project.ProjectName + " UE%1").arg(Project.UEditorVersion);
    }

    contentLayout->setContentsMargins(0, 0, 0, 0);

    contentWidget1->setLayout(contentLayout);

    contentWidget1->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    contentWidget1->adjustSize();

    ui->scrollArea->setWidget(contentWidget1);
}

void MainWindow::readOutput() {

    QByteArray data = process->readAllStandardOutput();

    QString Text;

    std::string windowsStr(data.constData(), data.size());

    try {
        std::string utf8Str = boost::locale::conv::to_utf<char>(windowsStr, "CP866");

        Text = QString::fromUtf8(utf8Str.c_str());

    } catch (const boost::locale::conv::conversion_error &e) {
        //std::cerr << "Ошибка конвертации: " << e.what() << std::endl;
    }

    ui->OutputLog->appendPlainText(Text);
}

void MainWindow::readError() {
    QByteArray data = process->readAllStandardError();
    ui->OutputLog->appendPlainText(QString::fromUtf8(data));
}

void MainWindow::onFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    Q_UNUSED(exitCode)
    Q_UNUSED(exitStatus)
}

void MainWindow::executeInMainThread(std::function<void()> callback) {

    QThread* currentThread = QThread::currentThread();
    // Получаем главный поток приложения
    QThread* mainThread = QCoreApplication::instance()->thread();

    // Если мы не в главном потоке
    if (currentThread != mainThread) {
        // Создаем объект для выполнения в главном потоке
        QObject* context = new QObject();
        QMetaObject::invokeMethod(context, [context, callback]() {
            callback();
            context->deleteLater(); // Удаляем временный объект после выполнения
        }, Qt::QueuedConnection);
    } else {
        // Если уже в главном потоке, просто выполняем
        callback();
    }
}