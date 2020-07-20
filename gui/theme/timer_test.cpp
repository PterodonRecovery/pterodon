/*
    Pterodon Recovery - timer and atomic byte test
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

#include <sys/types.h>
#include <sys/stat.h>

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <pthread.h>
#include <chrono>


#define byte uint8_t

class Timer
{
public:
void start() {
time = std::chrono::system_clock::now();
process_ended = false;
if (pthread_mutex_init(&timer_locker, NULL) != 0) {
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
	pthread_mutex_destroy(&timer_locker);
}

public:
void end() {
if (!process_ended) {
if (use_mutex)
pthread_mutex_lock(&timer_locker);
std::chrono::duration<double> duration = std::chrono::system_clock::now() - time;
printf("%s took %.9f s.\n", process_name.c_str(), duration.count());
process_ended = true;
if (use_mutex)
pthread_mutex_unlock(&timer_locker);
}
}
void endProcess() {
if (use_mutex) {
pthread_mutex_lock(&timer_locker);
process_ended = true;
pthread_mutex_unlock(&timer_locker);
} else {
process_ended = true;
}
}
double getTime() {
if (use_mutex)
pthread_mutex_lock(&timer_locker);
std::chrono::duration<double> duration = std::chrono::system_clock::now() - time;
double ret = duration.count();
if (use_mutex)
pthread_mutex_unlock(&timer_locker);
return ret;
}
private:
std::string process_name;
bool process_ended;
std::chrono::time_point<std::chrono::system_clock> time;
pthread_mutex_t timer_locker;
bool use_mutex;
};

class AtomicByte {
private:
    byte value;
    pthread_mutex_t mutex;
	bool mutex_ok;
public:
virtual ~AtomicByte() {
	if (mutex_ok)
		pthread_mutex_destroy(&mutex);
}

void set(byte new_value) {
	if (mutex_ok) {
		pthread_mutex_lock(&mutex);
		value = new_value;
		pthread_mutex_unlock(&mutex);
	} else {
		value = new_value;
	}
}

byte get(void) {
	byte ret;
	if (mutex_ok) {
		pthread_mutex_lock(&mutex);
		ret = value;
		pthread_mutex_unlock(&mutex);
	} else {
		ret = value;
	}
	return ret;
   }
   
AtomicByte(byte init_value) : value(init_value) {
    if (pthread_mutex_init(&mutex, NULL) != 0) {
	mutex_ok = false;
	} else {
	mutex_ok = true;
	}
}

AtomicByte()
    : value(0)
{
if (pthread_mutex_init(&mutex, NULL) != 0) {
	mutex_ok = false;
	} else {
	mutex_ok = true;
	}
}

explicit operator bool() const { return value != 0; }

AtomicByte& operator=(AtomicByte& other) {
byte ret = other.get();
set(ret);
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


int main(void) {
Timer timer, yolo, dd, tt;
AtomicByte lol = 5, kk = 3, haha = 6, wow;
printf("lol: %d, kk: %d, haha: %d\n", lol.get(), kk.get(), haha.get());
if (lol == 4)
printf("lol = 5\n");
if (lol != 3)
printf("lol = 2\n");
kk = 1;
if (kk)
printf("haha\n");
return 0;
}


