#ifndef CAMSETTING_H
#define CAMSETTING_H

#include <QDialog>
#include "src/Method/OpenConfig.h"

namespace Ui {
class CamSetting;
}

class CamSetting : public QDialog
{
    Q_OBJECT

public:
    explicit CamSetting(QWidget *parent = nullptr);
    ~CamSetting();

private slots:
    void on_btnExp1_clicked();
    void on_btnGain1_clicked();
    void on_btn_ROI_clicked();

    void on_cbx_select_activated(int index);

signals:
    void sig_ExpChanged(int index,int);
    void sig_GainChanged(int index,int);
    void sig_ROIChanged(int index,float x,float y,int width,int height);


private:
    Ui::CamSetting *ui;
    std::string cam_index;
    int _ca_index;
};

#endif // CAMSETTING_H
