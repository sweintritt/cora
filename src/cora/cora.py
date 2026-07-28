#!/bin/python

import logging
import os
import sys

import cora

from .commands import add_commands

from .player import Player
from .settings import Settings
from .stations import Stations

logger = logging.getLogger(__name__)


def main():
    try:
        parser = add_commands()
        args = parser.parse_args(sys.argv[1:])
        if args.debug:
            logging.basicConfig(stream=sys.stdout, level=logging.DEBUG,
                                format="%(asctime)s %(levelname)-5s [%(name)-20s] %(message)s")
        else:
            logging.basicConfig(stream=sys.stdout,
                                level=logging.INFO, format="%(message)s")

        player = Player()
        stations = Stations()
        settings = Settings()
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
