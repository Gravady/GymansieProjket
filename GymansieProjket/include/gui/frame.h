#ifndef FRAME_H
#define FRAME_H

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QtWidgets>

class Frame : public QFrame
{
	Q_OBJECT
public:
	Frame(QWidget* parent = nullptr) : QFrame(parent) {

	};

	~Frame() override = default;

public:

private:
	QLabel* label;
	QPushButton* button;
};

#endif