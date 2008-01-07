/* ASE - Allegro Sprite Editor
 * Copyright (C) 2001, 2002, 2003, 2004, 2005, 2007,
 *               2008  David A. Capello
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "config.h"

#ifndef USE_PRECOMPILED_HEADER

#include <assert.h>
#include <stdio.h>

#include "jinete/jinete.h"

#include "modules/color.h"
#include "modules/gui.h"
#include "modules/palette.h"
#include "widgets/colbar.h"
#include "widgets/paledit.h"

#endif

static int paledit_change_signal(JWidget widget, int user_data);
static bool window_hook(JWidget widget, JMessage msg);

void ji_minipal_new(JWidget color_bar, int x, int y)
{
  JWidget window, paledit;

  window = jwindow_new("MiniPal");
  paledit = palette_editor_new(current_palette, FALSE, 3);

  HOOK(paledit, SIGNAL_PALETTE_EDITOR_CHANGE, paledit_change_signal, color_bar);
  jwidget_add_hook(window, JI_USER_WIDGET, window_hook, paledit);

  jwidget_expansive(paledit, TRUE);
  jwidget_add_child(window, paledit);

  jwindow_position(window, x, y);
  jwindow_open_bg(window);
}

static int paledit_change_signal(JWidget widget, int user_data)
{
  if (jmouse_b(0)) {
    PaletteEditor *paledit = palette_editor_data(widget);
    JWidget color_bar = (JWidget)user_data;

    if (paledit->color[0] == paledit->color[1]) {
      char *color = color_index(paledit->color[1]);

      color_bar_set_color(color_bar,
			  jmouse_b(0) == 1 ? 0: 1,
			  color, FALSE);

      jfree(color);
    }
    else {
      bool array[256];
      char buf[256];
      int c, sel;

      palette_editor_get_selected_entries(widget, array);
      for (c=sel=0; c<256; c++)
	if (array[c])
	  sel++;

      color_bar_set_size(color_bar, sel);

      for (c=sel=0; c<256; c++) {
	if (array[c]) {
	  sprintf(buf, "index{%d}", c);
	  color_bar_set_color_directly(color_bar, sel++, buf);
	}
      }
    }
  }
  return FALSE;
}

static bool window_hook(JWidget widget, JMessage msg)
{
  switch (msg->type) {

    case JM_SIGNAL: {
      if (msg->signal.num == JI_SIGNAL_WINDOW_RESIZE) {
	JWidget paledit = jwidget_get_data(widget, JI_USER_WIDGET);
	int cols, box = 3;

	do {
	  box++;
	  palette_editor_data(paledit)->boxsize = box;
	  cols = (jrect_w(paledit->rc)-1) / (box+1);
	  palette_editor_set_columns(paledit, cols);
	} while (((jrect_h(paledit->rc)-1) / (box+1))*cols > 256);

	box--;
	palette_editor_data(paledit)->boxsize = box;
	cols = (jrect_w(paledit->rc)-1) / (box+1);
	palette_editor_set_columns(paledit, cols);

	jwidget_dirty(paledit);
      }
      else if (msg->signal.num == JI_SIGNAL_WINDOW_CLOSE) {
	jwidget_free_deferred(widget);
      }
      break;
    }

  }

  return FALSE;
}
