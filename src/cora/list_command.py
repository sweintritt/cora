import logging

from typing import Sequence

from cora.command import Command
from cora.player import Player
from cora.settings import Settings
from cora.stations import Stations

logger = logging.getLogger(__name__)


class ListCommand(Command):

    def __init__(self: Command, stations: Stations, settings: Settings, player: Player, subparsers):
        super().__init__(stations, settings, player)
        parser = subparsers.add_parser('list', help='List all available stations')
        parser.set_defaults(func=self.execute)

    def execute(self: Command, args: Sequence[str]):
        ids = self.stations.get_all_ids()
        for id in ids:
            station = self.stations.find_by_id(id[0])
            logger.info("id:%s, name:%s, genre:%s, country:%s", str(station.id),
                        station.name, station.genre, station.country)
