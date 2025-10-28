#ifndef WINDOW_ELEMENTS_H
#define WINDOW_ELEMENTS_H

#include <include/gui/mainwindow.h>

class Menu : public QObject, protected MenuStack
{
	Q_OBJECT
public:
	explicit Menu(QObject* parent = nullptr) : QObject(parent) {}
};

class Tracking : public QObject, protected MenuStack
{
	Q_OBJECT
public:
	explicit Tracking(QObject* parent = nullptr) : QObject(parent) {}
};

class Connect : public QObject, protected MenuStack
{
	Q_OBJECT
public:
	explicit Connect(QObject* parent = nullptr) : QObject(parent) {}
};

class Goals : public QObject, protected MenuStack
{
	Q_OBJECT
public:
	explicit Goals(QObject* parent = nullptr) : QObject(parent) {}
};

class Settings : public QObject, protected MenuStack
{
	Q_OBJECT
public:
	explicit Settings(QObject* parent = nullptr) : QObject(parent) {}
};

class Installation : public QObject, protected MenuStack
{
	Q_OBJECT
public:
	explicit Installation(QObject* parent = nullptr) : QObject(parent) {}
};

#endif