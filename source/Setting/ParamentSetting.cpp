#include "ParamentSetting.h"
#include "ui_ParamentSetting.h"

ParamentSetting::ParamentSetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ParamentSetting)
{
    ui->setupUi(this);
}

ParamentSetting::~ParamentSetting()
{
    delete ui;
}
