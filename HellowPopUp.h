#ifndef HELLOWPOPUP_H
#define HELLOWPOPUP_H

#include <QWidget>

namespace Ui {
class HellowPopUp;
}

class HellowPopUp : public QWidget
{
    Q_OBJECT

public:
    explicit HellowPopUp(QWidget *parent = nullptr);
    ~HellowPopUp();
signals:
    void startGame(int cellNumber);
private:
    Ui::HellowPopUp *ui;
};

#endif // HELLOWPOPUP_H
