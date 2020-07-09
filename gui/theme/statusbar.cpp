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

#include <time.h>
#include "statusbar.hpp"
#include "pterodon.h"
#include "aroma.h"
#include "constants.h"



namespace Pterodon {
namespace Gui {

/* statusbar */
void Pterodon::Gui::Statusbar::update(){
  word bgcolor = libaroma_alpha(
    Pterogui()->Statusbar_Bg_Color,
    0,51+Pterogui()->Statusbar_Side_Opa
  );
  byte isdark = libaroma_color_isdark(bgcolor);
  word text_color = isdark ? RGB(FFFFFF) : RGB(172018);
  
  libaroma_canvas_fillcolor(
    Pterogui()->Statusbar_Canvas,
    bgcolor
  );
    
  if (Pterogui()->Statusbar_Side_Width){
    if (Pterogui()->Statusbar_Overlay_Canvas){
      libaroma_draw_rect(
        Pterogui()->Statusbar_Canvas,
        0, 0, Pterogui()->Statusbar_Side_Width, Pterogui()->Statusbar_Canvas->h,
        0, 0x11
      );
      libaroma_draw_ex(
        Pterogui()->Statusbar_Canvas,
        Pterogui()->Statusbar_Overlay_Canvas,
        0, 0,
        Pterogui()->Statusbar_Overlay_Canvas->w-Pterogui()->Statusbar_Side_Width, 0,
        Pterogui()->Statusbar_Side_Width, Pterogui()->Statusbar_Overlay_Canvas->h,
        0, 0xcc
      );
    }
    else{
      libaroma_draw_rect(
        Pterogui()->Statusbar_Canvas,
        0, 0, Pterogui()->Statusbar_Side_Width, Pterogui()->Statusbar_Canvas->h,
        RGB(cccccc), RGB(172018)
      );
    }
  }
    
  libaroma_draw_text(
    Pterogui()->Statusbar_Canvas,
    Pterogui()->StatusbarCenteredText.c_str(),
    0,libaroma_dp(Pterogui()->Statusbar_dp - 18),text_color,Pterogui()->Statusbar_Canvas->w,
    LIBAROMA_FONT(0,3)|
    LIBAROMA_TEXT_CENTER,
    140
  );
  
  libaroma_draw_text(
    Pterogui()->Statusbar_Canvas,
    Pterogui()->StatusbarLeftText.c_str(),
    0,libaroma_dp(Pterogui()->Statusbar_dp - 18),text_color,Pterogui()->Statusbar_Canvas->w,
    LIBAROMA_FONT(0,3)|
    LIBAROMA_TEXT_LEFT,
    140
  );
  
  libaroma_draw_text(
    Pterogui()->Statusbar_Canvas,
    Pterogui()->StatusbarRightText.c_str(),
    0,libaroma_dp(Pterogui()->Statusbar_dp - 18),text_color,Pterogui()->Statusbar_Canvas->w,
    LIBAROMA_FONT(0,3)|
    LIBAROMA_TEXT_RIGHT,
    140
  );
  
  libaroma_wm_updatesync(0,0,0,0,1);
  libaroma_sync();
}

void Pterodon::Gui::Statusbar::sidebar_list_cb(
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
    Pterogui()->Statusbar_Force_Update=1;
  }
}

void Pterodon::Gui::Statusbar::SetColor(word color) {
  Pterogui()->Statusbar_Req_Color = color;
  if (Pterogui()->Statusbar_Req_Color != Pterogui()->Statusbar_Bg_Color){
    Pterogui()->Statusbar_Prv_Color = Pterogui()->Statusbar_Bg_Color;
    Pterogui()->Statusbar_Update_Start = libaroma_tick();
  }
}

 } // Gui namespace
} // Pterodon namespace


