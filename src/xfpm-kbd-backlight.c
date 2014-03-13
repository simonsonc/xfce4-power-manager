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

#include <dbus/dbus-glib.h>

#include "xfpm-kbd-backlight.h"
#include "xfpm-button.h"

#include "org.freedesktop.UPower.KbdBacklight.h"

static void xfpm_kbd_backlight_finalize (GObject *object);

#define XFPM_KBD_BACKLIGHT_GET_PRIVATE(o) \
(G_TYPE_INSTANCE_GET_PRIVATE ((o), XFPM_TYPE_KBD_BACKLIGHT, XfpmKbdBacklightPrivate))

struct XfpmKbdBacklightPrivate
{
    DBusGConnection *bus;
    DBusGProxy      *proxy;

    XfpmButton      *button;

    gint             max_level;
};

G_DEFINE_TYPE (XfpmKbdBacklight, xfpm_kbd_backlight, G_TYPE_OBJECT)

static gint
get_max_brightness (XfpmKbdBacklight *self)
{
    GError *error = NULL;
    gint max_brightness;

    org_freedesktop_UPower_KbdBacklight_get_max_brightness (self->priv->proxy,
            &max_brightness, &error);
    if ( error )
    {
        g_warning ("Unable to get keyboard max brightness value: %s", error->message);
        g_error_free (error);
        return -1;
    }

    return max_brightness;
}

static void change_brightness (XfpmKbdBacklight *self, gint amount)
{
    GError *error = NULL;
    gint brightness;

    if (self->priv->max_level == -1)
        return;

    org_freedesktop_UPower_KbdBacklight_get_brightness (self->priv->proxy,
            &brightness, &error);

    if ( error )
    {
        g_warning ("Unable to get keyboard brightness value: %s", error->message);
        g_error_free (error);
        return;
    }

    brightness += amount;
    if ( (brightness > self->priv->max_level) || (brightness < 0) )
        return;

    org_freedesktop_UPower_KbdBacklight_set_brightness (self->priv->proxy,
            brightness, NULL);
}

static void
button_pressed_cb (XfpmButton *button, XfpmButtonKey type, XfpmKbdBacklight *self)
{
    if ( type == BUTTON_KBD_BRIGHTNESS_UP )
    {
        change_brightness (self, 1);
    }
    else if ( type == BUTTON_KBD_BRIGHTNESS_DOWN )
    {
        change_brightness (self, -1);
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
    GError *error = NULL;

    self->priv = XFPM_KBD_BACKLIGHT_GET_PRIVATE (self);

    self->priv->bus = NULL;
    self->priv->proxy = NULL;
    self->priv->button = NULL;
    self->priv->max_level = -1;

    self->priv->bus = dbus_g_bus_get (DBUS_BUS_SYSTEM, &error);

    if ( error )
    {
        g_critical ("Unable to get system bus connection : %s", error->message);
        g_error_free (error);
        return;
    }

    self->priv->proxy = dbus_g_proxy_new_for_name (self->priv->bus,
            "org.freedesktop.UPower",
            "/org/freedesktop/UPower/KbdBacklight",
            "org.freedesktop.UPower.KbdBacklight");

    self->priv->button = xfpm_button_new ();

    self->priv->max_level = get_max_brightness (self);

    g_signal_connect (self->priv->button, "button-pressed",
            G_CALLBACK (button_pressed_cb), self);
}

static void
xfpm_kbd_backlight_finalize (GObject *object)
{
    XfpmKbdBacklight *self = XFPM_KBD_BACKLIGHT (object);

    if ( self->priv->button )
        g_object_unref (self->priv->button);

    if ( self->priv->proxy )
        g_object_unref (self->priv->proxy);

    if ( self->priv->bus )
        dbus_g_connection_unref (self->priv->bus);

    G_OBJECT_CLASS (xfpm_kbd_backlight_parent_class)->finalize (object);
}

XfpmKbdBacklight *
xfpm_kbd_backlight_new (void)
{
    XfpmKbdBacklight *self = NULL;
    self = g_object_new (XFPM_TYPE_KBD_BACKLIGHT, NULL);
    return self;
}
