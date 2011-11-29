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

import gconf
import gtk
import logging
import os

from gettext import gettext as _

from sugar.activity import activity
from sugar.graphics.toolbarbox import ToolbarBox
from sugar.activity.widgets import ActivityButton
from sugar.activity.widgets import ActivityToolbox
from sugar.activity.widgets import TitleEntry
from sugar.activity.widgets import StopButton
from sugar.activity.widgets import ShareButton

class MouseCamActivity(activity.Activity):
    """MouseCamActivity class as specified in activity.info"""

    def cb_change_event(self, adj):
        #manage here the change in the slide bar ;)
        #persist here in order to notify gconfself.
        self.client.set_float('/apps/mousecam/adj', adj.value)
        print(adj.value)
        print("pase")
    
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

        #register destroy callback
        self.connect("destroy", self.cb_cleanup)
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
        toolbar_box.show()

        # label with the text, make the string translatable
        #label = gtk.Label(_("Deslize la barra para enlentecer el mouse"))
        #self.set_canvas(label)
        #label.show()

        # And, one last HScale widget for adjusting the mouse speed
        box1 = gtk.VBox(False, 0)
        self.set_canvas(box1)
        box1.show()

        box2 = gtk.HBox(False, 100)
        box2.set_border_width(10)
        box1.pack_start(box2, True, True, 0)
        box2.show()

        # value, lower, upper, step_increment, page_increment, page_size
        # Note that the page_size value only makes a difference for
        # scrollbar widgets, and the highest value you'll get is actually
        # (upper - page_size).
        adj1 = gtk.Adjustment(0.0, 0.0, 101.0, 0.1, 1.0, 1.0)
        adj1.connect("value_changed", self.cb_change_event)
        box3 = gtk.VBox(False, 10)
        box2.pack_start(box3, True, True, 0)
        box3.show()

        # Reuse the same adjustment
        self.hscale = gtk.HScale(adj1)
        self.hscale.set_size_request(600, 90)
        ###scale_set_default_values(self.hscale)
        box3.pack_start(self.hscale, True, True, 0)
        self.hscale.show()


        box2 = gtk.HBox(False, 10)
        box2.set_border_width(10)
        box1.pack_start(box2, True, True, 0)
        box2.show()

  
        box2 = gtk.HBox(False, 10)
        box2.set_border_width(10)

        # An option menu to change the position of the value
        label = gtk.Label("Deslize la barra para enlentecer el mouse:")
        box3.pack_start(label, False, False, 0)
        label.show()
  
        menu = gtk.Menu()
  
        self.window.show()
        os.system("bin/execute.sh &")

