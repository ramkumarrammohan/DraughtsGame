#include "Piece.h"

#include <QDebug>

Piece::Piece(Enums::Player player, Enums::PieceType type, QObject *parent)
    :QObject(parent),
    m_player(player),
    m_type(type)
{
}

Piece::~Piece()
{
}

void Piece::updateFromVariantMap(const QVariantMap &map)
{
    if(map.contains("type"))
        this->settype(Enums::PieceType(map["type"].toInt()));
    if(map.contains("player"))
        this->setplayer(Enums::Player(map["player"].toInt()));
    if(map.contains("active"))
        this->setactive(map["active"].toBool());
}

QVariantMap Piece::toVariantMap()
{
    QVariantMap map;
    map["type"] = this->type();
    map["player"] = this->player();
    map["active"] = this->active();

    return map;
}
