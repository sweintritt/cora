
import logging

from typing import Sequence

from cora.command import Command
from cora.player import Player
from cora.settings import Settings
from cora.stations import Stations

logger = logging.getLogger(__name__)


class VersionCommand(Command):

    def __init__(self: Command, stations: Stations, settings: Settings, player: Player, subparsers):
        super().__init__(stations, settings, player)
        parser = subparsers.add_parser('version', help='Show the version')
        parser.set_defaults(func=self.execute)

    def execute(self: Command, args: Sequence[str]):
        logger.info("cora - Version %s", cora.__version__)
