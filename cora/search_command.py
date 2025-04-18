
import argparse
import logging

import command
import stations

logger = logging.getLogger(__name__)


class SearchCommand(command.Command):

    def __init__(self, stations, settings, player, subparsers):
        super().__init__(stations, settings, player)
        parser = subparsers.add_parser('search')
        parser.add_argument('keywords', nargs=argparse.REMAINDER)
        parser.set_defaults(func=self.execute)

    def execute(self, args):
        logger.debug("keywords: " + str(args.keywords))
        keywords = '%' + '%'.join(args.keywords) + '%'
        logger.debug("keywords: " + keywords)
        results = self.stations.find_all_by_keywords(keywords)

        if not results:
            logger.info("No stations found")
        else:
            for r in results:
                logger.info("id:" + str(r.id) + ", name:" + r.name + ", genre:" + r.genre + ", country:" + r.country)
