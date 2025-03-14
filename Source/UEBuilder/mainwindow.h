//
// Created by Stalker7274 on 23.02.2025.
//

#ifndef UEBUILDER_MAINWINDOW_H
#define UEBUILDER_MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include "FileFinder/GrokFileSearcher.h"
#include "FileFinder/FinderStructs.h"
#include <QProxyStyle>
#include <QStyleOptionComplex>

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

    void executeInMainThread(std::function<void()> callback);

    void OnProjectClicked();

private:

    void UpdateScrollBox();

    Ui::MainWindow *ui;

    QProcess *process;

    FileSearcher Searher;

    QVector<UProject> UProjects;
};

class TransparentScrollBarStyle : public QProxyStyle {
public:
    void drawComplexControl(QStyle::ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const override {
        if (control == QStyle::CC_ScrollBar) {
            QStyleOptionComplex opt(*option);
            opt.palette.setColor(QPalette::Window, Qt::transparent);
            QProxyStyle::drawComplexControl(control, &opt, painter, widget);
        } else {
            QProxyStyle::drawComplexControl(control, option, painter, widget);
        }
    }
};

#endif //UEBUILDER_MAINWINDOW_H