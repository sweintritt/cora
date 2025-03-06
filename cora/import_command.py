import command
import importer


class ImportCommand(command.Command):

    def __init__(self, stations, settings, player, subparsers):
        super().__init__(stations, settings, player)
        parser = subparsers.add_parser('import')
        parser.add_argument('--url',
                            type=str,
                            required=False,
                            default='https://de1.api.radio-browser.info/json/stations')
        parser.set_defaults(func=self.execute)

    def execute(self, args):
        importer.import_stations(self.stations)
