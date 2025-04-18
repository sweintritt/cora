#!/bin/python

import argparse
import logging
import os
import sys
import traceback

import import_command
import info_command
import list_command
import play_command
import player
import search_command
import settings
import stations
import version_command

logger = logging.getLogger(__name__)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(
                        prog='cora',
                        description='Play internet radio streams on your console')
    parser.add_argument('-d', '--debug',
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
