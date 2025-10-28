#ifndef SCREEN_H
#define SCREEN_H

#include <include/windows/win_utils.h>

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>s

#include <QObject>

#include <winuser.h>

//Move only function wrapper for screen related functionality

class SCREEN
{
public:
	SCREEN() = delete;
	SCREEN(const SCREEN& screen) = delete;
	SCREEN(SCREEN&& screen) = default;

	explicit SCREEN(HMONITOR monitor) : hMonitor(monitor){
		MONITORINFOEX monitorInfoEx;
		monitorInfoEx.cbSize = sizeof(MONITORINFOEX);
		if (GetMonitorInfo(hMonitor, &monitorInfoEx)) {
			this->screen_width = monitorInfoEx.rcMonitor.right - monitorInfoEx.rcMonitor.left;
			this->screen_height = monitorInfoEx.rcMonitor.bottom - monitorInfoEx.rcMonitor.top;
		}
		//Screeninit
		this->screen_width = static_cast<uint16_t>(GetSystemMetrics(SM_CXSCREEN));
		this->screen_height = static_cast<uint16_t>(GetSystemMetrics(SM_CYSCREEN));
		this->screen = GetDC(nullptr);
	}

	SCREEN& operator=(const SCREEN& screen) = delete;
	SCREEN& operator=(SCREEN&& screen) = default;

	~SCREEN() {
		ReleaseDC(nullptr, screen);
	};
public:
	inline void getScreenResolution(
		uint16_t& width,
		uint16_t& height);

	inline void getScreenResolution();

	inline void getSystemDPI(
		uint16_t& dpiX,
		uint16_t& dpiY);

	inline void getSystemDPI();
	
	inline void changeRefreshRate(
		uint16_t width, uint16_t height,
		uint16_t refreshRate);

	void getWindowNativeMetrics(HWND hwnd);
	void getWindowMetrics(const QObject* window);

	static void allocateScreenInstance(
		SCREEN* screen
		);

	static std::optional<SCREEN> getPrimaryScreen();

	uint16_t screen_width;
	uint16_t screen_height;
	uint16_t system_dpiX;
	uint16_t system_dpiY;
	uint16_t refresh_rate;
	std::string deviceName;

	HMONITOR hMonitor;
	HDC screen;
};

#endif