import logging
import os
import time
from threading import Thread

import vlc

logger = logging.getLogger(__name__)

class Player:

    def __init__(self):
        # turn of vlc output
        os.environ["VLC_VERBOSE"] = str("-1")
        self.player = None
        self.printTitleThread = None
        self.playing = False
        self.playThread = None

    def set_url(self, url):
        logger.debug("url:" + url)
        self.player = vlc.MediaPlayer(url)

    def play(self):
        self.playThread = Thread(target = self.player.play)
        self.playThread.start()
        self.playing = True
        # FIXME this blocks somehow
        self.printTitleThread = Thread(target = self.check_metadata)
        self.printTitleThread.start()

    def check_metadata(self):
        previous = ""
        while self.playing:
            time.sleep(5)
            logger.debug("checking stream metadata")
            meta = self.player.get_media().get_meta(12) # vlc.Meta 12: 'NowPlaying',
            if meta != previous:
                logger.info(meta)
                previous = meta
        logger.debug("check_metadata exited")

    def stop(self):
        self.playing = False

        if self.player is not None:
            self.player.stop()

        #if self.printTitleThread is not None:
        #    self.printTitleThread.join()
