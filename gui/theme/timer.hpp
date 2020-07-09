/*
    Pterodon Recovery - special thread objects
    Copyright (C) <2019> ATGDroid <bythedroid@gmail.com>

    This file is part of Pterodon Recovery Project

    Pterodon Recovery is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Pterodon Recovery is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Pterodon Recovery.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef PTERODON_TIMER_HPP
#define PTERODON_TIMER_HPP

#include <string>
#include <chrono>
#include "aroma_internal.h"
#include "constants.h"

class Timer
{
public:
void start() {
time = std::chrono::system_clock::now();
process_ended = false;
if (libaroma_mutex_init(timer_locker) != 0) {
use_mutex = false;
} else {
use_mutex = true;
}
}
Timer(std::string process = "Process")
			: process_name(process)
		{
		start();
		}
virtual ~Timer() {
end();
if (use_mutex)
	libaroma_mutex_free(timer_locker);
}

public:
void end() {
if (!process_ended) {
if (use_mutex)
libaroma_mutex_lock(timer_locker);
std::chrono::duration<double> duration = std::chrono::system_clock::now() - time;
LOGI("%s took %.9f s.", process_name.c_str(), duration.count());
process_ended = true;
if (use_mutex)
libaroma_mutex_unlock(timer_locker);
}
}
void endProcess() {
if (use_mutex) {
libaroma_mutex_lock(timer_locker);
process_ended = true;
libaroma_mutex_unlock(timer_locker);
} else {
process_ended = true;
}
}
double getTime() {
if (use_mutex)
libaroma_mutex_lock(timer_locker);
std::chrono::duration<double> duration = std::chrono::system_clock::now() - time;
double ret = duration.count();
if (use_mutex)
libaroma_mutex_unlock(timer_locker);
return ret;
}
private:
std::string process_name;
bool process_ended;
std::chrono::time_point<std::chrono::system_clock> time;
LIBAROMA_MUTEX timer_locker;
bool use_mutex;
};

class AtomicByte {
private:
    byte value;
    LIBAROMA_MUTEX mutex;
	bool mutex_ok;
public:
virtual ~AtomicByte() {
	if (mutex_ok)
		libaroma_mutex_free(mutex);
}

void set(byte new_value) {
	if (mutex_ok) {
		libaroma_mutex_lock(mutex);
		value = new_value;
		libaroma_mutex_unlock(mutex);
	} else {
		value = new_value;
	}
}

byte get(void) {
	byte ret;
	if (mutex_ok) {
		libaroma_mutex_lock(mutex);
		ret = value;
		libaroma_mutex_unlock(mutex);
	} else {
		ret = value;
	}
	return ret;
   }
   
AtomicByte(byte init_value) : value(init_value) {
    if (libaroma_mutex_init(mutex) != 0) {
	mutex_ok = false;
	} else {
	mutex_ok = true;
	}
}

AtomicByte()
    : value(0)
{
if (libaroma_mutex_init(mutex) != 0) {
	mutex_ok = false;
	} else {
	mutex_ok = true;
	}
}

explicit operator bool() const { return value != 0; }

AtomicByte& operator=(AtomicByte& other) {
set(other.get());
return *this;
}

AtomicByte& operator=(const byte new_value) {
set(new_value);
return *this;
}

bool operator==(AtomicByte& other) {
return get() == other.get();
}

bool operator==(const byte new_value) {
return get() == new_value;
}
  
bool operator!=(AtomicByte& other) {
return get() != other.get();
}
bool operator!=(const byte new_value) {
return get() != new_value;
}

AtomicByte& operator+(AtomicByte& other) {
set(get() + other.get());
return *this;
}

AtomicByte& operator+=(AtomicByte& other) {
return (*this + other);
}

AtomicByte& operator-(AtomicByte& other) {
set(get() - other.get());
return *this;
}

AtomicByte& operator-=(AtomicByte& other) {
return (*this - other);
  }
};


#endif // PTERODON_TIMER_HPP


