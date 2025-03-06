//
// Created by Stalker7274 on 23.02.2025.
//

#ifndef UEBUILDER_MAINWINDOW_H
#define UEBUILDER_MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

    void onBuildClicked();

    void onUpdateClicked();

    void readOutput();

    void readError();

    void onFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    Ui::MainWindow *ui;

    QProcess *process;
};

#endif //UEBUILDER_MAINWINDOW_H