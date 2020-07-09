/*
    Pterodon Recovery - header file of the DataBase
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

#ifndef _PTERODON_DATABASE_HPP
#define _PTERODON_DATABASE_HPP

#include <string>
#include <unordered_map>

#define PTERODON_DATABASE_USES_LOWLEVEL_CALLS 1

namespace Pterodon {

namespace wrappers {

class DataBase
{
public:
	DataBase() = default;
	virtual ~DataBase();
	
public:
bool operator==(const DataBase& other) const;
bool operator!=(const DataBase& other) const;

DataBase& operator=(const DataBase& other);
DataBase& operator+(const DataBase& other);
DataBase& operator-(const DataBase& other);
DataBase& operator+=(const DataBase& other);
DataBase& operator-=(const DataBase& other);

std::unordered_map<std::string, std::string>::iterator begin() { return data_.begin(); }
std::unordered_map<std::string, std::string>::iterator end() { return data_.end(); }
std::unordered_map<std::string, std::string>::const_iterator begin() const { return data_.begin(); }
std::unordered_map<std::string, std::string>::const_iterator end() const { return data_.end(); }

public:
void NewVar(const std::string& mVarName, const std::string& mVarValue);
bool GetVar(const std::string& mVarName, std::string &mVarValue) const;
std::string GetVar(const std::string& mVarName) const;
void SetVar(const std::string& mVarName, const std::string& mVarValue);
void DelVar(const std::string& mVarName);
bool GetVarByValue(const std::string& mVarName, std::string &mVarValue) const;

void reserve(const size_t& value);
void clear(void);

bool contains(const std::string& mVarName) const;
bool contains(const std::string& mVarName, const std::string& mVarValue) const;

size_t size(void) const;
void print(void) const;
bool empty(void) const;

//std::unordered_map<std::string, std::string>* GetData(void) { return &data_; }


/*
* Serialize
*
* If (Serialize("/sdcard/pterodon/settings/boot_data"))
* printf("Serialized boot_data to /sdcard/Pterodon/settings/boot_data\n");
*
*/
bool Serialize(const std::string& path) const;
bool Unserialize(const std::string& path, bool check_version);

bool SaveToPropFile(const std::string& path) const;
bool LoadFromPropFile(const std::string& path);



private:
void Format(std::string &str) const;
bool WriteValue(
#ifdef PTERODON_DATABASE_USES_LOWLEVEL_CALLS
int fd,
#else
FILE* fp, 
#endif
const std::string& mValue) const;
bool ReadValue(
#ifdef PTERODON_DATABASE_USES_LOWLEVEL_CALLS
int fd,
#else
FILE* fp, 
#endif
std::string &mValue) const;

private:
std::unordered_map<std::string, std::string> data_;
};

} // namespace wrappers
} // namespace Pterodon

#endif // _PTERODON_DATABASE_HPP
		