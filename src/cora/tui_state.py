"""State and transitions for the terminal user interface."""

from dataclasses import dataclass, field
from typing import Callable, Sequence

from cora.playback import PlaybackState
from cora.stations import Station


@dataclass
class TuiState:
    """Hold station selection, search, dialog, and playback display state."""

    all_stations: Sequence[Station]
    search: Callable[[Sequence[str]], Sequence[Station]]
    stations: list[Station] = field(init=False)
    keywords: list[str] = field(default_factory=list)
    selected: int = 0
    scroll: int = 0
    dialog: str | None = None
    search_mode: bool = False
    search_text: str = ""
    history: list[str] = field(default_factory=list)
    active_station: Station | None = None
    playback_state: PlaybackState = PlaybackState.STOPPED
    playback_error: str = ""

    def __post_init__(self) -> None:
        """Initialize the visible station list from all available stations."""
        self.stations = list(self.all_stations)

    @property
    def selected_station(self) -> Station | None:
        """Return the selected station, or ``None`` when the list is empty."""
        return self.stations[self.selected] if self.stations else None

    def move_selection(self, amount: int, visible_rows: int) -> None:
        """Move the selection and keep it within the visible list region."""
        if not self.stations or self.search_mode or self.dialog:
            return
        self.selected = min(max(self.selected + amount, 0), len(self.stations) - 1)
        if self.selected < self.scroll:
            self.scroll = self.selected
        elif self.selected >= self.scroll + visible_rows:
            self.scroll = self.selected - visible_rows + 1

    def apply_search(self) -> None:
        """Search for the entered terms and leave search mode."""
        self.keywords = self.search_text.split()
        self.stations = list(self.search(self.keywords))
        self._reset_selection()
        self.search_mode = False

    def clear_search(self) -> None:
        """Restore the complete station list and reset its selection."""
        self.keywords = []
        self.stations = list(self.all_stations)
        self._reset_selection()

    def begin_search(self) -> None:
        """Enter search mode unless a dialog is open."""
        if not self.dialog:
            self.search_text = ""
            self.search_mode = True

    def cancel_search(self) -> None:
        """Leave search mode without changing the displayed results."""
        self.search_mode = False
        self.search_text = ""

    def update_playback(self, state: PlaybackState, error: str, now_playing: str) -> None:
        """Update playback details and record new programme metadata."""
        self.playback_state = state
        self.playback_error = error
        if now_playing and now_playing not in self.history:
            self.history.insert(0, now_playing)
            del self.history[4:]

    def _reset_selection(self) -> None:
        """Select the first station and reset the list scroll position."""
        self.selected = 0
        self.scroll = 0
