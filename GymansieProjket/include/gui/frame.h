#ifndef FRAME_H
#define FRAME_H

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QtWidgets>
#include <QMainWindow>
#include <QObject>

#include <include/windows/screen.h>
#include <include/util/status.h>

#include <include/thread/proc_signal.h>
#include <include/util/stack.h>

//CRTP
class WindowHandler
{ 
public:
	WindowHandler& returnDerived() {
		return static_cast<WindowHandler*>(this);
	}

	explicit WindowHandler() {
		returnDerived();
	}

	virtual ~WindowHandler();
protected:
	//WindowHandler** represents classes that derive from WindowHandler
	std::unordered_map<void*, WindowHandler*> callback_hanlder;
public:
	template <typename Ret, typename C, typename ...Args>
	QT_STATUS registerTaskCallback(
		Ret(C::*)(Args...),
		Args&&... args,
		std::unordered_map<*void, WindowHandler*> callback_map
	) noexcept;

	template <typename Ret, typename ...Args>
	constexpr auto elementStackDump(
		Ret(*stack_func)(Args...) -> Ret(*)(Args...), 
		
	)
};

//Window workflow for different panels
class WindowWorkFlow
{

};

//Mainwindow and other GUI elements need to assesed based on the users system
//and screen resolution, so we need to pass the window name to the constructor
//HACK: All of the MainWindow properties need to be later reevaluted to fit the design
class MainWindow : public QMainWindow, public QObject, public WindowHandler
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
	//NOTE: Im unsure about this code and how it works, look into it
	template <typename Layout, typename Receiver>
	[[nodiscard]] QPushButton* buttonSetup(
		Layout* layout,
		const std::string& text,
		void (Receiver::* slot)()) noexcept;

	//To be clear Panel means the uppmost point of the window with X and -
	QT_STATUS adjustMainPanel(Bar::BAR_POSITION position);
	QT_STATUS removeMainPanel();
	QT_STATUS setupMainPanel(Bar::BAR_POSITION position = Bar::DEFAULT);
	template <typename Ret, typename ...Args>
	QT_STATUS addPanelAction(

	)

	QT_STATUS setupUI();
	QT_STATUS shutdownUI();
	
	void updateResMod();
};


#endif