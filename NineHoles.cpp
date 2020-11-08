#include "NineHoles.h"
#include "ui_NineHoles.h"

#include <QMessageBox>
#include <QSignalMapper>

#include <QDebug>

NineHoles::NineHoles(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::NineHoles) {

    ui->setupUi(this);

    QObject::connect(ui->actionNew, SIGNAL(triggered(bool)), this, SLOT(reset()));
    QObject::connect(ui->actionQuit, SIGNAL(triggered(bool)), qApp, SLOT(quit()));
    QObject::connect(ui->actionAbout, SIGNAL(triggered(bool)), this, SLOT(showAbout()));

    // Load the pieces.
    QSignalMapper* map = new QSignalMapper(this);
    for (int idx = 0; idx < 9; ++idx) {
        int r = idx / 3;
        int c = idx % 3;

        Hole* hole = this->findChild<Hole*>(QString("hole%1%2").arg(r).arg(c));
        Q_ASSERT(hole != nullptr);
        hole->setRow(r);
        hole->setCol(c);

        m_board[idx] = hole;
        map->setMapping(hole, idx);
        QObject::connect(hole, SIGNAL(clicked()), map, SLOT(map()));
    }
    QObject::connect(map, SIGNAL(mapped(int)), this, SLOT(play(int)));

    // Compact the layout of the widgets.
    this->adjustSize();

    // Set a fixed window size.
    this->setFixedSize(this->size());
}

NineHoles::~NineHoles() {
    delete ui;
}

void NineHoles::play(int index) {
    Hole* hole = m_board[index];
    Q_ASSERT(hole != 0);

    qDebug() << QString("hole: (%1, %2)").arg(hole->row()).arg(hole->col());
    hole->setState(Hole::RedState);
}

void NineHoles::reset() {
    for (int idx = 0; idx < 9; ++idx)
        m_board[idx]->reset();
}

void NineHoles::showAbout() {
    QMessageBox::information(this, tr("Sobre"), tr("Nine Holes"));
}
