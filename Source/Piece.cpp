#include "Piece.h"

Piece::Piece(Enums::Player player, Enums::PieceType type, QObject *parent)
    :QObject(parent),
    m_player(player),
    m_type(type)
{
}

Piece::~Piece()
{
}

QVariantMap Piece::toVariantMap()
{
    QVariantMap map;
    map["type"] = this->type();
    map["player"] = this->player();
    map["active"] = this->active();

    return map;
}
