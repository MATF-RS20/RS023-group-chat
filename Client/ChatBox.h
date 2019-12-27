#ifndef CHATBOX_H
#define CHATBOX_H

#include <QWidget>

namespace Ui {
class ChatBox;
}

class ChatBox : public QWidget
{
    Q_OBJECT

public:
    explicit ChatBox(QWidget *parent = nullptr);
    ~ChatBox();

private slots:
    void on_send_clicked();

private:
    Ui::ChatBox *ui;
};

#endif // CHATBOX_H
