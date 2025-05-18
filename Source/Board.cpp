#include "Board.h"
#include "Piece.h"
#include "Enums.h"

#include <QDebug>
#include <QQmlEngine>

#define BOARD_SIZE 10

Board::Board(QObject *parent)
    : QObject{parent}
{
    initialize();
    registerQmlTypes();
}

Board::~Board()
{
    this->deleteAll();
}

void Board::move()
{
    if(source() == QPoint(-1,-1) || destination() == QPoint(-1,-1))
    {
        qDebug() << "no valid movement found";
        return;
    }

    QPoint diff = source() - destination();
    QPoint absDiff = QPoint(qAbs(diff.x()), qAbs(diff.y()));
    qDebug() << "\nmoving from:" << source() << "to:" << destination()
             << " ==> absDiff:" << absDiff;

    bool moveOk = false;
    if(absDiff.x() == 1 && absDiff.y() == 1)
    {
        moveOk = singleMoveValidation();
    }
    else if(absDiff.x() == 2 && absDiff.y() == 2)
    {
        moveOk = multipleMoveValidation();
    }

    if(moveOk && !isMoving())
    {
        this->changePlayers();
        setsource(QPoint(-1, -1));
        setdestination(QPoint(-1, -1));
        setmoveCount(moveCount() + 1);
    }
    else if(!moveOk)
    {
        setisMoving(false);
        Piece *p = m_board[source().x()][source().y()];
        p->setactive(false);

        setsource(QPoint(-1, -1));
        setdestination(QPoint(-1, -1));

        emit invalidMove();
    }

    emit dataChanged();
    qDebug() << "current state. activeplayer:" << this->activePlayer()
             << "isMoving:" << this->isMoving()
             << "source:" << this->source()
             << "destination:" << this->destination();

    Enums::Player winner = checkForWinner();
    if(winner != Enums::PlayerNone)
    {
        qDebug() << "Gameover. winner:" << winner;
        emit gameOver(winner);
        return;
    }

    if(isDraw())
    {
        qDebug() << "Game draw.. No movements further";
        emit gameDraw();
        return;
    }
}

void Board::reset()
{
    deleteAll();
    initialize();
    setisMoving(false);
    setmoveCount(0);
    setplayerAScore(0);
    setplayerBScore(0);
    setsource(QPoint(-1,-1));
    setdestination(QPoint(-1,-1));
    setactivePlayer(Enums::PlayerA);
    setopponentPlayer(Enums::PlayerB);


    emit dataChanged();
}

int Board::rowCount()
{
    return BOARD_SIZE;
}

int Board::columnCount()
{
    return BOARD_SIZE;
}

QVariantMap Board::item(const int &row, const int &column)
{
    if(row < 0 || row >= BOARD_SIZE || column < 0 || column >= BOARD_SIZE)
        return QVariantMap();

    Piece *p = m_board[row][column];
    if(p)
    {
        QVariantMap map = p->toVariantMap();
        return map;
    }

    return QVariantMap();
}

void Board::setActiveItem(const int &row, const int &column)
{
    if(row < 0 || row >= BOARD_SIZE || column < 0 || column >= BOARD_SIZE)
        return;

    Piece *piece = m_board[row][column];
    if(!piece)
        return;

    piece->setactive(true);
    emit dataChanged();
}

void Board::initialize()
{
    // populates all the pieces
    m_board.resize(BOARD_SIZE);
    for(int i = 0; i < BOARD_SIZE; i++)
    {
        m_board[i].resize(BOARD_SIZE);
    }

    for(int i = 0; i < BOARD_SIZE; i++)
    {
        for(int j = 0; j < BOARD_SIZE; j++)
        {
            Piece *p = new Piece;

            if(i < 4 || i > 5)
            {
                if(((i % 2 == 0) && (j % 2 != 0)) || ((i % 2 != 0) && (j % 2 == 0)))
                {
                    p->settype(Enums::PieceType::Man);
                    if(i < 4)
                        p->setplayer(Enums::Player::PlayerA);
                    else if(i > 5)
                        p->setplayer(Enums::Player::PlayerB);
                }
            }

            m_board[i][j] = p;
        }
    }
}

void Board::changePlayers()
{
    if(activePlayer() == Enums::PlayerA)
    {
        setactivePlayer(Enums::Player::PlayerB);
        setopponentPlayer(Enums::Player::PlayerA);
    }
    else
    {
        setactivePlayer(Enums::Player::PlayerA);
        setopponentPlayer(Enums::Player::PlayerB);
    }
}

