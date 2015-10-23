/*
 * NodeJS wrapper-plugin for Aqbanking
 * Copyright (C) 2015  Lukas Matt <lukas@zauberstuhl.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <gwenhywfar/cgui.h>
#include <gwenhywfar/gui_be.h>

#include "ab_gui_interface.h"

int CUSTOM_GETPASSWORD_FN (GWEN_GUI *, unsigned int flags, const char * token,
                           const char * title, const char * text, char * buffer,
                           int minLen, int maxLen, unsigned int id) {
  strcpy(buffer, "1234");
  return 0;
}

int CUSTOM_CHECKCERT_FN (GWEN_GUI *, const GWEN_SSLCERTDESCR * cd, GWEN_SYNCIO * sio, unsigned int id) {
  return 0;
}
