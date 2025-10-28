#ifndef FRAME_H
#define FRAME_H

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QtWidgets>
#include <QMainWindow>
#include <QObject>

#include <include/windows/screen.h>
#include <include/util/types.h>
#include <include/util/status.h>

//Mainwindow and other GUI elements need to assesed based on the users system
//and screen resolution, so we need to pass the window name to the constructor

class Menu;
class Tracking;
class Connect;
class Goals;
class Settings;
class Installation;

//TODO: MenuStack may not be needed
class MenuStack
{
public:
	//Moveable only
public:
	void addStackItem(MenuStack* stack);
	void removeStackItem(MenuStack* stack);
	void lockStackItem(MenuStack** stack);
	void lockStackItems();
	void unlockStackItems();
private:
	QList<QObject*> stackItems;
};

//HACK: All of the MainWindow properties need to be later reevaluted to fit the design
class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(QWidget* parent = nullptr, const QString window_name){
		setWindowTitle(window_name); 
		updateResMod();
		resize(400 * y_mod, 300 * x_mod);
		setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
		setAcceptDrops(true);
		setupUI();

	};

	~MainWindow() override = default;

//HACK: Because of AST and compiler things variables will be defined as the second part of the class
//instead of the last part of the class
private:
	std::string window_name;
	static uint8_t y_mod;
	static uint8_t x_mod;
	struct Bar
	{
		enum BAR_POSITION
		{
			//DEFAULT(On top) = unknown
			DEFAULT = 0,
			TOP = 1,
			BOTTOM = 2,
			LEFT = 3,
			RIGHT = 4
		};
		QWidget* barWidget;
		QHBoxLayout* barLayout;
		QLabel* barLabel;
		QPushButton* minimizeBtn;
		QPushButton* maximizeBtn;
		QPushButton* closeBtn;
		BAR_POSITION position;
	};

public:
	template <typename Layout, typename Receiver>
	QPushButton* buttonSetup(
		Layout* layout,
		const std::string& text,
		void (Receiver::* slot)());

	QT_STATUS adjustMainPanel(Bar::BAR_POSITION position);
	QT_STATUS removeMainPanel();
	QT_STATUS setupMainPanel(Bar::BAR_POSITION position = Bar::DEFAULT);

	QT_STATUS setupUI();
	QT_STATUS shutdownUI();
	
	void updateResMod();
};


#endif