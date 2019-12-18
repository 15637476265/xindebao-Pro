#include "CamSetting.h"
#include "ui_CamSetting.h"
#include <QMessageBox>

CamSetting::CamSetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CamSetting)
{
    ui->setupUi(this);
    cam_index = "Cam1";
    _ca_index=0;
    int exp = OpenConfig::get<int>("conf/camera.json","Cam1.exposure");
    ui->ldtExp1->setText(QString::number(exp));

}

CamSetting::~CamSetting()
{
    delete ui;
}

void CamSetting::on_btnExp1_clicked()
{
    int ret = OpenConfig::write<int>("conf/camera.json",cam_index+".exposure",ui->ldtExp1->text().toInt());
    if(0 != ret){
        QMessageBox::warning(this, tr("Write Error"),
                                         tr("Write Parament Error!"),
                                         QMessageBox::Cancel);
    }
    emit sig_ExpChanged(_ca_index,ui->ldtExp1->text().toInt());
}

void CamSetting::on_btnGain1_clicked()
{
    int ret = OpenConfig::write<int>("conf/camera.json",cam_index+".gain",ui->ldtGain1->text().toInt());
    if(0 != ret){
        QMessageBox::warning(this, tr("Write Error"),
                                         tr("Write Parament Error!"),
                                         QMessageBox::Cancel);
    }
    emit sig_ExpChanged(_ca_index,ui->ldtGain1->text().toInt());
}


void CamSetting::on_btn_ROI_clicked()
{
    float x = ui->ldtRoix1->text().toFloat();
    float y = ui->ldtRoiy1->text().toFloat();
    int w = ui->ldtRoiw1->text().toInt();
    int h = ui->ldtRoih1->text().toInt();

    emit sig_ROIChanged(_ca_index,x,y,w,h);
}


void CamSetting::on_cbx_select_activated(int index)
{
    cam_index = "Cam" + to_string(index+1);
    _ca_index = index;
    int exp = OpenConfig::get<int>("conf/camera.json",cam_index+".exposure");
    ui->ldtExp1->setText(QString::number(exp));
    int gain = OpenConfig::get<int>("conf/camera.json",cam_index+".gain");
    ui->ldtGain1->setText(QString::number(gain));
}
