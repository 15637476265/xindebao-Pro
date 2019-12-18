#ifndef MOTORSETTING_H
#define MOTORSETTING_H

#include <QDialog>

namespace Ui {
class MotorSetting;
}

class MotorSetting : public QDialog
{
    Q_OBJECT

public:
    explicit MotorSetting(QWidget *parent = 0);
    ~MotorSetting();

private slots:
    void on_portSet_clicked();
    void on_bauSet_clicked();

    void on_checkBox_toggled(bool checked);

signals:
    void sig_PortChanged();
    void sig_BauChanged();
    void sig_ShieldRun(bool);

private:
    Ui::MotorSetting *ui;
};

#endif // MOTORSETTING_H
