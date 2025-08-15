#!/bin/python

import argparse
import logging
import os
import sys
import traceback

import cora

from cora.import_command import ImportCommand
from cora.info_command import InfoCommand
from cora.list_command import ListCommand
from cora.play_command import PlayCommand
from cora.player import Player
from cora.search_command import SearchCommand
from cora.settings import Settings
from cora.stations import Stations
from cora.version_command import VersionCommand

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

    _player = Player()
    _stations = Stations()
    _settings = Settings()
    home_dir = os.path.expanduser('~')
    # TODO: Default Value in init script
    file = home_dir + '/.cora2.sqlite'
    # TODO: Save Version
    # TODO: Only one filehandle would be better
    _stations.open(file=file)
    _settings.open(file=file)

    ImportCommand(_stations, _settings, _player, subparsers)
    InfoCommand(_stations, _settings, _player, subparsers)
    PlayCommand(_stations, _settings, _player, subparsers)
    SearchCommand(_stations, _settings, _player, subparsers)
    ListCommand(_stations, _settings, _player, subparsers)
    VersionCommand(_stations, _settings, _player, subparsers)

    try:
        args = parser.parse_args(sys.argv[1:])
        if args.debug:
            logging.basicConfig(stream=sys.stdout, level=logging.DEBUG,
                                format="%(asctime)s %(levelname)-5s [%(name)-20s] %(message)s")
        else:
            logging.basicConfig(stream=sys.stdout,
                                level=logging.INFO, format="%(message)s")

        args.func(args)
        _player.stop()
        _stations.close()
        _settings.close()
    except Exception as e:
        logger.error(e)
        traceback.print_exc()
        sys.exit(0)


if __name__ == "__main__":
    main()
