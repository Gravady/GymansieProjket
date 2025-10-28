#ifndef FILE_H
#define FILE_H

#include <include/windows/win.h>
#include <include/util/dbg.h> //Debugging file errors

#include <string>
#include <vector>
#include <cstdint>
#include <string_view>

//WINDOWS_FILESYSTEM can also be defined in compiler with WINDOWS_FILESYSTEM=0x0 or WINDOWS_FILESYSTEM=0x1
#if __has_include(<QFile>)
#include <QFile>
#include <QDir>
#include <QByteArray>
#include <QString>
DISPLAYMESSAGE("Using QFile for filesystem operations")
#else
DISPLAYERROR("QFile not found, please install Qt framework or use a different filesystem library")
#endif

class File
{
public:
	File(QString path) {
		file = new QFile(path);
		if (!file->exists()) {
			DISPLAYERROR("File does not exist: " + path.toStdString());
			//Do something more here
		}
	}
	File();

	~File();
private:
	QFile* file;
	std::string file_name;
	const char* file_path;
public:
	QByteArray static readFile(QString path);
	bool static writeFile(QString path, QByteArray data);
};

#endif