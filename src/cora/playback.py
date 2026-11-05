"""Interfaces and state values shared by playback implementations."""

from enum import Enum
from typing import Protocol


class PlaybackState(Enum):
    """Represent the current state of a radio stream."""

    STOPPED = "stopped"
    LOADING = "loading..."
    PLAYING = "playing"
    PAUSED = "paused"
    ERROR = "error"


class Playback(Protocol):
    """Define the operations required by a radio-stream player."""

    def set_url(self, url: str) -> None:
        """Set the stream URL to play."""

    def play(self) -> None:
        """Start playing the configured stream."""

    def pause(self) -> None:
        """Pause the current stream."""

    def resume(self) -> None:
        """Resume a paused stream."""

    def stop(self) -> None:
        """Stop playback and release player resources."""

    def get_state(self) -> PlaybackState:
        """Return the current playback state."""

    def get_now_playing(self) -> str:
        """Return metadata describing the current programme, if available."""

    def get_error(self) -> str:
        """Return the most recent playback error, if any."""
