"""Textual terminal interface for browsing and playing radio stations."""

import random
import shutil
import sqlite3

from rich.markup import escape

from textual.app import App, ComposeResult
from textual.containers import Horizontal, Vertical
from textual.widgets import Footer, Input, Static

from cora.playback import Playback, PlaybackState
from cora.settings import Settings
from cora.stations import Station, Stations
from cora.tui_state import TuiState

MIN_COLUMNS = 80
MIN_LINES = 24
LAST_PLAYED = "last.played"


def terminal_is_large_enough() -> bool:
    """Return whether the terminal meets the TUI's minimum dimensions."""
    size = shutil.get_terminal_size(fallback=(0, 0))
    return size.columns >= MIN_COLUMNS and size.lines >= MIN_LINES


def print_minimum_size_message() -> None:
    """Print the terminal dimensions required to run the TUI."""
    print(f"cora tui requires a terminal of at least {MIN_COLUMNS} columns by {MIN_LINES} rows.")


class CoraApp(App[None]):
    """Provide the interactive terminal interface for Cora."""

    CSS = """
    Screen { background: #282C34; color: #ABB2BF; }
    #station-list, #playing, #history, #status { border: round #ABB2BF; }
    #station-list { height: 1fr; }
    #panels { height: 7; }
    #playing, #history { width: 1fr; height: 1fr; padding: 0 1; }
    #status { height: 3; color: #56B6C2; padding: 0 1; }
    .selected { color: #61AFEF; text-style: bold; }
    .muted { color: #5C6370; }
    .playing { color: #98C379; }
    .loading { color: #E5C07B; }
    .paused { color: #C678DD; }
    .error { color: #E06C75; }
    #search { display: none; }
    """

    BINDINGS = [
        ("up", "previous", "Previous station"),
        ("k", "previous", "Previous station"),
        ("down", "next", "Next station"),
        ("j", "next", "Next station"),
        ("slash", "search", "Search"),
        ("c", "clear_search", "Clear search"),
        ("enter", "play", "Play"),
        ("space", "pause_resume", "Pause/resume"),
        ("r", "random_station", "Random"),
        ("i", "details", "Details"),
        ("h", "help", "Help"),
        ("q", "quit", "Quit"),
    ]

    def __init__(self, stations: Stations, settings: Settings, player: Playback) -> None:
        """Create the application with its station repository, settings, and player."""
        super().__init__()
        all_stations = stations.find_all_by_keywords([])
        self.repository = stations
        self.settings = settings
        self.player = player
        self.state = TuiState(all_stations, stations.find_all_by_keywords)
        self._saved_playing_id: int | None = None
        self._last_metadata_poll = 0.0
        self.status_message = ""

    def compose(self) -> ComposeResult:
        """Compose the station list, playback panels, search input, and footer."""
        station_list = Static(id="station-list")
        station_list.border_title = "cora"
        yield station_list
        with Horizontal(id="panels"):
            playing = Static(id="playing")
            playing.border_title = PlaybackState.STOPPED.value
            yield playing
            history = Static(id="history")
            history.border_title = "history"
            yield history
        with Vertical(id="status"):
            yield Input(placeholder="Search stations", id="search")
            yield Static(id="hint")
        yield Footer()

    def on_mount(self) -> None:
        """Start periodic refreshes and render the initial interface."""
        self.set_interval(0.25, self._refresh)
        self.call_after_refresh(self.set_focus, None)
        self._render()

    def on_resize(self) -> None:
        """Re-render the interface after the terminal dimensions change."""
        self._render()

    def on_input_submitted(self, event: Input.Submitted) -> None:
        """Apply the submitted station search, reporting temporary database locks."""
        self.state.search_text = event.value
        try:
            self.state.apply_search()
        except sqlite3.OperationalError as error:
            if self._is_busy_error(error):
                self.status_message = "Database is busy; try again."
            else:
                raise
            return
        self.query_one("#search", Input).display = False
        self.set_focus(None)
        self._render()

    def on_key(self, event) -> None:
        """Handle keys that take precedence over Textual's bound actions."""
        if event.key == "ctrl+c":
            event.stop()
            self.exit()
            return
        if self.state.dialog:
            if event.key in ("escape", "h"):
                self.state.dialog = None
                self._render()
            event.stop()
            return
        if self.state.search_mode and event.key == "escape":
            self._leave_search(cancel=True)
            event.stop()

    def action_previous(self) -> None:
        """Select the preceding station."""
        self.state.move_selection(-1, self._visible_rows())
        self._render()

    def action_next(self) -> None:
        """Select the next station."""
        self.state.move_selection(1, self._visible_rows())
        self._render()

    def action_search(self) -> None:
        """Enter station search mode and focus the search input."""
        self.state.begin_search()
        if self.state.search_mode:
            search = self.query_one("#search", Input)
            search.display = True
            search.value = ""
            search.focus()
            self._render()

    def action_clear_search(self) -> None:
        """Clear an active station search."""
        if not self.state.search_mode and not self.state.dialog:
            self.state.clear_search()
            self._render()

    def action_play(self) -> None:
        """Start playback of the selected station."""
        station = self.state.selected_station
        if station is None or self.state.search_mode or self.state.dialog:
            return
        if not station.urls:
            self.state.update_playback(PlaybackState.ERROR, "Station has no URL", "")
        else:
            self.player.set_url(station.urls[0])
            self.player.play()
            self.state.active_station = station
            self._saved_playing_id = None
            self.state.update_playback(self.player.get_state(), self.player.get_error(), "")
        self._render()

    def action_pause_resume(self) -> None:
        """Pause active playback or resume paused playback."""
        if self.state.search_mode or self.state.dialog:
            return
        state = self.player.get_state()
        if state is PlaybackState.PLAYING:
            self.player.pause()
        elif state is PlaybackState.PAUSED:
            self.player.resume()
        self._render()

    def action_random_station(self) -> None:
        """Select and play a random station from the visible list."""
        if self.state.search_mode or self.state.dialog or not self.state.stations:
            return
        self.state.selected = random.randrange(len(self.state.stations))
        self.state.scroll = max(0, self.state.selected - self._visible_rows() + 1)
        self.action_play()

    def action_details(self) -> None:
        """Show details for the selected station."""
        if self.state.selected_station and not self.state.search_mode and not self.state.dialog:
            self.state.dialog = "details"
            self._render()

    def action_help(self) -> None:
        """Show the keyboard shortcut help dialog."""
        if not self.state.search_mode and not self.state.dialog:
            self.state.dialog = "help"
            self._render()

    def action_quit(self) -> None:
        """Exit the application when no overlay is active."""
        if not self.state.search_mode and not self.state.dialog:
            self.exit()

    def _leave_search(self, cancel: bool) -> None:
        """Hide the search input, optionally discarding its pending query."""
        if cancel:
            self.state.cancel_search()
        self.query_one("#search", Input).display = False
        self.set_focus(None)
        self._render()

    def _refresh(self) -> None:
        """Poll playback state and metadata, persist playback, and re-render."""
        if not terminal_is_large_enough():
            self._render()
            return
        state = self.player.get_state()
        metadata = self.player.get_now_playing() if self._last_metadata_poll <= 0 else ""
        self._last_metadata_poll += 0.25
        if self._last_metadata_poll >= 15:
            self._last_metadata_poll = 0
        self.state.update_playback(state, self.player.get_error(), metadata)
        if state is PlaybackState.PLAYING and self.state.active_station and self._saved_playing_id != self.state.active_station.id:
            self.settings.save(LAST_PLAYED, self.state.active_station.id)
            self._saved_playing_id = self.state.active_station.id
        self._render()

    def _render(self) -> None:
        """Render the current application state or the terminal-size warning."""
        if not terminal_is_large_enough():
            self.query_one("#station-list", Static).update(
                f"cora tui requires a terminal of at least {MIN_COLUMNS} columns by {MIN_LINES} rows."
            )
            self.query_one("#playing", Static).update("")
            self.query_one("#history", Static).update("")
            self.query_one("#hint", Static).update("Resize the terminal or press Ctrl-C to quit.")
            return
        if self.state.dialog:
            self._render_dialog()
            return
        self._render_list()
        self._render_playback()
        self.query_one("#hint", Static).update(
            "Search: " + self.state.search_text if self.state.search_mode
            else self.status_message or "Press 'h' for help"
        )

    def _render_list(self) -> None:
        """Render the visible stations or an appropriate empty-list message."""
        if not self.state.stations:
            text = ("No stations are available.\n\nImport stations first:\n\n"
                    "  cora import\n\nPress [q] to quit." if not self.state.all_stations
                    else "no results found")
        else:
            width = max(12, (self.size.width - 12) // 3)
            rows = []
            for index, station in enumerate(self.state.stations[self.state.scroll:self.state.scroll + self._visible_rows()], self.state.scroll):
                marker = ">" if index == self.state.selected else " "
                row = (f"{marker} {index + 1:>3}. {escape(self._truncate(station.name, width)):<{width}} "
                       f"{escape(self._truncate(station.genre, width)):<{width}} "
                       f"{escape(self._truncate(station.country, width)):<{width}}")
                rows.append(f"[#61AFEF]{row}[/]" if index == self.state.selected else row)
            text = "\n".join(rows)
        self.query_one("#station-list", Static).update(text)

    def _render_playback(self) -> None:
        """Render playback status, current metadata, and metadata history."""
        state = self.state.playback_state
        station = escape(self.state.active_station.name) if self.state.active_station else ""
        current = escape(self.state.history[0]) if self.state.history else ""
        error = escape(self.state.playback_error) if state is PlaybackState.ERROR else ""
        playing = self.query_one("#playing", Static)
        playing.border_title = state.value
        playing.update(
            f"[{self._state_class(state)}]{state.value}[/]\n{station}\n{error or current}"
        )
        history = self.state.history + [""] * (4 - len(self.state.history))
        self.query_one("#history", Static).update("\n".join(escape(entry) for entry in history))

    def _render_dialog(self) -> None:
        """Render the station-details or keyboard-shortcuts dialog."""
        station = self.state.selected_station
        if self.state.dialog == "details" and station:
            content = (f"name: {escape(station.name)}\nid: {station.id}\ngenre: {escape(station.genre)}\n"
                       f"country: {escape(station.country)}\nlanguage: {escape(station.language)}\n"
                       f"description: {escape(station.description or '-')}\n\n[h] or Escape to close")
        else:
            content = ("Up/k Down/j: select    /: search    c: clear search\n"
                       "Enter: play    Space: pause/resume    r: random\n"
                       "i: details    q: quit\n\n[h] or Escape to close")
        self.query_one("#station-list", Static).update(content)
        self.query_one("#playing", Static).update("")
        self.query_one("#history", Static).update("")
        self.query_one("#hint", Static).update("Dialog")

    def _visible_rows(self) -> int:
        """Return the number of station rows that fit in the terminal."""
        return max(1, self.size.height - 13)

    @staticmethod
    def _truncate(value: str, width: int) -> str:
        """Truncate text to ``width`` characters, adding an ellipsis if needed."""
        return value if len(value) <= width else value[:max(0, width - 1)] + "…"

    @staticmethod
    def _state_class(state: PlaybackState) -> str:
        """Return the CSS class used to display a playback state."""
        return {PlaybackState.PLAYING: "playing", PlaybackState.LOADING: "loading",
                PlaybackState.PAUSED: "paused", PlaybackState.ERROR: "error"}.get(state, "muted")

    @staticmethod
    def _is_busy_error(error: sqlite3.OperationalError) -> bool:
        """Return whether a SQLite error indicates lock contention."""
        return "locked" in str(error).lower() or "busy" in str(error).lower()


def run_tui(stations: Stations, settings: Settings, player: Playback) -> None:
    """Run the terminal UI and always stop playback when it exits."""
    if not terminal_is_large_enough():
        print_minimum_size_message()
        return
    try:
        CoraApp(stations, settings, player).run()
    except sqlite3.Error as error:
        raise RuntimeError(f"Database error: {error}") from error
    finally:
        player.stop()
