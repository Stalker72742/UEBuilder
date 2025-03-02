//
// Created by Stalker7274 on 01.03.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_CustomTitleBar.h" resolved

#include <QHBoxLayout>
#include <QPushButton>
#include <QMouseEvent>
#include "customtitlebar.h"
#include "ui_CustomTitleBar.h"


CustomTitleBar::CustomTitleBar(QWidget *parent) :
        QWidget(parent), ui(new Ui::CustomTitleBar)
        {
    ui->setupUi(this);

    setFixedHeight(30);

    //setStyleSheet("background-color: #3498db; color: white;");

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // Растяжка для сдвига кнопки вправо
    layout->addStretch();

    QPushButton *CloseButton = new QPushButton("X", this);
    layout->addWidget(CloseButton);

    //connect(ui->minimizeButton, &QPushButton::clicked, this, &CustomTitleBar::minimizeClicked);
    connect(CloseButton, &QPushButton::clicked, this, &CustomTitleBar::closeClicked);
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