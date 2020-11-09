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
    enum Player {
        Red,
        Blue
    };
    enum GameState {
        Place,
        Move
    };

    NineHoles(QWidget *parent = nullptr);
    virtual ~NineHoles();

private:
    Ui::NineHoles *ui;
    Hole* m_board[9];
    Hole* selectedPiece;
    Player currentPlayer;
    GameState gameState;
    int placedPieces;

    void placePiece(Hole& hole);
    void movePiece(Hole& moveHole);

    void updateGameText();
    void setSelectableHoles(Hole& hole, bool selectable);
    Hole::State getHoleStateFromPlayer(Player& player);
    void changeCurrentPlayer();
    bool checkGameWon();

private slots:
    void play(int index);
    void reset();
    void showAbout();
    void showError(const char *message);

};

#endif // NINEHOLES_H
