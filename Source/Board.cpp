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
    QPoint diff = source() - destination();
    QPoint absDiff = QPoint(qAbs(diff.x()), qAbs(diff.y()));
    qDebug() << "\nmoving from:" << source() << "to:" << destination()
             << " ==> absDiff:" << absDiff;

    Piece* sourceP = m_board[source().x()][source().y()];
    if(!sourceP)
        return;

    bool captureRequired = hasAnyCaptures(activePlayer());
    if(captureRequired && sourceP->type() == Enums::Man)
    {
        if(absDiff.x() != 2 || absDiff.y() != 2)
        {
            qDebug() << "Capture is mandatory!";
            emit invalidMove("Mandatory Capture Available");
            return;
        }
    }

    clearHighlights();
    bool moveOk = false;
    if(sourceP->type() == Enums::PieceType::Man && absDiff.x() == 1 && absDiff.y() == 1)
        moveOk = singleMoveValidation();
    else if(sourceP->type() == Enums::PieceType::Man && absDiff.x() == 2 && absDiff.y() == 2)
        moveOk = multipleMoveValidation();
    else if(sourceP->type() == Enums::PieceType::King)
        moveOk = kingMoveValidation();

    if(moveOk && !isMoving())
    {
        winner = checkForWinner();
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

        emit invalidMove("Invalid Move");
    }

    emit dataChanged();
    qDebug() << "current state. activeplayer:" << this->activePlayer()
             << "isMoving:" << this->isMoving()
             << "source:" << this->source()
             << "destination:" << this->destination();

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

bool Board::kingMoveValidation()
{
    QPoint src = source();
    QPoint dst = destination();
    QPoint diff = dst - src;

    Piece* sourceP = m_board[src.x()][src.y()];
    Piece* destnP = m_board[dst.x()][dst.y()];

    if(!sourceP || !destnP || sourceP->type() != Enums::PieceType::King)
        return false;

    // set active to false
    sourceP->setactive(false);

    // destination empty check
    if(destnP->player() != Enums::PlayerNone || destnP->type() != Enums::TypeNone)
        return false;

    if(qAbs(diff.x()) != qAbs(diff.y()))
        return false;  // must be diagonal

    int dx = (diff.x() > 0) ? 1 : -1;
    int dy = (diff.y() > 0) ? 1 : -1;
    QPoint dpos(dx, dy);
    QPoint xy = src + dpos;

    bool captured = false;
    QPoint cap(-1,-1);

    while(xy != dst)
    {
        if(isInBounds(xy))
        {
            Piece* mid = m_board[xy.x()][xy.y()];

            if(mid && mid->player() == sourceP->player())
                return false;  // cannot jump over own piece

            if(mid && mid->player() == opponentPlayer())
            {
                if(captured)
                    return false;  // only one capture allowed in one step
                captured = true;
                cap = xy;
            }

            if(mid && mid->player() != Enums::PlayerNone && !(captured && xy == cap))
                return false;  // blocked
        }

        xy.setX(xy.x() + dpos.x());
        xy.setY(xy.y() + dpos.y());
    }

    if(captured)
    {
        delete m_board[cap.x()][cap.y()];
        m_board[cap.x()][cap.y()] = new Piece();
        incrementScore();
    }

    // swap
    m_board[dst.x()][dst.y()] = sourceP;
    m_board[src.x()][src.y()] = destnP;

    if(captured && hasFurtherCaptures(dst))
    {
        setsource(dst);
        setisMoving(true);
    }
    else
    {
        sourceP->setactive(false);
        setisMoving(false);
    }

    return true;
}

