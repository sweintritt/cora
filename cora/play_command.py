import command

class PlayCommand(command.Command):

    def __init__(self, stations, settings, player, subparsers):
        super().__init__(stations, settings, player)
        parser = subparsers.add_parser('play')
        parser.add_argument('--id', type=int)
        parser.set_defaults(func=self.execute)

    def execute(self, args):
        print("execute play")
        print(args)
        # player.set_url('http://94.23.51.96:8001') # Cinemix
        # player.play()
        # playing = True
        #while playing:
        #    print("Press any key to stop playing\n")
        #    key = input()
        #    playing = False
        #player.stop()