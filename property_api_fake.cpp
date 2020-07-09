/*
    Pterodon Recovery - Fake property API functions
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

#include "property_api_fake.hpp"
#include "DataBase.hpp"

static Pterodon::wrappers::DataBase data;

int property_get(const char *key, char *value, const char *default_value)
{
	if (strlen(key) > PROPERTY_KEY_MAX)
	return -1;
	
	std::string local_value;
	int ret = data.GetVar(key, local_value) ? 0 : -1;
	if (ret == -1) {
	strcpy(value, default_value);
	return strlen(default_value);
	} else {
	strcpy(value, local_value.c_str());
	return (int)local_value.size();
	}
}
	
int property_set(const char *key, const char *value)
{
	if (strlen(key) > PROPERTY_KEY_MAX || 
        strlen(value) > PROPERTY_VALUE_MAX)
	    return -1;
	
	data.SetVar(key, value);
	return 0;
}
