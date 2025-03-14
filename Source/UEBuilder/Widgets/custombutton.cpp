//
// Created by Stalker7274 on 13.03.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_CustomButton.h" resolved

#include "custombutton.h"
#include "ui_CustomButton.h"
#include <QPushButton>
#include <QVBoxLayout>

CustomButton::CustomButton(QString Text, QWidget *parent) :
        QWidget(parent), ui(new Ui::CustomButton) {
    ui->setupUi(this);

    QPushButton *Button = new QPushButton(Text, this);

    Button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    QVBoxLayout *layout = new QVBoxLayout;

    layout->addWidget(Button);

    setLayout(layout);

    connect(Button, &QPushButton::clicked, this, &CustomButton::Clicked);
}

CustomButton::~CustomButton() {
    delete ui;
}

void CustomButton::Clicked(bool Checked) {
    emit OnClicked(this);
}
