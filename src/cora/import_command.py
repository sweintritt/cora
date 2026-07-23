from typing import Sequence

from cora.command import Command
from cora.importer import import_stations
from cora.player import Player
from cora.settings import Settings
from cora.stations import Stations


class ImportCommand(Command):

    def __init__(self: Command, stations: Stations, settings: Settings, player: Player, subparsers):
        super().__init__(stations, settings, player)
        parser = subparsers.add_parser(
            'import', help='Import stations from radio-browser.info')
        parser.add_argument('--url',
                            type=str,
                            help="URL to the radio browser API",
                            required=False)
        parser.set_defaults(func=self.execute)

    def execute(self: Command, args: Sequence[str]):
        import_stations(self.stations, args.url)
