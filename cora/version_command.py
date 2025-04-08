
import argparse
import logging

import command

logger = logging.getLogger(__name__)

class VersionCommand(command.Command):

    def __init__(self, stations, settings, player, subparsers):
        super().__init__(stations, settings, player)
        parser = subparsers.add_parser('version')
        parser.set_defaults(func=self.execute)

    def execute(self, args):
        # TODO: How to save the current version?
        logger.info("cora - Version 0.1.0")
