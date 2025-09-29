#ifndef THREAD_H
#define THREAD_H

//DISCLAIMER: Because the threading implementation by Qt is already very abstract
//and high, my additional abstraction layer might be redundant in some cases, further testing
//is required to ensure that this is not the case, elsewise a more minimal thread implementation
//will be required

//Master include for all threading related functionality
#include <include/thread/thread_core.h>
#include <include/thread/thread_dump.h>
#include <include/thread/thread_future.h>
#include <include/thread/thread_sync.h>
#include <include/thread/thread_model/thread_pool.h>
#include <include/thread/thread_model/thread_single.h>

#endif

