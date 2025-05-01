#!/bin/python

import argparse
import logging
import os
import sys
import traceback

from cora.import_command import import_command
from cora.info_command import info_command
from cora.list_command import list_command
from cora.play_command import play_command
from cora.player import player
from cora.search_command import search_command
from cora.settings import settings
from cora.stations import stations
from cora.version_command import version_command

logger = logging.getLogger(__name__)


def main():
    parser = argparse.ArgumentParser(
                        prog='cora',
                        description='Play internet radio streams on your console')
    parser.add_argument('-d', '--debug',
                        help='Show debug output',
                        action=argparse.BooleanOptionalAction,
                        required=False)
    subparsers = parser.add_subparsers(required=True)

    player = player.Player()
    stations = stations.Stations()
    settings = settings.Settings()
    home_dir = os.path.expanduser('~')
    file = home_dir + '/.cora2.sqlite'
    # TODO: Save Version
    # TODO: Only one filehandle would be better
    stations.open(file=file)
    settings.open(file=file)

    import_command = import_command.ImportCommand(stations, settings, player, subparsers)
    info_command = info_command.InfoCommand(stations, settings, player, subparsers)
    play_command = play_command.PlayCommand(stations, settings, player, subparsers)
    search_command = search_command.SearchCommand(stations, settings, player, subparsers)
    list_command = list_command.ListCommand(stations, settings, player, subparsers)
    version_command = version_command.VersionCommand(stations, settings, player, subparsers)

    try:
        args = parser.parse_args(sys.argv[1:])
        if args.debug:
            logging.basicConfig(stream=sys.stdout, level=logging.DEBUG, format="%(asctime)s %(levelname)-5s [%(name)-20s] %(message)s")
        else:
            logging.basicConfig(stream=sys.stdout, level=logging.INFO, format="%(message)s")

        args.func(args)
        player.stop()
        stations.close()
        settings.close()
    except Exception as e:
        logger.error(e)
        traceback.print_exc()
        sys.exit(0)
