import logging
import os

import vlc

from cora.playback import PlaybackState

logger = logging.getLogger(__name__)


class Player:

    def __init__(self):
        # turn of vlc output
        os.environ["VLC_VERBOSE"] = str("-1")
        self.player = None
        self._state = PlaybackState.STOPPED
        self._error = ""
        self._now_playing = ""

    def set_url(self, url: str) -> None:
        if "\0" in url:
            self.stop()
            self._set_error("Station URL is invalid")
            return
        try:
            self.stop()
            logger.debug("url:%s", url)
            self.player = vlc.MediaPlayer(url)
            if self.player is None:
                raise RuntimeError("Unable to create media player")
            self._attach_state_events()
            self._error = ""
            self._state = PlaybackState.STOPPED
        except Exception as error:
            raise RuntimeError("Unable to initialize libVLC media player") from error

    def play(self) -> None:
        if self.player is None:
            return
        try:
            self._state = PlaybackState.LOADING
        except Exception:
            self._set_error("Unable to play this station")
            return
        try:
            result = self.player.play()
        except Exception:
            self._set_error("Unable to play this station")
            return
        if result == -1:
            self._set_error("Unable to play this station")
            return
        self._sync_state()

    def pause(self) -> None:
        if self.get_state() is PlaybackState.PLAYING:
            self.player.set_pause(1)
            self._sync_state()

    def resume(self) -> None:
        if self.get_state() is PlaybackState.PAUSED:
            self.player.set_pause(0)
            self._sync_state()

    def stop(self) -> None:
        if self.player is not None:
            try:
                self.player.stop()
                self.player.release()
            finally:
                self.player = None
        self._state = PlaybackState.STOPPED

    def get_state(self) -> PlaybackState:
        if self.player is not None and self._state is not PlaybackState.ERROR:
            self._sync_state()
        return self._state

    def get_now_playing(self) -> str:
        if self.player is None:
            return ""
        try:
            media = self.player.get_media()
            return media.get_meta(12) or "" if media is not None else ""
        except Exception:
            return ""

    def get_error(self) -> str:
        return self._error

    def _sync_state(self) -> None:
        try:
            state = self.player.get_state()
        except Exception as error:
            raise RuntimeError("Unable to query libVLC playback state") from error
        mapping = {
            vlc.State.Opening: PlaybackState.LOADING,
            vlc.State.Buffering: PlaybackState.LOADING,
            vlc.State.Playing: PlaybackState.PLAYING,
            vlc.State.Paused: PlaybackState.PAUSED,
            vlc.State.Stopped: PlaybackState.STOPPED,
            vlc.State.Ended: PlaybackState.STOPPED,
            vlc.State.Error: PlaybackState.ERROR,
        }
        self._state = mapping.get(state, self._state)
        if self._state is PlaybackState.ERROR:
            self._set_error("Unable to play this station")

    def _attach_state_events(self) -> None:
        try:
            events = self.player.event_manager()
            for event_type in (
                vlc.EventType.MediaPlayerOpening,
                vlc.EventType.MediaPlayerBuffering,
                vlc.EventType.MediaPlayerPlaying,
                vlc.EventType.MediaPlayerPaused,
                vlc.EventType.MediaPlayerStopped,
                vlc.EventType.MediaPlayerEndReached,
                vlc.EventType.MediaPlayerEncounteredError,
            ):
                events.event_attach(event_type, self._on_state_event)
        except Exception as error:
            raise RuntimeError("Unable to monitor libVLC playback state") from error

    def _on_state_event(self, _event) -> None:
        try:
            self._sync_state()
        except RuntimeError:
            logger.exception("Unable to query libVLC playback state")

    def _set_error(self, message: str) -> None:
        self._error = message
        self._state = PlaybackState.ERROR
        if self.player is not None:
            try:
                self.player.stop()
                self.player.release()
            finally:
                self.player = None
