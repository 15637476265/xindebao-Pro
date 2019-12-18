#ifndef DEBUGDIALOG_H
#define DEBUGDIALOG_H

#include <QDialog>
#include <QButtonGroup>

namespace Ui {
class DebugDialog;
}

class DebugDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DebugDialog(QWidget *parent = 0);
    ~DebugDialog();

private slots:
    void on_btn_XLeft_clicked();

    void on_btn_ZDown_clicked();

    void on_btn_XRight_clicked();

    void on_btn_ZTop_clicked();

    void slot_InputLogs(QString info,int length);

signals:
    void Sig_btn_XLeft(int);
    void Sig_btn_XRight(int);
    void Sig_btn_ZTop(int);
    void Sig_btn_ZDown(int);

    void logMove(QString ,int);
    void camIndexChanged(int);
private:
    Ui::DebugDialog *ui;
    QButtonGroup *camGroup;


    void initWidgets();
    void initSignals();
};

#endif // DEBUGDIALOG_H
