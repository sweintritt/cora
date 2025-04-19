
import argparse
import logging

import command

__LAST_PLAYED__ = "last.played"

logger = logging.getLogger(__name__)


class PlayCommand(command.Command):

    def __init__(self, stations, settings, player, subparsers):
        super().__init__(stations, settings, player)
        parser = subparsers.add_parser('play', help='Play a station given by ID or list of keywords')
        parser.add_argument('keywords', nargs=argparse.REMAINDER, help='An ID or a list of keywords')
        parser.set_defaults(func=self.execute)

    def execute(self, args):
        logger.debug("keywords: " + str(args.keywords))

        try:
            id = int(args.keywords[0])
            station = self.stations.find_by_id(id)
        except ValueError:
            logger.debug(args.keywords[0] + " is not an id")
            if args.keywords[0] == 'last':
                id = self.settings.get(__LAST_PLAYED__)
                logger.debug("last played: " + str(id))
                station = self.stations.find_by_id(id)
            else:
                keywords = '%' + '%'.join(args.keywords) + '%'
                logger.debug("keywords: " + keywords)
                station = self.stations.find_by_keywords(keywords)

        if station is None:
            logger.info("No station found for " + str(args.keywords))
        else:
            logger.info("playing " + station.name.strip())
            try:
                self.player.set_url(station.urls[0])
                self.settings.save(__LAST_PLAYED__, station.id)
                self.player.play()
                playing = True
                while playing:
                    logger.info("Press any key to stop playing")
                    key = input()
                    playing = False
            except KeyboardInterrupt:
                # Handle interrupt nicely
                pass
            self.player.stop()
