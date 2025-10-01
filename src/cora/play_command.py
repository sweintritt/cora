
import argparse
import logging

from cora.command import Command

__LAST_PLAYED__ = "last.played"

logger = logging.getLogger(__name__)


class PlayCommand(Command):

    def __init__(self, stations, settings, player, subparsers):
        super().__init__(stations, settings, player)
        parser = subparsers.add_parser(
            'play', help='Play a station given by ID or list of keywords')
        parser.add_argument('keywords',
                            nargs=argparse.REMAINDER,
                            # TODO:Newlines are not printed
                            help="""
An ID or a list of keywords.

Special keywords are

- random - plays a random station
- last   - plays the last played station

If a station has multiple URLs, you can specify the URL index
by appending the index to the keyword, e.g. 'play 123 1' will play
the second URL of station 123. If no index is given, the first URL
will be used.
This only works if an ID is given and the station has multiple URLs.

Otherwise the list of keywords is used to search for stations. And
if multiple stations match, the first one will be played.
""")
        parser.set_defaults(func=self.execute)

    def execute(self, args):
        logger.debug("keywords: %s", str(args.keywords))
        url_index = 0

        if args.keywords[0] == 'last':
            station_id = self.settings.get(__LAST_PLAYED__)
            station = self.stations.find_by_id(station_id)
        elif args.keywords[0] == 'random':
            station = self.stations.get_random()
        elif args.keywords[0].isdigit():
            station_id = int(args.keywords[0])
            if len(args.keywords) > 1:
                url_index = int(args.keywords[1])
            logger.debug("with url: %d", len(args.keywords))
            station = self.stations.find_by_id(station_id)
        else:
            keywords = '%' + '%'.join(args.keywords) + '%'
            logger.debug("keywords: %s", keywords)
            station = self.stations.find_by_keywords(keywords)

        if station is None:
            logger.info("No station found for %s", args.keywords)
        else:
            logger.info("playing %s", station.name.strip())
            try:
                self.player.set_url(station.urls[url_index])
                self.settings.save(__LAST_PLAYED__, station.id)
                self.player.play()
                playing = True
                while playing:
                    logger.info("Press any key to stop playing")
                    input()
                    playing = False
            except KeyboardInterrupt:
                # Handle interrupt nicely
                pass
            self.player.stop()