bool Board::singleMoveValidation()
{
    QPoint src = source();
    QPoint dst = destination();
    QPoint diff = dst - src;

    Piece* sourceP = m_board[src.x()][src.y()];
    Piece* destnP = m_board[dst.x()][dst.y()];

    if (!sourceP || !destnP)
        return false;

    // set active to false
    sourceP->setactive(false);

    // destination empty check
    if (destnP->player() != Enums::PlayerNone || destnP->type() != Enums::TypeNone)
        return false;

    // allow only 1 step diagonal
    if (std::abs(diff.x()) != 1 || std::abs(diff.y()) != 1)
        return false;

    // man piece movement conditions
    if (sourceP->type() == Enums::PieceType::Man)
    {
        if (sourceP->player() == Enums::Player::PlayerA && diff.x() != 1)
            return false;  // PlayerA moves downward
        if (sourceP->player() == Enums::Player::PlayerB && diff.x() != -1)
            return false;  // PlayerB moves upward
    }

    // swap the pieces
    m_board[dst.x()][dst.y()] = sourceP;
    m_board[src.x()][src.y()] = destnP;

    // extra validations
    setisMoving(false);
    checkAndPromoteKing(dst);

    return true;
}

bool Board::multipleMoveValidation()
{
    QPoint src = source();
    QPoint dst = destination();
    QPoint diff = dst - src;

    Piece* sourceP = m_board[src.x()][src.y()];
    Piece* destnP = m_board[dst.x()][dst.y()];

    if (!sourceP || !destnP)
        return false;

    // destination empty check
    if (destnP->player() != Enums::PlayerNone || destnP->type() != Enums::TypeNone)
        return false;

    // allow only 2 step diagonal
    if (std::abs(diff.x()) != 2 || std::abs(diff.y()) != 2)
        return false;

    // man piece movement conditions
    if (sourceP->type() == Enums::PieceType::Man)
    {
        if (sourceP->player() == Enums::Player::PlayerA && diff.x() != 2)
            return false;  // PlayerA moves downward (increasing x)
        if (sourceP->player() == Enums::Player::PlayerB && diff.x() != -2)
            return false;  // PlayerB moves upward (decreasing x)
    }

    // midpoint must contain opponent's piece
    int midX = src.x() + diff.x() / 2;
    int midY = src.y() + diff.y() / 2;

    Piece* middlePiece = m_board[midX][midY];
    if (!middlePiece || middlePiece->player() != opponentPlayer())
        return false;

    // perform the capture
    delete m_board[midX][midY];
    m_board[midX][midY] = new Piece();
    incrementScore();

    // swap the pieces
    m_board[dst.x()][dst.y()] = sourceP;
    m_board[src.x()][src.y()] = destnP;
    checkAndPromoteKing(dst);

    // check for continuation
    if (hasFurtherCaptures(dst))
    {
        setsource(dst);
        setisMoving(true);  // turn continue
    }
    else
    {
        sourceP->setactive(false);
        setisMoving(false);  // turn ends
    }

    return true;
}

bool Board::hasFurtherCaptures(const QPoint &pos)
{
    Piece* p = m_board[pos.x()][pos.y()];
    if (!p || p->player() == Enums::PlayerNone)
        return false;

    // allowed directions
    QVector<QPoint> directions;
    if (p->type() == Enums::PieceType::King)
    {
        directions = {QPoint(-2, -2), QPoint(-2, 2), QPoint(2, -2), QPoint(2, 2)};
    }
    else
    {
        if (p->player() == Enums::Player::PlayerA)
            directions = {QPoint(2, -2), QPoint(2, 2)};
        else if (p->player() == Enums::Player::PlayerB)
            directions = {QPoint(-2, -2), QPoint(-2, 2)};
    }

    // check for capture existance in all directions
    foreach(const QPoint &d, directions)
    {
        int newX = pos.x() + d.x();
        int newY = pos.y() + d.y();
        int midX = pos.x() + d.x() / 2;
        int midY = pos.y() + d.y() / 2;

        // non boundary check
        if (newX >= 0 && newX < BOARD_SIZE && newY >= 0 && newY < BOARD_SIZE)
        {
            Piece* landing = m_board[newX][newY];
            Piece* mid = m_board[midX][midY];

            if (landing && mid && landing->player() == Enums::PlayerNone &&
                mid->player() == opponentPlayer())
            {
                return true;
            }
        }
    }

    return false;
}

