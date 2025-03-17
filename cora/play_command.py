
import argparse
import logging

import command

logger = logging.getLogger(__name__)

class PlayCommand(command.Command):

    def __init__(self, stations, settings, player, subparsers):
        super().__init__(stations, settings, player)
        parser = subparsers.add_parser('play')
        parser.add_argument('search', nargs=argparse.REMAINDER)
        parser.set_defaults(func=self.execute)

    def execute(self, args):
        logger.debug("search: " + str(args.search))

        try:
            id = int(args.search[0])
            station = self.stations.find_by_id(id)
        except ValueError:
            logger.debug(args.search[0] + " is not an id")
            # TODO add special values like 'last'
            keywords = '%' + '%'.join(args.search) + '%'
            logger.debug("search: " + keywords)
            station = self.stations.find_by_keywords(keywords)

        logger.info("playing " + station.name.strip())
        self.player.set_url(station.urls[0])
        self.player.play()
        playing = True
        while playing:
            logger.info("Press any key to stop playing")
            key = input()
            playing = False
        self.player.stop()
