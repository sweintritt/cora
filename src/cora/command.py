
from typing import Sequence

from cora.player import Player
from cora.settings import Settings
from cora.stations import Stations


class Command:
    def __init__(self: Command, stations: Stations, settings: Settings, player: Player):
        self.stations = stations
        self.settings = settings
        self.player = player

    def execute(self: Command, args: Sequence[str]):
        # Empty implementation
        pass
