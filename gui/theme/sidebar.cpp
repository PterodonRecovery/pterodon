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
#include "engine.hpp"
#include "gui.hpp"
#include "sidebar.hpp"
#include "statusbar.hpp"
#include "control_id.h"
#include "pterodon.h"

namespace Pterodon {
namespace Gui {


void Pterodon::Gui::Sidebar::List_CB(
  LIBAROMA_CONTROLP ctl,LIBAROMA_CANVASP cv,int sy){
  if (Pterogui()->Statusbar_Overlay_Canvas){
    if ((Pterogui()->Statusbar_Overlay_Canvas->w!=cv->w)||
       (Pterogui()->Statusbar_Overlay_Canvas->h!=cv->h)){
      libaroma_canvas_free(Pterogui()->Statusbar_Overlay_Canvas);
    }
  }
  if (Pterogui()->Statusbar_Overlay_Canvas==NULL){
    Pterogui()->Statusbar_Overlay_Canvas=libaroma_canvas(
      cv->w, cv->h
    );
  }
  libaroma_draw(
    Pterogui()->Statusbar_Overlay_Canvas, cv, 0, 0, 0
  );
  if (Pterogui()->Statusbar_Side_Width){
    Pterogui()->Statusbar_Force_Update = 1;
  }
}

void Pterodon::Gui::Sidebar::OnSlide(LIBAROMA_WINDOWP sb,int x,int w){
  if (sb==Pterogui()->Sidebar){
    Pterogui()->Statusbar_Force_Update = 1;
    Pterogui()->Statusbar_Side_Width = x;
    int xs = (x*libaroma_dp(40))/w;
    if (xs<0){
      xs=0;
    }
    if (xs>libaroma_dp(40)){
      xs=libaroma_dp(40);
    }
    Pterogui()->Statusbar_Side_Opa=(byte) xs;
  }
}

void Pterodon::Gui::Sidebar::init(LIBAROMA_WINDOWP win) {
  /* init sidebar */
  Pterogui()->Sidebar = libaroma_window_sidebar(win,0);
  if (Pterogui()->Sidebar){
#if PTEROGUI_SHOW_STATUSBAR
    /* sidebar onslide callback */
    libaroma_window_sidebar_onslide(
      Pterogui()->Sidebar,
      &OnSlide
    );   
#endif    
    
  LIBAROMA_CONTROLP accentColorButton = libaroma_ctl_button(
    Pterogui()->Sidebar, 10, 0, 0, LIBAROMA_SIZE_FULL, 60,
    "Button one", LIBAROMA_CTL_BUTTON_COLORED, libaroma_colorget(NULL,win)->accent
  );
  
  LIBAROMA_CONTROLP backgroundColorButton = libaroma_ctl_button(
    Pterogui()->Sidebar, 11, 0, 60, LIBAROMA_SIZE_FULL, 60,
    "Button two", LIBAROMA_CTL_BUTTON_COLORED, libaroma_colorget(NULL,win)->accent
  );
  
   }
  }
 } // Gui namespace
} // Pterodon namespace
 
 
 
 