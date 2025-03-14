import logging
import os
import time
from threading import Thread

import vlc

logger = logging.getLogger(__name__)

class Player:

    def __init__(self):
        os.environ["VLC_VERBOSE"] = str("-1")
        self.player = None
        self.printTitleThread = None
        self.playThread = None

    def set_url(self, url):
        logger.debug("url:" + url)
        self.player = vlc.MediaPlayer(url)

    def play(self):
        self.playThread = Thread(target = self.player.play)
        self.playThread.run()
        self.printTitleThread = Thread(target = self.run)
        self.printTitleThread.run()

    def run(self):
        previous = ""
        while True:
            time.sleep(10)
            meta = self.player.get_media().get_meta(12) # vlc.Meta 12: 'NowPlaying',
            if meta != previous:
                logger.info(meta)
                previous = meta

    def stop(self):
        if self.player is not None:
            self.player().stop()

        if self.printTitleThread is not None:
            self.printTitleThread.stop()

        if self.playThread is not None:
            self.playThread.stop()
