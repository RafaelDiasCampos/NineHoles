#include "NineHoles.h"
#include "ui_NineHoles.h"

#include <QMessageBox>
#include <QSignalMapper>

#include <QDebug>

NineHoles::NineHoles(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::NineHoles),
      selectedPiece(NULL),
      currentPlayer(Player::Red),
      gameState(GameState::Place),
      placedPieces(0) {

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

    updateGameText();
}

NineHoles::~NineHoles() {
    delete ui;
}

void NineHoles::play(int index) {
    Hole* hole = m_board[index];
    Q_ASSERT(hole != 0);

    switch (gameState) {
        case GameState::Place:
            this->placePiece(*hole);
            break;
        case GameState::Move:
            this->movePiece(*hole);
            break;
    }
}

void NineHoles::reset() {
    for (int idx = 0; idx < 9; ++idx)
        m_board[idx]->reset();

    gameState = GameState::Place;
    currentPlayer = Player::Red;
    placedPieces = 0;

    updateGameText();
}

void NineHoles::showAbout() {
    QMessageBox::information(this, tr("Sobre"), tr("Nine Holes"));
}

void NineHoles::showError(const char* message) {
    QMessageBox::warning(this, tr("Erro"), tr(message));
}

void NineHoles::updateGameText() {
    auto textBox = ui->gameText;

    QString faseAtual = gameState == GameState::Place ? "colocar" : "mover";
    QString jogadorAtual = currentPlayer == Player::Red ? "vermelho" : "azul";

    QString text = QString("Fase de %1: vez do jogador %2.").arg(faseAtual).arg(jogadorAtual);

    textBox->setText(text);
}

Hole::State NineHoles::getHoleStateFromPlayer(Player& player) {
    switch (player) {
        case Player::Red:  return Hole::RedState;
        case Player::Blue:
        default:           return Hole::BlueState;
    }
}

void NineHoles::changeCurrentPlayer() {
    switch (currentPlayer) {
        case Player::Red:  currentPlayer = Player::Blue; break;
        case Player::Blue: currentPlayer = Player::Red;  break;
    }
    updateGameText();
}

void NineHoles::setSelectableHoles(Hole& hole, bool selectable) {
    Hole *left = NULL,
         *right = NULL,
         *up = NULL,
         *down = NULL;

    //If selectable is true, set hole to be selectable. Otherwise, set hole as empty
    Hole::State setState = selectable ? Hole::SelectableState : Hole::EmptyState;

    int holeRow = hole.row();
    int holeCol = hole.col();

    //Finds up and down holes
    switch (holeRow) {
        case 0:
            down = m_board[(3 * (holeRow + 1)) + holeCol];
            break;
        case 1:
            up = m_board[(3 * (holeRow - 1)) + holeCol];
            down = m_board[(3 * (holeRow + 1)) + holeCol];
            break;
        case 2:
            up = m_board[(3 * (holeRow - 1)) + holeCol];
            break;
    }

    //Finds left and right holes
    switch (holeCol) {
        case 0:
            right = m_board[(3 * holeRow) + (holeCol + 1)];
            break;
        case 1:
            left = m_board[(3 * holeRow) + (holeCol - 1)];
            right = m_board[(3 * holeRow) + (holeCol + 1)];
            break;
        case 2:
            left = m_board[(3 * holeRow) + (holeCol - 1)];
            break;
    }

    //Sets the state for each hole if they exist and if they are either empty or selectable
    if (left && ( left->state() == Hole::EmptyState || left->state() == Hole::SelectableState) ){
        left->setState(setState);
    }
    if (right && ( right->state() == Hole::EmptyState || right->state() == Hole::SelectableState) ){
        right->setState(setState);
    }
    if (up && ( up->state() == Hole::EmptyState || up->state() == Hole::SelectableState) ){
        up->setState(setState);
    }
    if (down && ( down->state() == Hole::EmptyState || down->state() == Hole::SelectableState) ){
        down->setState(setState);
    }

}

bool NineHoles::checkGameWon() {
    Player playerWon;
    bool gameWon = false;

    //Checks for horizontal lines
    for(int row = 0; row < 3; row++) {
        if (m_board[(3*row) + 0]->state() != Hole::EmptyState &&
            m_board[(3*row) + 0]->state() == m_board[(3*row) + 1]->state() &&
            m_board[(3*row) + 0]->state() == m_board[(3*row) + 2]->state()) {

            playerWon = m_board[(3*row) + 0]->state() == Hole::RedState ? Player::Red : Player::Blue;
            gameWon = true;
        }
    }

    //Checks for veritcal lines
    for(int col = 0; col < 3; col++) {
        if (m_board[(3 * 0) + col]->state() != Hole::EmptyState &&
            m_board[(3 * 0) + col]->state() == m_board[(3 * 1) + col]->state() &&
            m_board[(3 * 0) + col]->state() == m_board[(3 * 2) + col]->state()) {

            playerWon = m_board[(3 * 0) + col]->state() == Hole::RedState ? Player::Red : Player::Blue;
            gameWon = true;
        }
    }

    if (gameWon) {
        QString wonMessage = QString("Parabéns, o jogador %1 venceu.").arg(playerWon == Player::Red ? "vermelho" : "azul");
        QMessageBox::information(this, tr("Vencedor"), tr(wonMessage.toStdString().c_str()));
        reset();
    }

    return gameWon;

}

void NineHoles::placePiece(Hole& hole) {
    Hole::State newState = getHoleStateFromPlayer(currentPlayer);

    switch (hole.state()) {
        case Hole::BlueState:
        case Hole::RedState:
            showError("Já existe uma peça nessa casa.");
            break;
        case Hole::EmptyState:
            hole.setState(newState);
            placedPieces++;
            if (placedPieces == 6) {
                gameState = GameState::Move;
            }
            if (checkGameWon()) return;
            changeCurrentPlayer();
            break;
        default:
            qDebug() << QString("Impossible state");
    }

}

void NineHoles::movePiece(Hole& moveHole) {

    //Checks if there's no selected piece or the player is selecting a new piece
    if (!selectedPiece || moveHole.state() != Hole::SelectableState) {
        //Checks if player owns the piece
        if (moveHole.state() != getHoleStateFromPlayer(currentPlayer)) {
            showError("Comando de movimento inválido.");
            return;
        }

        //Cleans selectable holes
        if (selectedPiece) {
            setSelectableHoles(*selectedPiece, false);
        }

        //Sets the piece as selected and marks selectable holes
        selectedPiece = &moveHole;
        setSelectableHoles(*selectedPiece, true);
        return;
    }

    //Cleans selectable holes
    setSelectableHoles(*selectedPiece, false);

    //Moves piece to new hole
    moveHole.setState(selectedPiece->state());
    selectedPiece->setState(Hole::EmptyState);

    //Cleans selected piece
    selectedPiece = NULL;

    if (checkGameWon()) return;
    changeCurrentPlayer();
    return;
}
