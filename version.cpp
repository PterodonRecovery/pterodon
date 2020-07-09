/*
    Pterodon Recovery - version manager
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

#include <string>
#include <cstdio>
#include "version.hpp"
#include "pterodon.hpp"

static char _PTERODON_VERSION[50] = {0};

namespace Pterodon {

char* VersionManager::GetVersion() {
  if (_PTERODON_VERSION[0] == 0) {
    snprintf(_PTERODON_VERSION,
             50,
             "%i.%i.%i",
             PTERODON_VERSION_MAJOR,
             PTERODON_VERSION_MINOR,
             PTERODON_VERSION_MICRO
            );
  }
  return _PTERODON_VERSION;
}

std::string VersionManager::GetInfo(int info_type) {
  switch (info_type) {
    case PTERODON_INFO_NAME:
      return PTERODON_NAME;
      break;
      
    case PTERODON_INFO_VERSION:
      return std::string(GetVersion())  + " (Alpha)";
      break;
      
    case PTERODON_INFO_AUTHOR:
      return PTERODON_AUTHOR;
      break;
            
    case PTERODON_INFO_CODENAME:
      return PTERODON_VERSION_CODENAME;
      break;
      
    case PTERODON_INFO_YEAR:
      return PTERODON_VERSION_YEAR;
      break;
           
    case PTERODON_INFO_COPYRIGHT:
      return
        "Copyright (c) " PTERODON_VERSION_COPYRIGHT_YEAR " " PTERODON_AUTHOR;
  }
  return NULL;
}

} // namespace Pterodon

