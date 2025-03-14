//
// Created by Stalker7274 on 01.03.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_CustomTitleBar.h" resolved

#include <QHBoxLayout>
#include <QPushButton>
#include <QMouseEvent>
#include <QFrame>
#include "customtitlebar.h"
#include "ui_CustomTitleBar.h"


CustomTitleBar::CustomTitleBar(QWidget *parent) :
        QWidget(parent), ui(new Ui::CustomTitleBar)
        {
    ui->setupUi(this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    titleLabel = new QLabel("Custom Window Title", this);
    titleLabel->setStyleSheet("padding: 5px; color: white; background: transparent");

    QPushButton *CloseButton = new QPushButton("X", this);

    QPushButton *minimizeButton = new QPushButton("🗕", this);

            ui->horizontalLayout->addWidget(titleLabel);
            ui->horizontalLayout->addStretch();
            ui->horizontalLayout->addWidget(minimizeButton);
            ui->horizontalLayout->addWidget(CloseButton);

    CloseButton->setStyleSheet("color: #a8a8a8");

    connect(CloseButton, &QPushButton::clicked, this, &CustomTitleBar::closeClicked);


    setFixedHeight(30);
    setStyleSheet("background-color: #2d2d2d;");
}

CustomTitleBar::~CustomTitleBar()
{
    delete ui;

}
void CustomTitleBar::mousePressEvent(QMouseEvent *event)
{

    if (event->button() == Qt::LeftButton) {
        dragPosition = event->globalPosition().toPoint() - parentWidget()->frameGeometry().topLeft();
        event->accept();
    }
}

void CustomTitleBar::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        parentWidget()->move(event->globalPosition().toPoint() - dragPosition);
        event->accept();
    }
}