#!/usr/bin/env python3

from threading import Thread
import gi
gi.require_version('Gst', '1.0')
from gi.repository import Gst, GLib

class Player():

    def __init__(self):
        Gst.init(None)
        self.player = Gst.ElementFactory.make("playbin", "player")
        fakesink = Gst.ElementFactory.make("fakesink", "fakesink")
        self.player.set_property("video-sink", fakesink)
        bus = self.player.get_bus()
        bus.add_signal_watch()
        bus.connect("message", self.on_message)

    def set_url(self, url):
        self.player.set_property("uri", url)

    def play(self):
        self.thread = Thread(target = self.run)
        self.thread.start()

    def run(self):
        self.player.set_state(Gst.State.PLAYING)
        # Init GObject loop to handle Gstreamer Bus Events
        self.loop = GLib.MainLoop()
        try:
            self.loop.run()
        except e:
            e.print_exc()
            self.loop.quit()

    def stop(self):
        self.player.set_state(Gst.State.PAUSED)
        self.loop.quit()
        self.thread.join()

    def on_message(self, bus, message):
        t = message.type
        if t == Gst.MessageType.EOS:
            self.player.set_state(Gst.State.NULL)
        elif t == Gst.MessageType.ERROR:
            self.player.set_state(Gst.State.NULL)
            err, debug = message.parse_error()
            print("Error: %s" % err, debug)
        elif t == Gst.MessageType.TAG:
            tags = message.parse_tag()
            title = tags.get_string('title')
            if title[0]:
                print(title[1])
