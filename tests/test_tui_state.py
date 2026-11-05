import unittest

from cora.playback import PlaybackState
from cora.stations import Station
from cora.tui_state import TuiState


def station(identifier: int, name: str) -> Station:
    return Station(identifier, name, "genre", "country", "language", "", ["https://example.test"])


class TuiStateTests(unittest.TestCase):
    def setUp(self) -> None:
        self.stations = [station(1, "Jazz One"), station(2, "Rock Two"), station(3, "Jazz Three")]
        self.state = TuiState(
            self.stations,
            lambda keywords: [item for item in self.stations if all(word.lower() in item.name.lower() for word in keywords)],
        )

    def test_search_filters_and_resets_selection(self) -> None:
        self.state.selected = 2
        self.state.scroll = 2
        self.state.search_text = "jazz"

        self.state.apply_search()

        self.assertEqual(["Jazz One", "Jazz Three"], [item.name for item in self.state.stations])
        self.assertEqual(0, self.state.selected)
        self.assertEqual(0, self.state.scroll)
        self.assertFalse(self.state.search_mode)

    def test_navigation_keeps_selected_station_visible(self) -> None:
        self.state.move_selection(2, visible_rows=2)

        self.assertEqual(2, self.state.selected)
        self.assertEqual(1, self.state.scroll)

    def test_search_mode_and_dialog_block_navigation(self) -> None:
        self.state.begin_search()
        self.state.move_selection(1, visible_rows=2)
        self.state.cancel_search()
        self.state.dialog = "help"
        self.state.move_selection(1, visible_rows=2)

        self.assertEqual(0, self.state.selected)

    def test_metadata_history_is_distinct_and_limited(self) -> None:
        for title in ("one", "two", "one", "three", "four", "five"):
            self.state.update_playback(PlaybackState.PLAYING, "", title)

        self.assertEqual(["five", "four", "three", "two"], self.state.history)
