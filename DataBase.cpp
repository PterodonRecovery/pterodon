/*
    Pterodon Recovery - DataBase source file
    Copyright (C) <2020> ATGDroid <bythedroid@gmail.com>

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

#include <unordered_map> // members of the unordered_map
#include <string> // string
#include <sstream> // istringstream()
#include <fstream> // ifstream
#include "framework.hpp" // is_numeric(), FileExists(), delete_file()
#include "DataBase.hpp" // header
#include "constants.h" // LOGI(), LOGW()

#define PTERODON_DATABASE_VERSION 2 // 4

#ifndef O_BINARY
#define O_BINARY 0
#endif


#ifdef PTERODON_DATABASE_USES_LOWLEVEL_CALLS
#include <unistd.h> // TEMP_FAILURE_RETRY()
#include <fcntl.h> // O_WRONLY | O_CREAT | O_TRUNC | O_BINARY
#else
#include <stdio.h> // fread(), fwrite(), fopen(), fclose()
#endif

namespace Pterodon {

namespace wrappers {

DataBase::~DataBase(void) {
/* clear all the data base variables once the object goes out
*  of scope                                                                                           */
clear();
}


 /*
*  Just in case that we ever save some sensitive information
*  to a database file, we should also use some simple encryption
*  algorithm to at least prevent our file from being trivially decrypted by some average person.
*  This function does exactly what we need.
*/
void DataBase::Format(std::string& str) const {
	/* We use a lot of keys, since each additional key which we use is gonna 
     *  effectively double the amount of time it takes to brute force the encrypted string. */
    char key[63] = {'o', 'd', 'X', '`', '`', 'M', 
                               'J', ']', 'f', 'I', 'Y', '_', 'e', ',',
                               'q', 'J', '7', 'L', 'N', '/', '#', '8', 
                               'z', '7', '*', 'q', 'w', '9', 'f', 'x', 'F', 
                               '|', 'o', 'O', 'b', '{', 'Z', 'V', 'Q', 'z', 'P', 
                               'a', ';', 'T', '$', '^', 'h', 'h', 'K', '$', '9', '6', 
                               'E', '#', 'L', 'I', 'N', '4', 'q', '8', '(', '2', 'V',
};
   
   /* this is a Exclusive-OR encryption implementation,
   *  so this function can be called for both encryption and decryption routines */
    for (size_t i = 0; i < str.size(); i++)
        str[i] = str[i] ^ key[i % (sizeof(key) / sizeof(char))];
}

/* read and decrypt key/value strings from a data base file */
bool DataBase::ReadValue(
#ifdef PTERODON_DATABASE_USES_LOWLEVEL_CALLS
     int fd,
#else
     FILE* fp,
#endif
     std::string &mValue) const {
     
    size_t len;
    /* read the length of a c-string first, then read the data */
#ifdef PTERODON_DATABASE_USES_LOWLEVEL_CALLS
    if (TEMP_FAILURE_RETRY(read(fd, &len, sizeof(len))) != sizeof(len))
#else
    if (fread(&len, 1, sizeof(size_t), fp) != sizeof(size_t))
#endif
	goto fail;
	/* we know the length of a c-string which we are now going to read,
	*   so we can now just resize our std::string so it can fit the whole data */
	mValue.resize(len);
#ifdef PTERODON_DATABASE_USES_LOWLEVEL_CALLS
	if (TEMP_FAILURE_RETRY(read(fd, &mValue[0], len)) < 0)
#else
    if (fread(&mValue[0], 1, len, fp) != len)
#endif
	goto fail;
	len = mValue.size();
	Format(mValue);
	/* decrypted string ends with one character which we should
	 *  ignore, so resize the string by one character to remove it */
	if (len > 1)
	mValue.resize(len - 1);
	return true;
	
	fail:
#ifdef PTERODON_DATABASE_USES_LOWLEVEL_CALLS
    close(fd);
#else
	fclose(fp);
#endif
	return false;
}

/* encrypt and write key/value strings to a data base file */
bool DataBase::WriteValue(
#ifdef PTERODON_DATABASE_USES_LOWLEVEL_CALLS
int fd,
#else
FILE* fp, 
#endif
const std::string& mValue) const {

        std::string out_str = mValue;
        /* write the length of a c-string first, then write the data */
        size_t length = out_str.length() + 1;
#ifdef PTERODON_DATABASE_USES_LOWLEVEL_CALLS
        if (TEMP_FAILURE_RETRY(write(fd, &length, sizeof(length))) != sizeof(length)) 
#else
		if (fwrite(&length, 1, sizeof(size_t), fp) != sizeof(size_t))
#endif
        return false;
        /* encrypt our string before we write it to the data base file */
		Format(out_str);
#ifdef PTERODON_DATABASE_USES_LOWLEVEL_CALLS
		if (TEMP_FAILURE_RETRY(write(fd, &out_str[0], length)) < 0)
#else
		if (fwrite(out_str.c_str(), 1, length, fp) != length)
#endif
		return false;
		else
		return true;
}

