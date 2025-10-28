#ifndef DBG_H
#define DBG_H

#include <QDebug>

#include <memory>
#include <string>
#include <mutex>
#include <atomic>

#include <include/windows/win_utils.h> //For opening debugging terminal
#include <include/windows/win.h>

//Debug header primarly for debugging purposes in developer mode
//Functionality for locking program or freezing other things whilst debugging is happening to not cause more errors
class DBG
{
public:

private:
	std::mutex dbg_lock; 
public:
};

#endif