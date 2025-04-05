#!/bin/python

import argparse
import logging
import os
import sys
import traceback

import import_command
import list_command
import play_command
import player
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
    db = stations.Stations()
    home_dir = os.path.expanduser('~')
    file = home_dir + '/.cora2.sqlite'
    print(file)
    db.open(file=file)

    import_command = import_command.ImportCommand(db, None, player, subparsers)
    play_command = play_command.PlayCommand(db, None, player, subparsers)
    list_command = list_command.ListCommand(db, None, player, subparsers)
    version_command = version_command.VersionCommand(db, None, player, subparsers)

    try:
        args = parser.parse_args(sys.argv[1:])

        if args.debug:
            logging.basicConfig(stream=sys.stdout, level=logging.DEBUG, format="%(asctime)s %(levelname)-5s [%(name)-20s] %(message)s")
        else:
            logging.basicConfig(stream=sys.stdout, level=logging.INFO, format="%(message)s")

        args.func(args)
        player.stop()
        db.close()
    except Exception as e:
        logger.error(e)
        traceback.print_exc()
        sys.exit(0)
