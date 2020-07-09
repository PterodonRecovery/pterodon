/*
    Pterodon Recovery - Wrapper for libaroma window pools
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

#include "engine.hpp"
#include "constants.h"
#include "aroma.h"
#include "engine_debug.hpp"
#include "pterodon.h"
#include "windowpool.hpp"
#include "device_driver.hpp"
#include "gui.hpp"
#include "settings.hpp"

namespace Pterodon {
namespace Gui {
	
void WindowPooler::Pool(LIBAROMA_WINDOWP win, LIBAROMA_MSGP msg) {
do {
command = libaroma_window_pool(win,msg);
cmd = LIBAROMA_CMD(command);
id = LIBAROMA_CMD_ID(command);
param = LIBAROMA_CMD_PARAM(command);
if (msg->msg == LIBAROMA_MSG_WIN_POWERMENU) {
     if (libaroma_wm()->power_menu_window != NULL)
          libaroma_wm()->power_menu_window();
     } else {
    if (cmd==LIBAROMA_CMD_CLICK && Pterodon::SettingsManager::GetIntVar("pterodon_vibrate_on_touch_check") == 1) {
     Pterodon::DeviceDriver::Vibrate(20);
     }
     break;
     }
   } while(true);
 }

} // namespace Gui
} // namespace Pterodon


