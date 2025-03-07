import command
import logging

logger = logging.getLogger(__name__)

class ListCommand(command.Command):

    def __init__(self, stations, settings, player, subparsers):
        super().__init__(stations, settings, player)
        parser = subparsers.add_parser('list')
        parser.set_defaults(func=self.execute)

    def execute(self, args):
        ids = self.stations.get_all_ids()
        for id in ids:
            station = self.stations.find_by_id(id[0])
            logger.info("id:" + str(station.id) + ", name:" + station.name + ", genre:" + station.genre + ", country:" + station.country)
