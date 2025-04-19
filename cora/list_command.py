import logging

import command

logger = logging.getLogger(__name__)


class ListCommand(command.Command):

    def __init__(self, stations, settings, player, subparsers):
        super().__init__(stations, settings, player)
        parser = subparsers.add_parser('list', help='List all available stations')
        parser.set_defaults(func=self.execute)

    def execute(self, args):
        ids = self.stations.get_all_ids()
        for id in ids:
            station = self.stations.find_by_id(id[0])
            logger.info("id:%s, name:%s, genre:%s, country:%s", str(station.id),
                        station.name, station.genre, station.country)
