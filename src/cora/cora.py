#!/bin/python

import logging
import os
import sys

import cora

from .commands import add_commands

from .settings import Settings
from .stations import Stations
from .tui import print_minimum_size_message, terminal_is_large_enough

logger = logging.getLogger(__name__)


def main() -> None:
    try:
        parser = add_commands()
        args = parser.parse_args(sys.argv[1:])
        if args.debug:
            logging.basicConfig(stream=sys.stdout, level=logging.DEBUG,
                                format="%(asctime)s %(levelname)-5s [%(name)-20s] %(message)s")
        else:
            logging.basicConfig(stream=sys.stdout,
                                level=logging.INFO, format="%(message)s")

        stations = Stations()
        settings = Settings()
        if args.func.__name__ == "cmd_tui" and not terminal_is_large_enough():
            print_minimum_size_message()
            return

        from .player import Player
        player = Player()
        home_dir = os.path.expanduser('~')
        file = home_dir + "/" + cora.__filename__

        logger.debug("opening %s", file)
        stations.open(file=file)
        settings.user(connection=stations.connection)
        args.func(args, stations, settings, player)
        player.stop()
        stations.close()
        settings.close()
    except Exception as e:
        logger.exception(e)
        sys.exit(0)


if __name__ == "__main__":
    main()
