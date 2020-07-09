/*
    Pterodon Recovery - Debugging class for libaroma
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

#ifndef _PHOENIX_DEBUG_ENGINE_HPP
#define _PHOENIX_DEBUG_ENGINE_HPP


#include "constants.h"
#include "aroma.h"

namespace Pterodon {
namespace Gui {
namespace Debug {

class Engine {
public:
static void LogMessages(LIBAROMA_MSGP msg);
}; // class Engine
	
} // namespace Debug
} // namespace Gui
} // namespace Pterodon

#endif // _PHOENIX_DEBUG_ENGINE_HPP


