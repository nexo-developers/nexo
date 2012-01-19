# Copyright 2011 NEXO fing UdelaR based on Hello World activity
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

"""HelloWorld Activity: A case study for developing an activity."""

import gtk
import logging
import math
import os

try:
    import gconf
except ImportError:
    import sys
    sys.path.append('lib/python') #revisar si no falta ./ 
    import gconf

from gettext import gettext as _

from sugar.activity import activity
try:  # 0.86 toolbar widgets
    from sugar.activity.widgets import ActivityToolbarButton, ActivityButton, ActivityToolbox, TitleEntry, StopButton, ShareButton
    from sugar.graphics.toolbarbox import ToolbarBox
    HAS_TOOLBARBOX = True
except ImportError:
    HAS_TOOLBARBOX = False

NOT_DEFINED = 0

class MouseCamActivity(activity.Activity):
    """MouseCamActivity class as specified in activity.info"""

    def cb_change_event(self, adj):
        #manage here the change in the slide bar ;)
        #persist here in order to notify gconfself.
        self.client.set_float('/apps/mousecam/adj', adj.value)
        print('adj: ',adj.value)
        print("pase")
        
    def cb_threshold_change_event(self, adj):
        #manage here the change in the slide bar ;)
        #persist here in order to notify gconfself.
        aux = math.trunc(adj.value)
        self.client.set_int('/apps/mousecam/threshold', aux)
        print('humbral: ',adj.value)
        print("pase threshold")

    def cb_keypad_events(self, widget):
        self.client.set_bool('/apps/mousecam/keypad_events', (False, True)[widget.get_active()])
        print('keypad_events: ', (False, True)[widget.get_active()])
        
    def cb_returnkey_event(self, widget):
        self.client.set_bool('/apps/mousecam/return_key_instead_click', (False, True)[widget.get_active()])
        print('return_key_instead_click: ', (False, True)[widget.get_active()])

    def cb_cleanup(self, arg):
        #aca hay que usar algun hacking para matar el proceso o mandar un mensaje a mousecam para que se cierre
        os.system("pkill MouseCam")
        

    # makes the sample window

    def __init__(self, handle):
        self.client = gconf.client_get_default()
        """Set up the MouseCam activity."""
        activity.Activity.__init__(self, handle)

        # we do not have collaboration features
        # make the share option insensitive
        self.max_participants = 1

        self.has_toolbarbox = HAS_TOOLBARBOX

        #register destroy callback
        self.connect("destroy", self.cb_cleanup)

        if(self.has_toolbarbox):
            # toolbar with the new toolbar redesign
            toolbar_box = ToolbarBox()
            activity_button = ActivityButton(self)
            toolbar_box.toolbar.insert(activity_button, 0)
            activity_button.show()

            title_entry = TitleEntry(self)
            toolbar_box.toolbar.insert(title_entry, -1)
            title_entry.show()

            share_button = ShareButton(self)
            toolbar_box.toolbar.insert(share_button, -1)
            share_button.show()

            separator = gtk.SeparatorToolItem()
            separator.props.draw = False
            separator.set_expand(True)
            toolbar_box.toolbar.insert(separator, -1)
            separator.show()

            stop_button = StopButton(self)
            toolbar_box.toolbar.insert(stop_button, -1)
            stop_button.show()
            self.set_toolbar_box(toolbar_box)

        else:
            # toolbar with the old toolbar design
            toolbar_box = activity.ActivityToolbox(self)
            self.set_toolbox(toolbar_box)

        toolbar_box.show()
        
        # Set default values for keypad and returnkey events
        self.client.set_bool('/apps/mousecam/keypad_events', False)
        self.client.set_bool('/apps/mousecam/return_key_instead_click', False)

        # And, one last HScale widget for adjusting the mouse speed
        box1 = gtk.VBox(False, 0)
        box1.set_border_width(10)
        self.set_canvas(box1)
        box1.show()

        # value, lower, upper, step_increment, page_increment, page_size
        # Note that the page_size value only makes a difference for
        # scrollbar widgets, and the highest value you'll get is actually
        # (upper - page_size).
        gconf_persist_value = self.client.get_float('/apps/mousecam/adj')
        if(gconf_persist_value == NOT_DEFINED):
            slide_vel_start_value = 50.0
            self.client.set_float('/apps/mousecam/adj', slide_vel_start_value)
        else:
            slide_vel_start_value = gconf_persist_value         
        adj1 = gtk.Adjustment(slide_vel_start_value, 1.0, 101.0, 0.1, 1.0, 1.0)
        adj1.connect("value_changed", self.cb_change_event)
        box1_1 = gtk.VBox(False, 10)
        box1.pack_start(box1_1, True, True, 0)
        box1_1.show()

        gconf_persist_value = self.client.get_int('/apps/mousecam/threshold')
        if(gconf_persist_value == NOT_DEFINED):
            slide_threshold_start_value = 100
            self.client.set_int('/apps/mousecam/threshold', slide_threshold_start_value)
        else:
            slide_threshold_start_value = gconf_persist_value
        adj2 = gtk.Adjustment(slide_threshold_start_value, 0, 256, 1.0, 1.0, 1.0)
        adj2.connect("value_changed", self.cb_threshold_change_event)
        box1_2 = gtk.VBox(False, 10)
        box1.pack_start(box1_2, True, True, 0)
        box1_2.show()

        # Reuse the same adjustment
        self.hscale = gtk.HScale(adj1)
        self.hscale.set_size_request(600, 90)
        ###scale_set_default_values(self.hscale)
        box1_1.pack_start(self.hscale, True, True, 0)
        self.hscale.show()
        
        
        self.hscale2 = gtk.HScale(adj2)
        self.hscale2.set_digits(0)
        self.hscale2.set_size_request(600, 90)
        box1_2.pack_start(self.hscale2, True, True, 0)
        self.hscale2.show()
        


        # An option menu to change the position of the value
        #label = gtk.Label(_('Deslice la barra para enlentecer el mouse:'))
        label = gtk.Label(_('Slide the bar to slow the mouse speed:'))
        box1_1.pack_start(label, False, False, 0)
        label.show()
  
        label = gtk.Label(_('Slide the bar to decrease the threshold:'))
        box1_2.pack_start(label, False, False, 0)
        label.show()
        
        
        # Check button for mouse / keypad event switch
        button_keypad = gtk.CheckButton(_('Enable keypad events'))
        button_keypad.connect("toggled", self.cb_keypad_events)
        button_keypad.show()

        # Check button for click / return key event switch
        button_returnkey = gtk.CheckButton(_('Enable the return key in place of the mouse click'))
        button_returnkey.connect("toggled", self.cb_keypad_events)
        button_returnkey.show()


        box1_3 = gtk.VBox(False, 10)
        box1_3.show()
        box1_3.pack_start(button_keypad, False, False, 2)
        box1_3.pack_start(button_returnkey, False, False, 2)
        
        box1.pack_start(box1_3, True, True, 0)
        
        

        menu = gtk.Menu()
  
        self.window.show()
        os.system("bin/execute.sh &")



