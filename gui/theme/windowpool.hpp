/*
    Pterodon Recovery - Wrapper for libaroma window pools
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

#ifndef _PHOENIX_WINDOW_POOL_HPP
#define _PHOENIX_WINDOW_POOL_HPP

#include "constants.h"
#include "aroma.h"

namespace Pterodon {
namespace Gui {

class WindowPooler {
public:
dword command;
byte cmd;
word id;
byte param;
WindowPooler() : command(0), cmd(0), id(0), param(0) {}
virtual ~WindowPooler() {}
void Pool(LIBAROMA_WINDOWP win, LIBAROMA_MSGP msg);
}; // class WindowPooler

} // namespace Gui
} // namespace Pterodon

#endif // _PHOENIX_WINDOW_POOL_HPP


