#ifndef COMM_H
#define COMM_H

//Communication header file for identifying the communication method such as file extension etc
//to be able to recieve and give data in the correct format

#include <QString>
#include <QByteArray>

class Comm
{
public:
private:
public:
	uint8_t connectComm(QString message);
	uint8_t connectComm(QByteArray message);
	uint8_t disconnectComm();
};

#endif