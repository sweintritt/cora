import logging
import os
import threading

import vlc

logger = logging.getLogger(__name__)

class Player:

    def __init__(self):
        # turn of vlc output
        os.environ["VLC_VERBOSE"] = str("-1")
        self.player = None
        self.play_thread = None
        self.metadata_thread = None
        self.playing = None
        self.timer = None
        self.playing_title = None

    def set_url(self, url):
        logger.debug("url:" + url)
        self.player = vlc.MediaPlayer(url)

    def play(self):
        self.play_thread = threading.Thread(target = self.player.play)
        self.play_thread.start()
        self.playing = threading.Event()
        self.metadata_thread = threading.Timer(5, self.check_metadata)
        self.metadata_thread.start()

    def check_metadata(self):
        while not self.playing.wait(5):
            logger.debug("checking stream metadata")
            meta = self.player.get_media().get_meta(12) # vlc.Meta 12: 'NowPlaying',
            if meta != self.playing_title:
                logger.info(meta)
                self.playing_title = meta

    def stop(self):
        self.playing.set()

        if self.player is not None:
            self.player.stop()

        if self.metadata_timer is not None:
            self.metadata_stop()
