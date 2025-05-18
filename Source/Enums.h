#ifndef ENUMS_H
#define ENUMS_H

#include <QObject>

class Enums : public QObject
{
    Q_OBJECT

public:
    enum Player {
        PlayerNone,
        PlayerA,
        PlayerB,
    };
    Q_ENUM(Player)

    enum PieceType {
        TypeNone,
        Man,
        King,
    };
    Q_ENUM(PieceType)

    explicit Enums(QObject *parent = nullptr) : QObject(parent) {}

};


#endif // ENUMS_H
