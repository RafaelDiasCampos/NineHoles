#ifndef NINEHOLES_H
#define NINEHOLES_H

#include <QMainWindow>
#include <Hole.h>

QT_BEGIN_NAMESPACE
namespace Ui {
    class NineHoles;
}
QT_END_NAMESPACE

class NineHoles : public QMainWindow {
    Q_OBJECT

public:
    NineHoles(QWidget *parent = nullptr);
    virtual ~NineHoles();

private:
    Ui::NineHoles *ui;
    Hole* m_board[9];

private slots:
    void play(int index);
    void reset();
    void showAbout();

};

#endif // NINEHOLES_H
