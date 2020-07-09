/*
    Pterodon Recovery - statusbar element
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

#ifndef _PTERODON_STATUSBAR_HPP_
#define _PTERODON_STATUSBAR_HPP_

#include "aroma.h"

#define SB_LEFT_SIDE 1
#define SB_CENTER_SIDE 2
#define SB_RIGHT_SIDE 3

#define SB_TIME 1
#define SB_DATE 2
#define SB_DATETIME 3
#define SB_BATTERY 4
#define SB_BATTERY_TEMPERATURE 5
#define SB_CPU_TEMPERATURE 6

namespace Pterodon {
namespace Gui {

class Statusbar {
public:
  /*
    * Function name: update
    * Description:
    * @return void: 
    */
    static void update();
  /*
    * Function name: ui_thread
    * Description: 
    * @return byte: 
    */
    static byte ui_thread();
  /*
    * Function name: sidebar_list_cb
    * Description: 
    * @return void: 
    */
    static void sidebar_list_cb(LIBAROMA_CONTROLP ctl,LIBAROMA_CANVASP cv,int sy);
  /*
    * Function name: SetColor
    * Description: 
    * @param color: 
    * @return void: 
    */
    static void SetColor(word color);
}; // class Statusbar

} // namespace Gui
} // namespace Pterodon

#endif  // _PTERODON_STATUSBAR_HPP_
