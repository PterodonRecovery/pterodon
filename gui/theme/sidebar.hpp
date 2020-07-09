/*
    Pterodon Recovery - Sidebar element
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

#ifndef _PTERODON_SIDEBAR_HPP_
#define _PTERODON_SIDEBAR_HPP_

#include "aroma.h"

namespace Pterodon {
namespace Gui {

class Sidebar {
public:
  /*
    * Function name: List_CB
    * Description: 
    * @return void: 
    */
    static void List_CB(LIBAROMA_CONTROLP ctl,LIBAROMA_CANVASP cv,int sy);
  /*
    * Function name: OnSlide
    * Description: 
    * @param sb: 
    * @param x: 
    * @param w: 
    * @return void: 
    */
    static void OnSlide(LIBAROMA_WINDOWP sb,int x,int w);
  /*
    * Function name: init
    * Description: 
    * @return void: 
    */
    static void init(LIBAROMA_WINDOWP win);
}; // class Sidebar

} // namespace Gui
} // namespace Pterodon

#endif  // _PTERODON_SIDEBAR_HPP_