void Board::checkAndPromoteKing(const QPoint &pos)
{
    Piece* p = m_board[pos.x()][pos.y()];
    if (!p || p->type() != Enums::PieceType::Man)
        return;

    if ((p->player() == Enums::PlayerA && pos.x() == BOARD_SIZE - 1) ||
        (p->player() == Enums::PlayerB && pos.x() == 0))
    {
        p->settype(Enums::PieceType::King);
    }
}

Enums::Player Board::checkForWinner()
{
    bool playerAHasMoves = false;
    bool playerBHasMoves = false;

    for (int row = 0; row < BOARD_SIZE; ++row)
    {
        for (int col = 0; col < BOARD_SIZE; ++col)
        {
            Piece* p = m_board[row][col];
            if (!p || p->player() == Enums::PlayerNone)
                continue;

            // Check if piece can move
            QPoint pos(row, col);
            if (canPieceMove(pos))
            {
                if (p->player() == Enums::PlayerA)
                    playerAHasMoves = true;
                else if (p->player() == Enums::PlayerB)
                    playerBHasMoves = true;
            }
        }
    }

    if (!playerAHasMoves && playerBHasMoves)
        return Enums::PlayerB;
    if (!playerBHasMoves && playerAHasMoves)
        return Enums::PlayerA;

    return Enums::PlayerNone;  // no winner yet or draw
}

bool Board::canPieceMove(const QPoint &pos)
{
    Piece* p = m_board[pos.x()][pos.y()];
    if (!p || p->player() == Enums::PlayerNone)
        return false;

    QVector<QPoint> directions;
    if (p->type() == Enums::PieceType::King)
    {
        // all diagonal directions
        directions = { {-1,-1}, {-1,1}, {1,-1}, {1,1} };
    }
    else if (p->player() == Enums::PlayerA)
    {
        // downward only
        directions = { {1,-1}, {1,1} };
    }
    else if (p->player() == Enums::PlayerB)
    {
        // upward only
        directions = { {-1,-1}, {-1,1} };
    }

    foreach(const QPoint &d, directions)
    {
        QPoint move = pos + d;
        if (isInBounds(move))
        {
            Piece* dest = m_board[move.x()][move.y()];
            if (dest && dest->player() == Enums::PlayerNone)
                return true;  // simple move available
        }

        // Check capture
        QPoint capture = pos + d * 2;
        QPoint mid = pos + d;
        if (isInBounds(capture) && isInBounds(mid))
        {
            Piece* middle = m_board[mid.x()][mid.y()];
            Piece* dest = m_board[capture.x()][capture.y()];
            if (middle && middle->player() == opponentPlayer() &&
                dest && dest->player() == Enums::PlayerNone)
                return true;  // capture available
        }
    }

    return false;
}

bool Board::canPlayerMove(Enums::Player player)
{
    for (int row = 0; row < BOARD_SIZE; ++row)
    {
        for (int col = 0; col < BOARD_SIZE; ++col)
        {
            Piece* p = m_board[row][col];
            if (!p || p->player() != player)
                continue;

            QPoint pos(row, col);
            if (canPieceMove(pos))
                return true;
        }
    }

    return false;
}

bool Board::isInBounds(const QPoint &pos)
{
    return pos.x() >= 0 && pos.y() >= 0 &&
           pos.x() < BOARD_SIZE && pos.y() < BOARD_SIZE;
}

bool Board::isDraw()
{
    return !canPlayerMove(Enums::PlayerA) && !canPlayerMove(Enums::PlayerB);
}

void Board::deleteAll()
{
    for (int row = 0; row < BOARD_SIZE; ++row)
    {
        qDeleteAll(m_board[row]);
    }
}

void Board::incrementScore()
{
    if(activePlayer() == Enums::PlayerA)
        setplayerAScore(playerAScore() + 1);
    else if(activePlayer() == Enums::PlayerB)
        setplayerBScore(playerBScore() + 1);
}

void Board::registerQmlTypes()
{
    qmlRegisterType<Piece>("com.checkerboard", 1, 0,"Piece");
    qmlRegisterSingletonType(QUrl("qrc:/Qml/Theme.qml"),
                             "com.checkerboard.Theme", 1, 0, "Theme");
    qmlRegisterUncreatableType<Enums>("com.checkerboard.Enums", 1, 0, "Enums",
                                      "Enums are read only");
}
