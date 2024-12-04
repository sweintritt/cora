#!/bin/python

import player
import stations

player = player.Player()
player.set_url('http://94.23.51.96:8001') # Cinemix
player.play()
playing = True

db = stations.Stations()
db.open(file="test.sqlite")
db.close()

while playing:
    print("Press any key to stop playing\n")
    key = input()
    playing = False

player.stop()
