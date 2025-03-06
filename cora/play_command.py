import command


class PlayCommand(command.Command):

    def __init__(self, stations, settings, player, subparsers):
        super().__init__(stations, settings, player)
        parser = subparsers.add_parser('play')
        parser.add_argument('--id', type=int)
        parser.set_defaults(func=self.execute)

    def execute(self, args):
        station = self.stations.find_by_id(args.id)
        self.player.set_url(station.urls[0])
        self.player.play()
        playing = True
        while playing:
            print("Press any key to stop playing\n")
            key = input()
            playing = False
        self.player.stop()
