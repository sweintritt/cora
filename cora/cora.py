#!/bin/python

import argparse
import logging
import sys
import traceback

import import_command
import list_command
import play_command
import player
import stations

logger = logging.getLogger(__name__)

if __name__ == '__main__':
    logging.basicConfig(stream=sys.stdout, level=logging.DEBUG, format="%(levelname)s - %(message)s")

    logger.debug("initializing commands")
    parser = argparse.ArgumentParser(
                        prog='cora',
                        description='Play internet radio streams on your console')
    subparsers = parser.add_subparsers(required=True)

    player = player.Player()
    db = stations.Stations()
    logger.debug("opening database")
    db.open(file="test.sqlite")

    import_command = import_command.ImportCommand(db, None, player, subparsers)
    play_command = play_command.PlayCommand(db, None, player, subparsers)
    list_command = list_command.ListCommand(db, None, player, subparsers)

    try:
        args = parser.parse_args(sys.argv[1:])
        args.func(args)
        player.stop()
        db.close()
    except Exception as e:
        logger.error(e)
        traceback.print_exc()
        sys.exit(0)