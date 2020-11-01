//Local
#include "HellowPopUp.h"
#include "ui_HellowPopUp.h"

//Qt
#include <QDebug>

HellowPopUp::HellowPopUp(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HellowPopUp)
{
    ui->setupUi(this);

    ui->spinBoxCellCount->setRange(4, 100);

    connect(ui->btnStartGame, &QPushButton::clicked, this, [this](bool checked)
    {
        Q_UNUSED(checked)

        int cellCount = ui->spinBoxCellCount->text().toInt();

        emit startGame(cellCount);

        qInfo() << QString("Start game %1x%1").arg(cellCount);
    });
}

HellowPopUp::~HellowPopUp()
{
    delete ui;
}
