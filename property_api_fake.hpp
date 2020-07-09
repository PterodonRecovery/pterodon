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

#ifndef _PTERODON_FAKE_PROPERTYAPI_HPP
#define _PTERODON_FAKE_PROPERTYAPI_HPP

#ifndef PROPERTY_KEY_MAX
#define PROPERTY_KEY_MAX 32
#endif

#ifndef PROPERTY_VALUE_MAX
#define PROPERTY_VALUE_MAX 92
#endif

int property_get(const char *key, char *value, const char *default_value);
int property_set(const char *key, const char *value);

#endif // _PTERODON_FAKE_PROPERTYAPI_HPP

