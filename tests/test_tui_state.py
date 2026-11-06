import asyncio
import unittest
from unittest.mock import patch

from cora.playback import PlaybackState
from cora.stations import Station
from cora.tui import CoraApp
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


class CoraAppTests(unittest.TestCase):
    def test_sections_have_headers(self) -> None:
        class EmptyStations:
            def find_all_by_keywords(self, keywords):
                return []

        class Settings:
            def save(self, key, value):
                pass

        class Player:
            def get_state(self):
                return PlaybackState.STOPPED

            def get_now_playing(self):
                return ""

            def get_error(self):
                return ""

        async def verify_headers() -> None:
            app = CoraApp(EmptyStations(), Settings(), Player())
            async with app.run_test():
                self.assertEqual("cora", app.query_one("#station-list").border_title)
                self.assertEqual("stopped", app.query_one("#playing").border_title)
                self.assertEqual("history", app.query_one("#history").border_title)

        asyncio.run(verify_headers())

    def test_keyboard_shortcuts_work_when_search_is_hidden(self) -> None:
        stations = [
            station(1, "Jazz One"),
            station(2, "Rock Two"),
        ]

        class Stations:
            def find_all_by_keywords(self, keywords):
                return stations

        class Settings:
            def save(self, key, value):
                pass

        class Player:
            def get_state(self):
                return PlaybackState.STOPPED

            def get_now_playing(self):
                return ""

            def get_error(self):
                return ""

        async def verify_shortcuts() -> None:
            app = CoraApp(Stations(), Settings(), Player())
            with patch("cora.tui.terminal_is_large_enough", return_value=True):
                async with app.run_test(size=(100, 30)) as pilot:
                    await pilot.pause()
                    self.assertIsNone(app.focused)
                    await pilot.press("j")
                    self.assertEqual(1, app.state.selected)

        asyncio.run(verify_shortcuts())
