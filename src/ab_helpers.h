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

#ifndef UBHELPER_H
#define UBHELPER_H

#include <aqbanking/banking.h>

namespace UB {
  class Helper {
    private:
      AB_BANKING * ab;
      GWEN_GUI * gui;

      typedef struct {
        int * no;
        char * name;
        char * description;
        int * bank_code;
        char * bank_name;

        AB_BANKING *ab;
      } aqbanking_Account;

      int init(void);
    public:
      Helper() {
        init();
      }
      int close(void); // free
      int transactions(AB_ACCOUNT *);
      int list_accounts(void);
  };
}

#endif
