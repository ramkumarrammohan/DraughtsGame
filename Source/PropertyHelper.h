#ifndef PROPERTYHELPER_H
#define PROPERTYHELPER_H

/*
 * Source link: https://gist.github.com/Rolias/48d453a0490d36090193
 */

#pragma once
#include <QObject>
//See Gist Comment for description, usage, warnings and license information
#define AUTO_PROPERTY(TYPE, NAME, VAL) \
    Q_PROPERTY(TYPE NAME READ NAME WRITE set##NAME NOTIFY NAME##Changed ) \
    public: \
       TYPE NAME() const { return m_##NAME ; } \
       void set##NAME(const TYPE& value) { \
          if (m_##NAME == value)  return; \
          m_##NAME = value; \
          emit NAME##Changed(value); \
        } \
       Q_SIGNAL void NAME##Changed(TYPE value);\
    private: \
       TYPE m_##NAME = VAL;

#define MANUAL_READ_PROPERTY(TYPE, NAME, VAL) \
    Q_PROPERTY(TYPE NAME READ NAME WRITE set##NAME NOTIFY NAME##Changed ) \
    public: \
       TYPE NAME() const; \
    void set##NAME(const TYPE& value) { \
       if (m_##NAME == value)  return; \
       m_##NAME = value; \
       emit NAME##Changed(value); \
     } \
       Q_SIGNAL void NAME##Changed(TYPE value);\
    private: \
       TYPE m_##NAME = VAL;

#define MANUAL_WRITE_PROPERTY(TYPE, NAME, VAL) \
    Q_PROPERTY(TYPE NAME READ NAME WRITE set##NAME NOTIFY NAME##Changed ) \
    public: \
       TYPE NAME() const { return m_##NAME ; } \
       void set##NAME(const TYPE& value); \
       Q_SIGNAL void NAME##Changed(TYPE value);\
    private: \
       TYPE m_##NAME = VAL;

#define MANUAL_READ_WRITE_PROPERTY(TYPE, NAME, VAL) \
    Q_PROPERTY(TYPE NAME READ NAME WRITE set##NAME NOTIFY NAME##Changed ) \
    public: \
       TYPE NAME() const; \
       void set##NAME(const TYPE& value); \
       Q_SIGNAL void NAME##Changed(TYPE value);\
    private: \
       TYPE m_##NAME = VAL;

#define READONLY_PROPERTY(TYPE, NAME, VAL) \
    Q_PROPERTY(TYPE NAME READ NAME CONSTANT ) \
    public: \
       TYPE NAME() const { return m_##NAME ; } \
    private: \
       TYPE m_##NAME = VAL;

#define READ_PROPERTY(TYPE, NAME, VAL) \
    Q_PROPERTY(TYPE NAME READ NAME NOTIFY NAME##Changed) \
    public: \
       TYPE NAME() const { return m_##NAME ; } \
       Q_SIGNAL void NAME##Changed(TYPE value);\
    private: \
       TYPE m_##NAME = VAL;

#endif // PROPERTYHELPER_H
