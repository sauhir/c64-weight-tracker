/* Weight Tracker 64
 *
 * Copyright (C) 2019 Sauli Hirvi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef __menu_h_
#define __menu_h_

#include "defs.h"
void Menu_draw(struct Menu *);
void Menu_add_item(struct Menu *, unsigned char *);
void Menu_cleanup(struct Menu *);
#endif
