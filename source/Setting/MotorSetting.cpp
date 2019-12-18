#include "MotorSetting.h"
#include "ui_MotorSetting.h"
#include "src/Method/OpenConfig.h"
#include <QMessageBox>


MotorSetting::MotorSetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MotorSetting)
{
    ui->setupUi(this);
}

MotorSetting::~MotorSetting()
{
    delete ui;
}

void MotorSetting::on_portSet_clicked()
{
    std::string port = OpenConfig::get<std::string>("conf/serial.json","port");
    if(ui->cbx_port->currentText().toStdString() != port){
        emit sig_PortChanged();
        int ret = OpenConfig::write<std::string>("conf/serial.json","port",ui->cbx_port->currentText().toStdString());
        if(0 != ret){
            QMessageBox::warning(this, tr("Write Error"),
                                             tr("Write Parament Error!"),
                                             QMessageBox::Cancel);
        }
    }
}

void MotorSetting::on_bauSet_clicked()
{
    int ret = OpenConfig::get<int>("conf/serial.json","bau");
    if(ui->cbx_bau->currentText().toInt() != ret){
        emit sig_BauChanged();
        ret = OpenConfig::write<int>("conf/serial.json","bau",ui->cbx_port->currentText().toInt());
        if(0 != ret){
            QMessageBox::warning(this, tr("Write Error"),
                                             tr("Write Parament Error!"),
                                             QMessageBox::Cancel);
        }
    }



}

void MotorSetting::on_checkBox_toggled(bool checked)
{
    emit sig_ShieldRun(checked);
}
