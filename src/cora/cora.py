#!/bin/python

import argparse
import logging
import os
import sys
import traceback

import cora

from .import_command import ImportCommand
from .info_command import InfoCommand
from .play_command import PlayCommand
from .version_command import VersionCommand
from .search_command import SearchCommand
from .list_command import ListCommand
from .player import Player
from .settings import Settings
from .stations import Stations

logger = logging.getLogger(__name__)


def main():
    parser = argparse.ArgumentParser(
        prog='cora',
        description='Play internet radio streams on your console v' + cora.__version__,
        formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('-d', '--debug',
                        help='Show debug output',
                        action=argparse.BooleanOptionalAction,
                        required=False)
    subparsers = parser.add_subparsers(required=True)

    player = Player()
    stations = Stations()
    settings = Settings()
    home_dir = os.path.expanduser('~')
    file = home_dir + "/" + cora.__filename__
    ImportCommand(stations, settings, player, subparsers)
    InfoCommand(stations, settings, player, subparsers)
    PlayCommand(stations, settings, player, subparsers)
    SearchCommand(stations, settings, player, subparsers)
    ListCommand(stations, settings, player, subparsers)
    VersionCommand(stations, settings, player, subparsers)

    try:
        args = parser.parse_args(sys.argv[1:])
        if args.debug:
            logging.basicConfig(stream=sys.stdout, level=logging.DEBUG,
                                format="%(asctime)s %(levelname)-5s [%(name)-20s] %(message)s")
        else:
            logging.basicConfig(stream=sys.stdout,
                                level=logging.INFO, format="%(message)s")

        logger.debug("opening %s", file)
        stations.open(file=file)
        settings.user(connection=stations.connection)
        args.func(args)
        player.stop()
        stations.close()
        settings.close()
    except Exception as e:
        logger.error(e)
        sys.exit(0)


if __name__ == "__main__":
    main()
