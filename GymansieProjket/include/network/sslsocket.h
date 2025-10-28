#ifndef SSLSOCKET_H
#define SSLSOCKET_H

#include <QSslSocket>
#include <QSsl>

#include <windows.h>
#include <wincrypt.h>

#include <vector>

#include <include/bytestream/file.h>

class SslSocket : public QSslSocket
{
	Q_OBJECT
public:

private:
	QSslSocket socket;
	struct Device
	{
		uint8_t id;
		bool trusted;
		const char* ip_addr;
	};
	std::vector<Device*> devices;
	std::vector<const char*> prefered_file_types;

public:
	bool checkSSLComp(PCCERT_CONTEXT cert, LPWSTR serverName);
	[[nodiscard]] uint8_t* connectLink(const char* ip_addr, const char* dns = "8.8.8.8", const char* proc); //return array of device id and network status
	uint8_t trustConnectedDevice(uint8_t device_id, bool trust);
	void registerPreferedFileType(const char* file_ext);
};

#endif