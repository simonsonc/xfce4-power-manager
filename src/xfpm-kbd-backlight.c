/*
 * * Copyright (C) 2014 Carl Simonson <simonsonc@gmail.com>
 *
 * Licensed under the GNU General Public License Version 2
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "xfpm-kbd-backlight.h"
#include "xfpm-button.h"

static void xfpm_kbd_backlight_finalize (GObject *object);

#define XFPM_KBD_BACKLIGHT_GET_PRIVATE(o) \
(G_TYPE_INSTANCE_GET_PRIVATE ((o), XFPM_TYPE_KBD_BACKLIGHT, XfpmKbdBacklightPrivate))

struct XfpmKbdBacklightPrivate
{
    XfpmButton      *button;
};

G_DEFINE_TYPE (XfpmKbdBacklight, xfpm_kbd_backlight, G_TYPE_OBJECT)

static void
button_pressed_cb (XfpmButton *button, XfpmButtonKey type, XfpmKbdBacklight *self)
{
    if ( type == BUTTON_KBD_BRIGHTNESS_UP )
    {
    }
    else if ( type == BUTTON_KBD_BRIGHTNESS_DOWN )
    {
    }
}

static void
xfpm_kbd_backlight_class_init (XfpmKbdBacklightClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = xfpm_kbd_backlight_finalize;

    g_type_class_add_private (klass, sizeof (XfpmKbdBacklightPrivate));
}

static void
xfpm_kbd_backlight_init (XfpmKbdBacklight *self)
{
    self->priv = XFPM_KBD_BACKLIGHT_GET_PRIVATE (self);

    self->priv->button = xfpm_button_new ();
}

static void
xfpm_kbd_backlight_finalize (GObject *object)
{
    XfpmKbdBacklight *self = XFPM_KBD_BACKLIGHT (object);

    if ( self->priv->button )
        g_object_unref (self->priv->button);

    g_signal_connect (self->priv->button, "button-pressed",
            G_CALLBACK (button_pressed_cb), self);

    G_OBJECT_CLASS (xfpm_kbd_backlight_parent_class)->finalize (object);
}

XfpmKbdBacklight *
xfpm_kbd_backlight_new (void)
{
    XfpmKbdBacklight *self = NULL;
    self = g_object_new (XFPM_TYPE_KBD_BACKLIGHT, NULL);
    return self;
}
