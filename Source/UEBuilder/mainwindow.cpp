//
// Created by Stalker7274 on 23.02.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "mainwindow.h"
#include "ui_MainWindow.h"
#include <QProcess>
#include <QStringDecoder>
#include <boost/locale.hpp>

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    setAutoFillBackground(true);

    setStyleSheet("background-color: black;");

    //ui->horizontalLayoutWidget->setStyleSheet("background-color: black;");

    ui->Build->setStyleSheet("background-color: transparent; border: none; color: white;");
    //ui->pushButton_2->setStyleSheet("background-color: transparent; border: none; color: white;");
    //ui->pushButton_3->setStyleSheet("background-color: transparent; border: none; color: white;");

    ui->outputEdit->setStyleSheet("background-color: white;");

    process = new QProcess(this);

    connect(ui->Build, &QPushButton::clicked, this, &MainWindow::onBuildClicked);
    connect(process, &QProcess::readyReadStandardOutput, this, &MainWindow::readOutput);
    connect(process, &QProcess::readyReadStandardError, this, &MainWindow::readError);
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
           this, &MainWindow::onFinished);
}

MainWindow::~MainWindow() {
    delete process;
    delete ui;
}

void MainWindow::onBuildClicked()
{
    ui->outputEdit->clear();

    QString program = "C:/Program Files/Epic Games/UE_5.4/Engine/Build/BatchFiles/Build.bat";
    QStringList arguments;
    arguments <<"DoomEditor" << "Win64" << "Development" << "-project=D:/UE_Projects/Doom/Doom.uproject";
    process->start(program, arguments);
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

    ui->outputEdit->appendPlainText(Text);
}

void MainWindow::readError() {
    QByteArray data = process->readAllStandardError();
    ui->outputEdit->appendPlainText(QString::fromUtf8(data));
}

void MainWindow::onFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    Q_UNUSED(exitCode)
    Q_UNUSED(exitStatus)
    ui->outputEdit->appendPlainText("Команда завершена.");
}