#ifndef PARAMENTSETTING_H
#define PARAMENTSETTING_H

#include <QDialog>

namespace Ui {
class ParamentSetting;
}

class ParamentSetting : public QDialog
{
    Q_OBJECT

public:
    explicit ParamentSetting(QWidget *parent = nullptr);
    ~ParamentSetting();

private:
    Ui::ParamentSetting *ui;
};

#endif // PARAMENTSETTING_H
