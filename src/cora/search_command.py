
import argparse
import logging

from typing import Sequence

from cora.command import Command
from cora.player import Player
from cora.settings import Settings
from cora.stations import Stations

logger = logging.getLogger(__name__)


class SearchCommand(Command):

    def __init__(self: Command, stations: Stations, settings: Settings, player: Player, subparsers):
        super().__init__(stations, settings, player)
        parser = subparsers.add_parser('search', help='Search for stations')
        parser.add_argument('keywords', nargs=argparse.REMAINDER, help='List of keywords')
        parser.set_defaults(func=self.execute)

    def execute(self: Command, args: Sequence[str]):
        logger.debug("keywords: %s", str(args.keywords))
        keywords = '%' + '%'.join(args.keywords) + '%'
        logger.debug("keywords: %s", keywords)
        results = self.stations.find_all_by_keywords(keywords)

        if not results:
            logger.info("No stations found")
        else:
            for r in results:
                logger.info("id:" + str(r.id) + ", name:" + r.name + ", genre:" + r.genre + ", country:" + r.country)
