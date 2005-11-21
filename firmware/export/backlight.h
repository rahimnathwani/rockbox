/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (C) 2002 by Daniel Stenberg
 *
 * All files in this archive are subject to the GNU General Public License.
 * See the file COPYING in the source tree root for full license agreement.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/
#ifndef BACKLIGHT_H
#define BACKLIGHT_H

#include "config.h"

void backlight_on(void);
void backlight_off(void);
void backlight_set_timeout(int index);
#ifdef CONFIG_BACKLIGHT
void backlight_init(void);
int  backlight_get_timeout(void);
#if CONFIG_BACKLIGHT == BL_IRIVER_H100
void backlight_set_fade_in(int index);
void backlight_set_fade_out(int index);
#endif
bool backlight_get_on_when_charging(void);
void backlight_set_on_when_charging(bool yesno);
extern const char backlight_timeout_value[];
#else
#define backlight_init()
#endif

#ifdef HAVE_REMOTE_LCD
void remote_backlight_on(void);
void remote_backlight_off(void);
void remote_backlight_set_timeout(int index);
void remote_backlight_set_on_when_charging(bool yesno);
#endif

#ifdef SIMULATOR
void sim_backlight(int value);
void sim_remote_backlight(int value);
#endif
#endif
