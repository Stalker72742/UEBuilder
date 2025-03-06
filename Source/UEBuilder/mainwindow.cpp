//
// Created by Stalker7274 on 23.02.2025.
//

#include "mainwindow.h"
#include "ui_MainWindow.h"
#include "Widgets/TitleBar/customtitlebar.h"
#include <QProcess>
#include <QStringDecoder>
#include <Boost/include/boost-1_87/boost/locale.hpp>
#include <QLabel>
#include "FileFinder/FileFinder.h"

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
    connect(process, &QProcess::readyReadStandardOutput, this, &MainWindow::readOutput);
    connect(process, &QProcess::readyReadStandardError, this, &MainWindow::readError);
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
           this, &MainWindow::onFinished);

    QWidget *contentWidget1 = new QWidget;
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget1);


    for (int i = 1; i <= 20; ++i) {
        QLabel *label = new QLabel(QString("Элемент %1").arg(i));
        contentLayout->addWidget(label);
        label->setStyleSheet("color: white");
    }
    contentWidget1->setLayout(contentLayout);

    ui->scrollArea->setWidget(contentWidget1);
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

void MainWindow::onUpdateClicked() {

    FileScannerThread *scannerThread = new FileScannerThread(QStringList{"UnrealEditor.exe"});
    scannerThread->start();
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