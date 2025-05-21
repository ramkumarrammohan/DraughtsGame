#ifndef PIECE_H
#define PIECE_H

#include <QObject>
#include <QVariantMap>

#include "Enums.h"
#include "PropertyHelper.h"

class Piece : public QObject
{
    Q_OBJECT

    AUTO_PROPERTY(bool, active, false)
    AUTO_PROPERTY(bool, highlighted, false)
    AUTO_PROPERTY(Enums::Player, player, Enums::Player::PlayerNone)
    AUTO_PROPERTY(Enums::PieceType, type, Enums::PieceType::TypeNone)

public:
    explicit Piece(Enums::Player player=Enums::Player::PlayerNone,
                   Enums::PieceType type=Enums::PieceType::TypeNone,
                   QObject *parent = nullptr);
    ~Piece() override;

    QVariantMap toVariantMap();
};

#endif // PIECE_H