bool DataBase::LoadFromPropFile(const std::string& path) {
    LOGI("Loading new data from property file: '%s'", path.c_str());
    std::ifstream file(path.c_str());
    if(file.fail()) {
    LOGW("Failed to open specified file!");
    return false;
    }
    std::string mVarName, mVarValue, line;
    size_t pos;
	while (std::getline(file,line)) {
    if (line[0] == '#' || line[0] == ' ' || line[0] == '=')
    continue;
    pos = line.find_first_of("=");
    if (pos != std::string::npos) {
    mVarName = line.substr(0, pos);
    mVarValue = line.substr(pos + 1);
    SetVar(mVarName, mVarValue);
    }
   }
 return true;
 }


/* load and save data from a data base file to the device memory. 
*   This function does not overwrite values of the keys which already
*   exists.  */
bool DataBase::Unserialize(const std::string& path, bool check_version) {
	LOGI("Loading new data from '%s'", path.c_str());
#ifdef PTERODON_DATABASE_USES_LOWLEVEL_CALLS
	int fd = TEMP_FAILURE_RETRY(open(path.c_str(), O_CREAT | O_RDONLY | O_BINARY, 0644));
    if (fd < 0) {
#else
	FILE* fp = fopen(path.c_str(), "rb");
	if (!fp) {
#endif
	LOGW("Unable to open data base file!");
	return false;
	}
	bool first_run = true;
#ifdef PTERODON_DATABASE_USES_LOWLEVEL_CALLS
    while (true) {
#else
	while (!feof(fp)) {
#endif
	std::string mVarName, mVarValue;
	if (first_run) {
#ifdef PTERODON_DATABASE_USES_LOWLEVEL_CALLS
    if (!ReadValue(fd, mVarName))
#else
	if (!ReadValue(fp, mVarName))
#endif
	return true;
	/* data base version should always be a numeric variable */
	if (!Pterodon::Framework::is_numeric(mVarName)) {
	LOGW("Cannot parse data file: data version is not numeric");
	break;
	}
	/* We don't want to load files which doesn't match the current data base
	*  version.. For example some critical part is changed in the source and we
	*  don't want to load variables which are incompatible with it, so just change
    *  the data base version to ignore these old files.  */
	if (check_version && mVarName != EXPAND(PTERODON_DATABASE_VERSION)) {
		LOGW("Data version verification failed, expected '%s' but received '%s'", 
        EXPAND(PTERODON_DATABASE_VERSION), mVarValue.c_str());
		break;
		}
#ifdef PTERODON_DATABASE_USES_LOWLEVEL_CALLS
    if (!ReadValue(fd, mVarValue))
#else
	if (!ReadValue(fp, mVarValue))
#endif
	return true;
    /* variable which indicate the amout of all variables in the file is
	*  expected to be a numeric */
	if (!Pterodon::Framework::is_numeric(mVarValue)) {
	LOGW("Cannot parse data file: size of data is not numeric");
	break;
	}
	std::istringstream iss(mVarValue);
	size_t len;
    iss >> len;
    reserve(len);
    first_run = false;
    continue;
    }
#ifdef PTERODON_DATABASE_USES_LOWLEVEL_CALLS
   if (!ReadValue(fd, mVarName) || 
        !ReadValue(fd, mVarValue))
 #else
    if (!ReadValue(fp, mVarName) || 
        !ReadValue(fp, mVarValue))
#endif
        return true;
        
        SetVar(mVarName, mVarValue);
 }
#ifdef PTERODON_DATABASE_USES_LOWLEVEL_CALLS
    close(fd);
#else
	fclose(fp);
#endif
	return false;
}

/* a c++20 contains() implementation */
bool DataBase::contains(const std::string& mVarName) const
{
    //std::unordered_map<std::string, std::string>::const_iterator pos = data_.find(mVarName);
    //return pos != data_.end();
    return (data_.find(mVarName) != data_.end());
}

/* check if data base contains variable with specific key and value */
bool DataBase::contains(const std::string& mVarName, const std::string& mVarValue) const
{
    std::unordered_map<std::string, std::string>::const_iterator pos = data_.find(mVarName);
    return (pos != data_.end() && pos->second == mVarValue);
}

/* Add new variable to data base, variable can't be added if some variable
*   with the same key already exists */
void DataBase::NewVar(const std::string& mVarName, const std::string& mVarValue) {
/*
*      With emplace(mVarName,mVarValue)
*      Serialization and unserialization of 30 variables took 0.000999000 s.
*
*      With insert(std::make_pair(mVarName,mVarValue)
*      Serialization and unserialization of 30 variables took 0.001171000 s.
*
*      This testing also included the calls for SetVar(), to first create these 30 variables
*      for (int i = 0; i < 30; i++)
*      data.SetVar(("pterodon_var_" + to_string(i)), "some_random_variable");
*/ 
   data_.emplace(mVarName,mVarValue);
   //data_.emplace(std::make_pair(mVarName, mVarValue));
   
   //data_.insert(std::make_pair(mVarName,mVarValue));
    
}

/* Reserve space for a new variables, this function should only be called
*   when you know that there is gonna be added a lot of new variables
*   and you know the specific number of them.
*/
void DataBase::reserve(const size_t& value) {
/* for testing */
// return;

/*
* With reserve()
* Serialization and unserialization of 30 variables took 0.001041000 s.
*
* Without reserve()
* Serialization and unserialization of 30 variables took 0.001407000 s.
*/
LOGI("Reserving: %zu", value);
data_.reserve(value);
}

/* deletes all the variables which are saved at the moment */
void DataBase::clear(void) {
data_.clear();
}

/* returns the number of all variables which are saved at the moment */
size_t DataBase::size(void) const {
return data_.size();
}

/* Check whether database is empty (there aren't saved any variables) */
bool DataBase::empty(void) const {
return data_.empty();
}

bool DataBase::SaveToPropFile(const std::string& path) const {
#ifdef PTERODON_DATABASE_USES_LOWLEVEL_CALLS
   int fd = TEMP_FAILURE_RETRY(open(path.c_str(), 
               O_WRONLY | O_CREAT | O_TRUNC, 0644));
    if (fd < 0)
#else
    FILE* fp = fopen(path.c_str(), "w");
	if (!fp)
#endif
    return false;
    
        for (std::unordered_map<std::string, std::string>::const_iterator it = data_.begin(); it != data_.end(); ++it) {
        	std::string out_str = it->first + "=" + it->second + "\n";
#ifdef PTERODON_DATABASE_USES_LOWLEVEL_CALLS
		if (TEMP_FAILURE_RETRY(write(fd, &out_str[0], out_str.size())) < 0)
#else
		if (fwrite(out_str.c_str(), 1, out_str.size(), fp) != out_str.size())
#endif
            goto fail;
	}
      LOGI("Saved data to prop file '%s'", path.c_str());
#ifdef PTERODON_DATABASE_USES_LOWLEVEL_CALLS
    close(fd);
#else
	fclose(fp);
#endif
	return true;
	
	fail:
#ifdef PTERODON_DATABASE_USES_LOWLEVEL_CALLS
    close(fd);
#else
	fclose(fp);
#endif
	LOGW("Failed to save data to prop file '%s'", path.c_str());
	Pterodon::Framework::delete_file(path);
	return false;  
}

/* Save all key/value variables to a data base file */
bool DataBase::Serialize(const std::string& path) const {
if (Pterodon::Framework::FileExists(path))
     Pterodon::Framework::delete_file(path);
#ifdef PTERODON_DATABASE_USES_LOWLEVEL_CALLS
   int fd = TEMP_FAILURE_RETRY(open(path.c_str(), 
               O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, S_IRUSR | S_IWUSR));
    if (fd < 0)
#else
    FILE* fp = fopen(path.c_str(), "wb");
	if (!fp)
#endif
		return false;
	
		/* first write data base version, after that number which
		 * will later tell us how many key/value variables are saved
	     * in this file once we try to load this file again, this way we can
	     * just call reserve() to prepare data base for all those
	     * new variables */
#ifdef PTERODON_DATABASE_USES_LOWLEVEL_CALLS
		if (!WriteValue(fd, EXPAND(PTERODON_DATABASE_VERSION)) || 
             !WriteValue(fd, std::to_string(data_.size())))
#else
         if (!WriteValue(fp, EXPAND(PTERODON_DATABASE_VERSION)) || 
             !WriteValue(fp, std::to_string(data_.size())))
#endif
		 goto fail;
		
    for (std::unordered_map<std::string, std::string>::const_iterator it = data_.begin(); it != data_.end(); ++it) {
#ifdef PTERODON_DATABASE_USES_LOWLEVEL_CALLS
        if (!WriteValue(fd, it->first) || 
             !WriteValue(fd, it->second))
#else
    	if (!WriteValue(fp, it->first) || 
             !WriteValue(fp, it->second))
#endif
            goto fail;
	}
	LOGI("Saved data to '%s'", path.c_str());
#ifdef PTERODON_DATABASE_USES_LOWLEVEL_CALLS
    close(fd);
#else
	fclose(fp);
#endif
	return true;
	
	fail:
#ifdef PTERODON_DATABASE_USES_LOWLEVEL_CALLS
    close(fd);
#else
	fclose(fp);
#endif
	LOGW("Failed to save data to '%s'", path.c_str());
	Pterodon::Framework::delete_file(path);
	return false;
}

/* log all the key/value variables to our recovery log file */
void DataBase::print(void) const {
    for (std::unordered_map<std::string, std::string>::const_iterator it = data_.begin(); it != data_.end(); ++it) {
    LOGI("'%s' --> '%s'", it->first.c_str(), it->second.c_str());
   }
}

/* copy a value of key specified by mVarName to mVarValue */
bool DataBase::GetVar(const std::string& mVarName, std::string &mVarValue) const { 
	 std::unordered_map<std::string, std::string>::const_iterator pos = data_.find(mVarName);
     if (pos == data_.end())
     return false;
     mVarValue = pos->second;
	 return true;
}

bool DataBase::GetVarByValue(const std::string& mVarName, std::string &mVarValue) const {
	for (std::unordered_map<std::string, std::string>::const_iterator it = data_.begin(); it != data_.end(); ++it) {
    if (it->second == mVarName) {
        mVarValue = it->first;
        return true;
        }
     }
     return false;
}

/* returns a value of key specified by mVarName */
std::string DataBase::GetVar(const std::string& mVarName) const { 
     std::unordered_map<std::string, std::string>::const_iterator pos = data_.find(mVarName);
     return (pos == data_.end() ? "" : pos->second);
}

/* sets a new value specified by mVarValue to a key specified by mVarName,
*   if a specified key does not exists then a new variable is automatically created */
void DataBase::SetVar(const std::string& mVarName, const std::string& mVarValue) {
	 std::unordered_map<std::string, std::string>::iterator pos = data_.find(mVarName);
     if (pos == data_.end())
     NewVar(mVarName, mVarValue);
     else
     pos->second = mVarValue;
}


/* removes variable specified by key, if such variable exists */
void DataBase::DelVar(const std::string& mVarName) {
	 std::unordered_map<std::string, std::string>::iterator pos = data_.find(mVarName);
     if (pos != data_.end())
        data_.erase(pos);
}

/* assign all variables from another DataBase object */
DataBase& DataBase::operator=(const DataBase& other) {
	/*
	    data_ = other.GetData();
	    return *this;
	*/
	clear();
	reserve(other.size());
	for (std::unordered_map<std::string, std::string>::const_iterator it = other.begin(); it != other.end(); ++it) {
    NewVar(it->first, it->second);
     }
     return *this;
   }

/* check if our key/value variables are equal to variables of another
*   DataBase object */
bool DataBase::operator==(const DataBase& other) const {
	if (size() != other.size())
	return false;
	
    for (std::unordered_map<std::string, std::string>::const_iterator it = other.begin(); it != other.end(); ++it) {
    if (!contains(it->first, it->second))
    return false;
     }
   return true;
  }
  
/* check if our key/value variables are not equal to variables of another
*   DataBase object */
bool DataBase::operator!=(const DataBase& other) const {
  if (size() != other.size())
	  return true;
	
    for (std::unordered_map<std::string, std::string>::const_iterator it = other.begin(); it != other.end(); ++it) {
    if (!contains(it->first, it->second))
    return true;
     }
   return false;
}

DataBase& DataBase::operator+(const DataBase& other) {
for (std::unordered_map<std::string, std::string>::const_iterator it = other.begin(); it != other.end(); ++it) {
SetVar(it->first, it->second);
}
 return *this;
}

DataBase& DataBase::operator+=(const DataBase& other) {
return (*this + other);
}

DataBase& DataBase::operator-(const DataBase& other) {
for (std::unordered_map<std::string, std::string>::const_iterator it = other.begin(); it != other.end(); ++it) {
DelVar(it->first);
}
return *this;
}

DataBase& DataBase::operator-=(const DataBase& other) {
return (*this - other);
}

} // namespace wrappers
} // namespace Pterodon