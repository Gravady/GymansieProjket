#include <include/gui/mainwindow.h>

void MainWindow::updateResMod(){
	SCREEN screen = SCREEN(MonitorFromWindow((HWND)this->winId(), MONITOR_DEFAULTTONEAREST));
	screen.getScreenResolution();
	MainWindow::y_mod = static_cast<double>(screen.screen_height) / 1080.0;
	MainWindow::x_mod = static_cast<double>(screen.screen_width) / 1920.0;
}

void MainWindow::labelSetup() {

}

template <typename Layout, typename Receiver>
QPushButton* MainWindow::buttonSetup(
	Layout* layout,
	const std::string& text,
	void (Receiver::* slot)())
{
	//Assertations can be worked upon
	static_assert(std::is_base_of_v<QLayout, Layout>, "Layout must derive from QLayout");
	static_assert(std::is_base_of_v<QObject, Receiver>, "Receiver must derive from QObject");

	QPushButton* button = new QPushButton(QString::fromStdString(text), this);
	layout->addWidget(button);

	connect(button, &QPushButton::clicked, this, slot);

	return button;
}

QT_STATUS MainWindow::setupMainPanel(Bar::BAR_POSITION position = Bar::DEFAULT) {
	Bar mainbar;
	mainbar.barWidget = new QWidget(this);
	mainbar.barWidget->setFixedHeight(30 * y_mod);
	mainbar.barWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	mainbar.barLayout = new QHBoxLayout(mainbar.barWidget);
	mainbar.barLayout->setContentsMargins(0, 0, 0, 0);
	mainbar.barLayout->setSpacing(10*y_mod/2);
}

QT_STATUS MainWindow::setupUI(){
	QWidget* centralWidget = new QWidget(this);
	setCentralWidget(centralWidget);

	setupMainPanel();
}

QT_STATUS MainWindow::shutdownUI(){

}