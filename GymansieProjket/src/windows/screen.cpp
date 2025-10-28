#include <include/windows/screen.h>

inline void SCREEN::getScreenResolution()
{
	screen_width = static_cast<uint16_t>(GetSystemMetrics(SM_CXSCREEN));
	screen_height = static_cast<uint16_t>(GetSystemMetrics(SM_CYSCREEN));
}

inline void SCREEN::getScreenResolution(uint16_t& width, uint16_t& height)
{
	width = static_cast<uint16_t>(GetSystemMetrics(SM_CXSCREEN));
	height = static_cast<uint16_t>(GetSystemMetrics(SM_CYSCREEN));
}

inline void SCREEN::getSystemDPI(uint16_t& dpiX, uint16_t& dpiY)
{
	dpiX = static_cast<uint16_t>(GetDeviceCaps(screen, LOGPIXELSX));
	dpiY = static_cast<uint16_t>(GetDeviceCaps(screen, LOGPIXELSY));
}

inline void SCREEN::getSystemDPI()
{
	system_dpiX = static_cast<uint16_t>(GetDeviceCaps(screen, LOGPIXELSX));
	system_dpiY = static_cast<uint16_t>(GetDeviceCaps(screen, LOGPIXELSY));
}

inline void SCREEN::changeRefreshRate(uint16_t width, uint16_t height, uint16_t refreshRate)
{
	DEVMODE devMode = {};
	devMode.dmSize = sizeof(DEVMODE);
	devMode.dmPelsWidth = width;
	devMode.dmPelsHeight = height;
	devMode.dmDisplayFrequency = refreshRate;
	devMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;
	LONG result = ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);
	if (result != DISP_CHANGE_SUCCESSFUL) {
		
	}
}