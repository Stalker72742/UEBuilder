//
// Created by Stalker7274 on 13.03.2025.
//

#ifndef UEBUILDER_CUSTOMBUTTON_H
#define UEBUILDER_CUSTOMBUTTON_H

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui { class CustomButton; }
QT_END_NAMESPACE

class CustomButton : public QWidget {
Q_OBJECT

public:
    explicit CustomButton(QString Text = "", QWidget *parent = nullptr);

    ~CustomButton() override;

    void Clicked(bool Checked);

signals:
    void OnClicked(CustomButton *Button);

private:
    Ui::CustomButton *ui;
};


#endif //UEBUILDER_CUSTOMBUTTON_H
