import os
import time
from threading import Thread

import vlc


class Player():

    def __init__(self):
        os.environ["VLC_VERBOSE"] = str("-1")
        self.player = None
        self.thread = None

    def set_url(self, url):
        #self.player = vlc.MediaPlayer(url)
        self.player = vlc.MediaPlayer('http://94.23.51.96:8001')

    def play(self):
        self.player.play()
        self.thread = Thread(target = self.run)
        self.thread.run()

    def run(self):
        previous = ""
        while True:
            time.sleep(10)
            meta = self.player.get_media().get_meta(12) # vlc.Meta 12: 'NowPlaying',
            if meta != previous:
                print(meta)
                previous = meta

    def stop(self):
        self.player().stop()
        self.thread.stop()
