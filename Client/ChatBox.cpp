#include "ChatBox.h"
#include "ui_ChatBox.h"

ChatBox::ChatBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatBox)
{
    ui->setupUi(this);
}

ChatBox::~ChatBox()
{
    delete ui;
}

void ChatBox::on_send_clicked()
{
    //Salje poruku iz message u textBox..
}
