//
// Created by Stalker7274 on 13.03.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_CustomButton.h" resolved

#include "custombutton.h"
#include "ui_CustomButton.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

CustomButton::CustomButton(QString Text, QWidget *parent) :
        QWidget(parent), ui(new Ui::CustomButton) {
    ui->setupUi(this);

    auto Label = new QLabel(Text);
    Label->setWordWrap(true);

    auto layout = new QHBoxLayout();
    layout->addWidget(Label,0,Qt::AlignLeft);

    //ui->Button->setText(Text);

    ui->Button->setLayout(layout);


    connect(ui->Button, &QPushButton::clicked, this, &CustomButton::Clicked);
}

CustomButton::~CustomButton() {
    delete ui;
}

void CustomButton::Clicked(bool Checked) {
    emit OnClicked(this);
}
