
import logging

from cora.command import Command

logger = logging.getLogger(__name__)


class VersionCommand(Command):

    def __init__(self, stations, settings, player, subparsers):
        super().__init__(stations, settings, player)
        parser = subparsers.add_parser('version', help='Show the version')
        parser.set_defaults(func=self.execute)

    def execute(self, args):
        # TODO: How to save the current version?
        logger.info("cora - Version 0.1.0")
