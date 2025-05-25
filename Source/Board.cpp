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
    if(source() == QPoint(-1,-1) || destination() == QPoint(-1,-1) ||
        source() == destination())
    {
        qDebug() << "no valid movement found";
        emit invalidMove("Invalid source & destination");
        return;
    }

    Enums::Player winner = Enums::PlayerNone;
    Piece* sourceP = m_board[source().x()][source().y()];
    if(!sourceP)
        return;

    // Check if capture is mandatory but player is not capturing
    bool captureRequired = hasAnyCaptures(activePlayer());
    if(captureRequired && !hasFurtherCaptures(source()))
    {
        qDebug() << "Capture is mandatory!";
        emit invalidMove("Mandatory Capture Available");
        return;
    }

    sourceP->setactive(false);
    clearHighlights();

    MoveResult result = validateAndExecuteMove(source(), destination());

    if(!result.isValid) {
        setisMoving(false);
        setsource(QPoint(-1, -1));
        setdestination(QPoint(-1, -1));
        emit invalidMove(result.errorMessage);
        emit dataChanged();
        return;
    }

    // Handle successful move
    if(result.hasMoreCaptures) {
        // Continue capturing sequence
        setsource(destination());
        setisMoving(true);
        sourceP->setactive(true);
        highlightLegalMoves(destination());
    } else {
        // End turn
        winner = checkForWinner();
        sourceP->setactive(false);
        setisMoving(false);
        changePlayers();
        setsource(QPoint(-1, -1));
        setdestination(QPoint(-1, -1));
        setmoveCount(moveCount() + 1);
    }

    emit dataChanged();

    // Check for game end conditions
    if(winner != Enums::PlayerNone) {
        qDebug() << "Gameover. winner:" << winner;
        emit gameOver(winner);
        return;
    }

    if(isDraw()) {
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

    // checking for mandatory captures
    if(hasAnyCaptures(activePlayer()) && !hasFurtherCaptures(QPoint(row, column)))
    {
        emit invalidMove("Mandatory Capture Available");
        return;
    }

    piece->setactive(true);
    this->setisMoving(true);
    this->setsource(QPoint(row, column));
    this->highlightLegalMoves(QPoint(row, column));
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

bool Board::hasAnyCaptures(Enums::Player player)
{
    for(int row = 0; row < BOARD_SIZE; ++row)
    {
        for(int col = 0; col < BOARD_SIZE; ++col)
        {
            Piece* piece = m_board[row][col];
            if(piece && piece->player() == player)
            {
                QPoint pos(row, col);
                if(hasFurtherCaptures(pos))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Board::hasFurtherCaptures(const QPoint &pos)
{
    Piece* piece = m_board[pos.x()][pos.y()];
    if(!piece || piece->player() == Enums::PlayerNone)
        return false;

    QVector<QPoint> directions = { {1, 1}, {-1, -1}, {1, -1}, {-1, 1} };
    Enums::Player player = piece->player();

    if(piece->type() == Enums::PieceType::Man)
    {
        // Standard 2-square jump check for Man
        foreach(auto& dir, directions)
        {
            QPoint xy = pos + dir;
            QPoint cxy = pos + (dir * 2);

            if(isInBounds(cxy))
            {
                Piece* mid = m_board[xy.x()][xy.y()];
                Piece* landing = m_board[cxy.x()][cxy.y()];

                if(mid && mid->player() != player && mid->player() != Enums::PlayerNone &&
                    landing && landing->player() == Enums::PlayerNone)
                {
                    return true;
                }
            }
        }
    }
    else if(piece->type() == Enums::PieceType::King)
    {
        // Flying capture for King
        foreach(auto& dir, directions)
        {
            QPoint xy = pos + dir;
            bool foundOpponent = false;

            while (isInBounds(xy))
            {
                Piece* current = m_board[xy.x()][xy.y()];

                if(!current || current->player() == Enums::PlayerNone)
                {
                    if(foundOpponent)
                        return true; // landing spot after opponent
                }
                else if(current->player() == player)
                {
                    break; // blocked by own piece
                }
                else
                {
                    if(foundOpponent)
                        break; // cannot capture more than one piece in a line
                    foundOpponent = true;
                }

                xy += dir;
            }
        }
    }

    return false;
}

void Board::checkAndPromoteKing(const QPoint &pos)
{
    Piece* p = m_board[pos.x()][pos.y()];
    if(!p || p->type() != Enums::PieceType::Man)
        return;

    if((p->player() == Enums::PlayerA && pos.x() == BOARD_SIZE - 1) ||
        (p->player() == Enums::PlayerB && pos.x() == 0))
    {
        p->settype(Enums::PieceType::King);
    }
}

Enums::Player Board::checkForWinner()
{
    bool playerAHasMoves = false;
    bool playerBHasMoves = false;

    for(int row = 0; row < BOARD_SIZE; ++row)
    {
        for(int col = 0; col < BOARD_SIZE; ++col)
        {
            Piece* p = m_board[row][col];
            if(!p || p->player() == Enums::PlayerNone)
                continue;

            // Check if piece can move
            QPoint pos(row, col);
            if(canPieceMove(pos))
            {
                if(p->player() == Enums::PlayerA)
                    playerAHasMoves = true;
                else if(p->player() == Enums::PlayerB)
                    playerBHasMoves = true;
            }
        }
    }

    if(!playerAHasMoves && playerBHasMoves && activePlayer() == Enums::PlayerB)
        return Enums::PlayerB;
    if(!playerBHasMoves && playerAHasMoves && activePlayer() == Enums::PlayerA)
        return Enums::PlayerA;

    return Enums::PlayerNone;  // no winner yet or draw
}

bool Board::canPieceMove(const QPoint &pos)
{
    Piece* p = m_board[pos.x()][pos.y()];
    if(!p || p->player() == Enums::PlayerNone)
        return false;

    // Use the unified legal moves system
    QVector<MoveCandidate> moves = getLegalMoves(pos);
    return !moves.isEmpty();
}

bool Board::canPlayerMove(Enums::Player player)
{
    for(int row = 0; row < BOARD_SIZE; ++row)
    {
        for(int col = 0; col < BOARD_SIZE; ++col)
        {
            Piece* p = m_board[row][col];
            if(!p || p->player() != player)
                continue;

            QPoint pos(row, col);
            if(canPieceMove(pos))
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
    for(int row = 0; row < BOARD_SIZE; ++row)
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

void Board::clearHighlights()
{
    for(int i = 0; i < BOARD_SIZE; ++i)
    {
        for(int j = 0; j < BOARD_SIZE; ++j)
        {
            if(m_board[i][j])
                m_board[i][j]->sethighlighted(false);
        }
    }
}

QVector<MoveCandidate> Board::getLegalMoves(const QPoint &pos)
{
    QVector<MoveCandidate> moves;
    Piece* piece = m_board[pos.x()][pos.y()];

    if(!piece || piece->player() == Enums::PlayerNone)
        return moves;

    bool mustCapture = hasAnyCaptures(piece->player());

    if(piece->type() == Enums::PieceType::King)
        moves = getKingMoves(pos, mustCapture);
    else
        moves = getManMoves(pos, mustCapture);

    return moves;
}

QVector<MoveCandidate> Board::getManMoves(const QPoint &pos, bool mustCapture)
{
    QVector<MoveCandidate> moves;
    Piece* piece = m_board[pos.x()][pos.y()];

    QVector<QPoint> directions;
    if(piece->player() == Enums::PlayerA)
    {
        directions = { {1, -1}, {1, 1} }; // downward
    }
    else
    {
        directions = { {-1, -1}, {-1, 1} }; // upward
    }

    // During multi-capture sequence, allow all directions
    if(mustCapture && source() == pos)
    {
        directions = { {-1, -1}, {-1, 1}, {1, -1}, {1, 1} };
    }

    foreach(const QPoint& dir, directions)
    {
        // Check for captures (2-step moves)
        QPoint jumpPos = pos + dir * 2;
        QPoint midPos = pos + dir;

        if(isInBounds(jumpPos) && isInBounds(midPos))
        {
            Piece* midPiece = m_board[midPos.x()][midPos.y()];
            Piece* jumpPiece = m_board[jumpPos.x()][jumpPos.y()];

            if(midPiece && midPiece->player() == opponentPlayer() &&
                jumpPiece && jumpPiece->player() == Enums::PlayerNone)
            {
                moves.append(MoveCandidate(pos, jumpPos, true, midPos));
            }
        }

        // Check for simple moves (1-step) only if no captures are mandatory
        if(!mustCapture)
        {
            QPoint movePos = pos + dir;
            if(isInBounds(movePos))
            {
                Piece* movePiece = m_board[movePos.x()][movePos.y()];
                if(movePiece && movePiece->player() == Enums::PlayerNone)
                {
                    moves.append(MoveCandidate(pos, movePos, false));
                }
            }
        }
    }

    return moves;
}

QVector<MoveCandidate> Board::getKingMoves(const QPoint &pos, bool mustCapture)
{
    QVector<MoveCandidate> moves;
    QVector<QPoint> directions = { {-1, -1}, {-1, 1}, {1, -1}, {1, 1} };

    for (const QPoint& dir : directions)
    {
        int step = 1;
        QPoint enemyPos(-1, -1);
        bool foundEnemy = false;

        while (true)
        {
            QPoint current = pos + dir * step;
            if(!isInBounds(current))
                break;

            Piece* currentPiece = m_board[current.x()][current.y()];

            if(!currentPiece || currentPiece->player() == Enums::PlayerNone)
            {
                // Empty square
                if(!mustCapture && !foundEnemy)
                {
                    // Normal move
                    moves.append(MoveCandidate(pos, current, false));
                }
                else if(foundEnemy)
                {
                    // Landing square after capture
                    moves.append(MoveCandidate(pos, current, true, enemyPos));
                }
            } else if(currentPiece->player() == opponentPlayer())
            {
                if(foundEnemy)
                    break; // Can't jump over multiple enemies

                foundEnemy = true;
                enemyPos = current;
            }
            else
            {
                break; // Own piece - blocked
            }

            step++;
        }
    }

    return moves;
}

MoveResult Board::validateAndExecuteMove(const QPoint &from, const QPoint &to)
{
    MoveResult result;

    QVector<MoveCandidate> legalMoves = getLegalMoves(from);
    MoveCandidate* validMove = nullptr;

    // Find if the requested move is in our legal moves
    for (auto& move : legalMoves)
    {
        if(move.to == to)
        {
            validMove = &move;
            break;
        }
    }

    if(!validMove)
    {
        result.errorMessage = "Invalid Move";
        return result;
    }

    // Execute the move
    result.isValid = true;
    result.isCapture = validMove->isCapture;
    result.capturedPiecePos = validMove->capturedPos;

    Piece* sourcePiece = m_board[from.x()][from.y()];
    Piece* destPiece = m_board[to.x()][to.y()];

    // Handle capture
    if(validMove->isCapture)
    {
        delete m_board[validMove->capturedPos.x()][validMove->capturedPos.y()];
        m_board[validMove->capturedPos.x()][validMove->capturedPos.y()] = new Piece();
        incrementScore();
    }

    // Move the piece
    m_board[to.x()][to.y()] = sourcePiece;
    m_board[from.x()][from.y()] = destPiece;

    // Check for king promotion
    checkAndPromoteKing(to);

    // Check for further captures
    if(validMove->isCapture)
    {
        result.hasMoreCaptures = hasFurtherCaptures(to);
    }

    return result;
}

void Board::highlightLegalMoves(const QPoint &pos)
{
    Piece* p = m_board[pos.x()][pos.y()];
    if(!p || p->player() != activePlayer())
        return;

    QVector<MoveCandidate> legalMoves = getLegalMoves(pos);

    foreach(const auto& move, legalMoves)
    {
        m_board[move.to.x()][move.to.y()]->sethighlighted(true);
    }
}

