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

#include "menu.h"
#include <conio.h>
#include <string.h>
#include <stdlib.h>

void Menu_draw(struct Menu *menu) {
    unsigned char i;

    for (i=0; i<menu->count; ++i) {
        revers(menu->selected == i);
        cprintf("%s\r\n", menu->list[i]);
    }
}

void Menu_add_item(struct Menu *menu, unsigned char *item) {
    unsigned char *new_item;
    new_item = (unsigned char*)calloc(strlen(item)+1, sizeof(unsigned char));
    strcpy(new_item, item);
    menu->list[menu->count] = new_item;
    menu->count += 1;
}

void Menu_cleanup(struct Menu *menu) {
    unsigned char i;

    for (i=0; i<menu->count; ++i) {
        free(menu->list[i]);
        menu->list[i] = NULL;
    }
    menu->count = 0;
}
