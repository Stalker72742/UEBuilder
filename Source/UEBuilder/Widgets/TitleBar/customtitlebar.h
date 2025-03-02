//
// Created by Stalker7274 on 01.03.2025.
//

#ifndef UEBUILDER_CUSTOMTITLEBAR_H
#define UEBUILDER_CUSTOMTITLEBAR_H

#include <QWidget>
#include <QLabel>


QT_BEGIN_NAMESPACE
namespace Ui { class CustomTitleBar; }
QT_END_NAMESPACE

class CustomTitleBar : public QWidget {
Q_OBJECT

public:
    explicit CustomTitleBar(QWidget *parent = nullptr);

    ~CustomTitleBar() override;

signals:
    void minimizeClicked();
    void closeClicked();

private:
    Ui::CustomTitleBar *ui;

    QLabel *titleLabel;
    QPoint dragPosition;
protected:

    virtual void mousePressEvent(QMouseEvent *event) override;

    virtual void mouseMoveEvent(QMouseEvent *event) override;

};


#endif //UEBUILDER_CUSTOMTITLEBAR_H
