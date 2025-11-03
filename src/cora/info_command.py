import argparse
import logging

from cora.command import Command

logger = logging.getLogger(__name__)


class InfoCommand(Command):

    def __init__(self, stations, settings, player, subparsers):
        super().__init__(stations, settings, player)
        parser = subparsers.add_parser("info", help="Show detail info for a station")
        parser.add_argument("id", nargs=argparse.REMAINDER)
        parser.set_defaults(func=self.execute)

    def execute(self, args):
        logger.debug("search: %s", str(args.id))

        try:
            id = int(args.id[0])
            station = self.stations.find_by_id(id)
        except ValueError:
            logger.error("You must provide an ID.")
            return

        if station is None:
            logger.info("No station found for %s", str(id))
        else:
            logger.info("")
            logger.info("      station: %s", station.name.strip())
            logger.info("        genre: %s", station.genre.strip())
            logger.info("      country: %s", station.country.strip())
            logger.info("     language: %s", station.language.strip())
            logger.info("  description: %s", station.description.strip())
            for i, url in enumerate(station.urls):
                logger.info("       url[%d]: %s", i, url)
