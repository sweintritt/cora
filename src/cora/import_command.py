from cora.command import Command
from cora.importer import import_stations


class ImportCommand(Command):

    def __init__(self, stations, settings, player, subparsers):
        super().__init__(stations, settings, player)
        parser = subparsers.add_parser(
            'import', help='Import stations from radio-browser.info')
        parser.add_argument('--url',
                            type=str,
                            help="URL to the radio browser API",
                            required=False,
                            default='https://de1.api.radio-browser.info/json/stations')
        parser.set_defaults(func=self.execute)

    def execute(self, args):
        import_stations(self.stations)
