#ifndef THREAD_H
#define THREAD_H

#include <include/thread/proc_signal.h>

//DISCLAIMER: Because the threading implementation by Qt is already very abstract
//and high, my additional abstraction layer might be redundant in some cases, further testing
//is required to ensure that this is not the case, elsewise a more minimal thread implementation
//will be required

//IS CURRENTLY BEING REWORKED

//Master include for all threading related functionality

class Processor : public SlotProvider, public CommunicationProvider

#endif

