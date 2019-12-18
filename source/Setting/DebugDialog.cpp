#include "DebugDialog.h"
#include "ui_DebugDialog.h"

DebugDialog::DebugDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DebugDialog)
{
    ui->setupUi(this);
    initWidgets();
    initSignals();
}

DebugDialog::~DebugDialog()
{
    delete ui;
}

void DebugDialog::on_btn_XLeft_clicked()
{
    int length = ui->sp_Step->text().toInt();
    emit logMove("X左移:",length);
    emit Sig_btn_XLeft(length);
}

void DebugDialog::on_btn_ZDown_clicked()
{
    int length = ui->sp_Step->text().toInt();
    emit logMove("Z下移:",length);
    emit Sig_btn_ZDown(length);
}

void DebugDialog::on_btn_XRight_clicked()
{
    int length = ui->sp_Step->text().toInt();
    emit logMove("X右移:",length);
    emit Sig_btn_XRight(length);
}

void DebugDialog::on_btn_ZTop_clicked()
{
    int length = ui->sp_Step->text().toInt();
    emit logMove("Z上移:",length);
    emit Sig_btn_ZTop(length);
}

void DebugDialog::slot_InputLogs(QString info,int length)
{
    ui->history->append(info);
    ui->history->append("\n");
    QTextCursor text_cursor(ui->history->textCursor());
    text_cursor.movePosition(QTextCursor::End);
}

void DebugDialog::initWidgets()
{
    camGroup = new QButtonGroup;
    camGroup->setExclusive(true);
    camGroup->addButton(ui->rbtn_cam1);
    camGroup->addButton(ui->rbtn_cam2);
    camGroup->addButton(ui->rbtn_cam3);
    camGroup->setExclusive(true);
    ui->sp_Step->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
}

void DebugDialog::initSignals()
{
    connect(this,&DebugDialog::logMove,this,&DebugDialog::slot_InputLogs);
    connect(camGroup, static_cast<void(QButtonGroup::*)(int, bool)>(&QButtonGroup::buttonToggled),
          [=](int id, bool checked){
            if(!checked) return;
            emit camIndexChanged(id+4);
    });


}
