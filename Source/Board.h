#ifndef BOARD_H
#define BOARD_H

#include <QObject>
#include <QVector>
#include <QVariantMap>

#include "Enums.h"
#include "PropertyHelper.h"

struct MoveResult {
    bool isValid;
    bool isCapture;
    QPoint capturedPiecePos;
    bool hasMoreCaptures;
    QString errorMessage;

    MoveResult() : isValid(false), isCapture(false), capturedPiecePos(-1,-1), hasMoreCaptures(false) {}
};

struct MoveCandidate {
    QPoint from;
    QPoint to;
    bool isCapture;
    QPoint capturedPos;

    MoveCandidate(QPoint f, QPoint t, bool cap = false, QPoint capPos = QPoint(-1,-1))
        : from(f), to(t), isCapture(cap), capturedPos(capPos) {}
};


class Piece;
/**
 * @brief The Board class - depicts the 2D board and
 * contains the game logics & move validations
 */
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

signals:
    void dataChanged();
    void invalidMove(QString msg);
    void gameOver(Enums::Player winner);
    void gameDraw();

private:
    void initialize();
    void changePlayers();
    bool hasAnyCaptures(Enums::Player player);
    bool hasFurtherCaptures(const QPoint &pos);
    void checkAndPromoteKing(const QPoint &pos);
    Enums::Player checkForWinner();
    bool canPieceMove(const QPoint &pos);
    bool canPlayerMove(Enums::Player player);
    bool isInBounds(const QPoint &pos);
    bool isDraw();
    void deleteAll();
    void incrementScore();
    void registerQmlTypes();
    void highlightLegalMoves(const QPoint &pos);
    void clearHighlights();

    // Unified method to get all legal moves for a piece
    QVector<MoveCandidate> getLegalMoves(const QPoint &pos);
    QVector<MoveCandidate> getManMoves(const QPoint &pos, bool mustCapture);
    QVector<MoveCandidate> getKingMoves(const QPoint &pos, bool mustCapture);
    MoveResult validateAndExecuteMove(const QPoint &from, const QPoint &to);

private:
    // 2D board representation
    QVector<QVector<Piece*> > m_board;

};

#endif // BOARD_H
