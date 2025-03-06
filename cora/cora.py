#!/bin/python

import argparse
import sys

import import_command
import list_command
import play_command
import player
import stations

parser = argparse.ArgumentParser(
                    prog='cora',
                    description='Play internet radio streams on your console')
subparsers = parser.add_subparsers(required=True)

player = player.Player()
db = stations.Stations()
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
    print(e)
    sys.exit(0)