bool Board::singleMoveValidation()
{
    QPoint src = source();
    QPoint dst = destination();
    QPoint diff = dst - src;

    Piece* sourceP = m_board[src.x()][src.y()];
    Piece* destnP = m_board[dst.x()][dst.y()];

    if(!sourceP || !destnP)
        return false;

    // set active to false
    sourceP->setactive(false);

    // destination empty check
    if(destnP->player() != Enums::PlayerNone || destnP->type() != Enums::TypeNone)
        return false;

    // allow only 1 step diagonal
    if(std::abs(diff.x()) != 1 || std::abs(diff.y()) != 1)
        return false;

    // man piece movement conditions
    if(sourceP->type() == Enums::PieceType::Man)
    {
        if(sourceP->player() == Enums::Player::PlayerA && diff.x() != 1)
            return false;  // PlayerA moves downward
        if(sourceP->player() == Enums::Player::PlayerB && diff.x() != -1)
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

    if(!sourceP || !destnP)
        return false;

    // destination empty check
    if(destnP->player() != Enums::PlayerNone || destnP->type() != Enums::TypeNone)
        return false;

    // allow only 2 step diagonal
    if(std::abs(diff.x()) != 2 || std::abs(diff.y()) != 2)
        return false;

    // midpoint must contain opponent's piece
    int midX = src.x() + diff.x() / 2;
    int midY = src.y() + diff.y() / 2;

    Piece* middlePiece = m_board[midX][midY];
    if(!middlePiece || middlePiece->player() != opponentPlayer())
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
    if(hasFurtherCaptures(dst))
    {
        setsource(dst);
        setisMoving(true);  // turn continue
        this->highlightLegalMoves(dst);
    }
    else
    {
        sourceP->setactive(false);
        setisMoving(false);  // turn ends
    }

    return true;
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

    QVector<QPoint> directions;
    if(p->type() == Enums::PieceType::King)
    {
        // all diagonal directions
        directions = { {-1,-1}, {-1,1}, {1,-1}, {1,1} };
    }
    else if(p->player() == Enums::PlayerA)
    {
        // downward only
        directions = { {1,-1}, {1,1} };
    }
    else if(p->player() == Enums::PlayerB)
    {
        // upward only
        directions = { {-1,-1}, {-1,1} };
    }

    foreach(const QPoint &d, directions)
    {
        QPoint move = pos + d;
        if(isInBounds(move))
        {
            Piece* dest = m_board[move.x()][move.y()];
            if(dest && dest->player() == Enums::PlayerNone)
                return true;  // simple move available
        }

        // Check capture
        QPoint capture = pos + d * 2;
        QPoint mid = pos + d;
        if(isInBounds(capture) && isInBounds(mid))
        {
            Piece* middle = m_board[mid.x()][mid.y()];
            Piece* dest = m_board[capture.x()][capture.y()];
            if(middle && middle->player() == opponentPlayer() &&
                dest && dest->player() == Enums::PlayerNone)
                return true;  // capture available
        }
    }

    return false;
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

void Board::highlightLegalMoves(const QPoint &pos)
{
    Piece* p = m_board[pos.x()][pos.y()];
    if(!p || p->player() != activePlayer())
        return;

    // Step 1: Check if any capture exists on board
    bool anyCaptureExists = false;
    for(int row = 0; row < BOARD_SIZE && !anyCaptureExists; ++row)
    {
        for(int col = 0; col < BOARD_SIZE && !anyCaptureExists; ++col)
        {
            Piece* checkP = m_board[row][col];
            if(checkP && checkP->player() == activePlayer())
            {
                QPoint piecePos(row, col);
                if(hasFurtherCaptures(piecePos))
                {
                    anyCaptureExists = true;
                }
            }
        }
    }

    QVector<QPoint> directions;
    if(p->type() == Enums::PieceType::King || anyCaptureExists)
    {
        directions = { {-1,-1}, {-1,1}, {1,-1}, {1,1} };
    }
    else if(p->player() == Enums::PlayerA)
    {
        directions = { {1,-1}, {1,1} };
    }
    else if(p->player() == Enums::PlayerB)
    {
        directions = { {-1,-1}, {-1,1} };
    }

    // Step 2: Highlight legal moves for current piece
    foreach(const QPoint& d, directions)
    {
        if(p->type() == Enums::PieceType::King)
        {
            // Long-range king movement and capturing
            int step = 1;
            bool enemyFound = false;
            QPoint enemyPos;

            while(true)
            {
                QPoint current = pos + d * step;
                if(!isInBounds(current))
                    break;

                Piece* target = m_board[current.x()][current.y()];

                if(!target || target->player() == Enums::PlayerNone)
                {
                    if(!anyCaptureExists && !enemyFound)
                    {
                        // Normal king move
                        m_board[current.x()][current.y()]->sethighlighted(true);
                    }
                    else if(enemyFound)
                    {
                        // Capture landing square
                        m_board[current.x()][current.y()]->sethighlighted(true);
                    }
                }
                else if(target->player() == opponentPlayer())
                {
                    if(enemyFound)
                        break; // Only one enemy allowed between start and landing
                    enemyFound = true;
                    enemyPos = current;
                }
                else // blocked
                {
                    break;
                }

                ++step;
            }
        }
        else
        {
            // Man movement and capturing (1-step diagonals)
            QPoint move = pos + d;
            QPoint jump = pos + d * 2;
            QPoint between = pos + d;

            // Capture
            if(isInBounds(jump) && isInBounds(between))
            {
                Piece* midPiece = m_board[between.x()][between.y()];
                Piece* destPiece = m_board[jump.x()][jump.y()];

                if(midPiece && midPiece->player() == opponentPlayer() &&
                    destPiece && destPiece->player() == Enums::PlayerNone)
                {
                    destPiece->sethighlighted(true);
                }
            }

            // Normal move
            if(!anyCaptureExists && isInBounds(move))
            {
                Piece* destPiece = m_board[move.x()][move.y()];
                if(destPiece && destPiece->player() == Enums::PlayerNone)
                {
                    destPiece->sethighlighted(true);
                }
            }
        }
    }
}
