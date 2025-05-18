#ifndef BOARD_H
#define BOARD_H

#include <QObject>
#include <QVector>
#include <QVariantMap>

#include "PropertyHelper.h"
#include "Enums.h"

class Piece;
class Board : public QObject
{
    Q_OBJECT

    AUTO_PROPERTY(bool, isMoving, false)
    AUTO_PROPERTY(int, moveCount, 0)
    AUTO_PROPERTY(int, playerAScore, 0)
    AUTO_PROPERTY(int, playerBScore, 0)
    AUTO_PROPERTY(QPoint, source, QPoint(-1,-1))
    AUTO_PROPERTY(QPoint, destination, QPoint(-1,-1))
    AUTO_PROPERTY(Enums::Player, activePlayer, Enums::Player::PlayerA)
    AUTO_PROPERTY(Enums::Player, opponentPlayer, Enums::Player::PlayerB)

public:
    explicit Board(QObject *parent = nullptr);
    ~Board() override;

    Q_INVOKABLE void move();
    Q_INVOKABLE void reset();
    Q_INVOKABLE int rowCount();
    Q_INVOKABLE int columnCount();
    Q_INVOKABLE QVariantMap item(const int &row, const int &column);
    Q_INVOKABLE void setActiveItem(const int &row, const int &column);

    // utility function - can be removed later
    Q_INVOKABLE void print(const int &row, const int &column);
    Q_INVOKABLE void makeKing(const int &row, const int &column);

signals:
    void dataChanged();
    void invalidMove();
    void gameOver(Enums::Player winner);
    void gameDraw();

private:
    void initialize();
    void changePlayers();
    bool singleMoveValidation();
    bool multipleMoveValidation();
    bool hasFurtherCaptures(const QPoint &pos);
    void checkAndPromoteKing(const QPoint &pos);
    Enums::Player checkForWinner();
    bool canPieceMove(const QPoint &pos);
    bool canPlayerMove(Enums::Player player);
    bool isInBounds(const QPoint &pos);
    bool isDraw();
    void deleteAll();
    void incrementScore();

private:
    QVector<QVector<Piece*> > m_board;

};

#endif // BOARD_H
