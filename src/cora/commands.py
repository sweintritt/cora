
import argparse
import logging
import cora

from typing import Sequence

from cora.importer import import_stations
from cora.player import Player
from cora.settings import Settings
from cora.stations import Stations

__LAST_PLAYED__ = "last.played"

logger = logging.getLogger(__name__)

def add_commands() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog='cora',
        description='Play internet radio streams on your console v' + cora.__version__,
        formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('-d', '--debug',
                        help='Show debug output',
                        action=argparse.BooleanOptionalAction,
                        required=False)
    subparsers = parser.add_subparsers(required=True)


    import_parser = subparsers.add_parser('import', help='Import stations from radio-browser.info')
    import_parser.add_argument('--url', type=str, help="URL to the radio browser API", required=False)
    import_parser.set_defaults(func=cmd_import)

    search_parser = subparsers.add_parser('search', help='Search for stations')
    search_parser.add_argument('keywords', nargs=argparse.REMAINDER, help='List of keywords')
    search_parser.set_defaults(func=cmd_search)

    info_parser = subparsers.add_parser("info", help="Show detail info for a station")
    info_parser.add_argument("id", nargs=argparse.REMAINDER)
    info_parser.set_defaults(func=cmd_info)

    subparsers.add_parser('list', help='List all available stations').set_defaults(func=cmd_list)
    subparsers.add_parser('version', help='Show the version').set_defaults(func=cmd_version)

    play_parser = subparsers.add_parser(
        'play', help='Play a station given by ID or list of keywords')
    play_parser.add_argument('keywords',
                        nargs=argparse.REMAINDER,
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
    play_parser.set_defaults(func=cmd_play)
    return parser


def cmd_version(_args: Sequence[str], _stations: Stations, _settings: Settings, _player: Player) -> None:
    logger.info("cora - Version %s", cora.__version__)


def cmd_search(args: Sequence[str], stations: Stations, _settings: Settings, _player: Player) -> None:
    logger.debug("keywords: %s", str(args.keywords))
    keywords = '%' + '%'.join(args.keywords) + '%'
    logger.debug("keywords: %s", keywords)
    results = stations.find_all_by_keywords(keywords)

    if not results:
        logger.info("No stations found")
    else:
        for r in results:
            logger.info("id:" + str(r.id) + ", name:" + r.name + ", genre:" + r.genre + ", country:" + r.country)


def cmd_list(_args: Sequence[str], stations: Stations, _settings: Settings, _player: Player) -> None:
    ids = stations.get_all_ids()
    for id in ids:
        station = stations.find_by_id(id[0])
        logger.info("id:%s, name:%s, genre:%s, country:%s", str(station.id),
                    station.name, station.genre, station.country)


def cmd_info(args: Sequence[str], stations: Stations, _settings: Settings, _player: Player) -> None:
    logger.debug("search: %s", str(args.id))

    try:
        station = stations.find_by_id(int(args.id[0]))
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


def cmd_import(args: Sequence[str], stations: Stations, _settings: Settings, _player: Player) -> None:
    import_stations(stations, args.url)


def cmd_play(args: Sequence[str], stations: Stations, settings: Settings, player: Player) -> None:
    logger.debug("keywords: %s", str(args.keywords))
    url_index = 0

    if args.keywords[0] == 'last':
        station_id = settings.get(__LAST_PLAYED__)
        station = stations.find_by_id(station_id)
    elif args.keywords[0] == 'random':
        station = stations.get_random()
    elif args.keywords[0].isdigit():
        station_id = int(args.keywords[0])
        if len(args.keywords) > 1:
            url_index = int(args.keywords[1])
        logger.debug("with url: %d", len(args.keywords))
        station = stations.find_by_id(station_id)
    else:
        keywords = '%' + '%'.join(args.keywords) + '%'
        logger.debug("keywords: %s", keywords)
        station = stations.find_by_keywords(keywords)

    if station is None:
        logger.info("No station found for %s", args.keywords)
    else:
        logger.info("playing %s", station.name.strip())
        try:
            player.set_url(station.urls[url_index])
            settings.save(__LAST_PLAYED__, station.id)
            player.play()
            playing = True
            while playing:
                logger.info("Press any key to stop playing")
                input()
                playing = False
        except KeyboardInterrupt:
            # Handle interrupt nicely
            pass
        player.stop()
