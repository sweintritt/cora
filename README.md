# cora

cora (**Co**nsole **Ra**dio) is a very simple command line application to play
internet radio streams.

# Import a stations database

If you run any cora command for the first time, it will create an empty database.

## RadioBrowser

To import station data jus run `cora import`. This will import all stations from
https://www.radio-browser.info/ and store it in the local databse. The database will
not be updated automatically. To update the database, simply run `import` again.

# Search for station

The `search` command will search for stations based on a list of given keywords.

```bash
$ cora search 90s alternative
id:976, name:1.FM 90s Alternative Radio, genre:90s,alternative,alternative rock,rock, country:Switzerland
id:1145, name:100Hitz - 90s Alternative Hitz, genre:90s alternative, country:The United States Of America
id:1618, name:113.FM Alt-X, genre:90s,alternative / indie,alternative rock, country:The United States Of America
id:1690, name:181.FM - 90s Alternative, genre:90s,alternative,waynesboro, country:The United States Of America
id:2734, name:90s Pinoy Alternative, genre:, country:The Philippines
id:2735, name:90s Pinoy Alternative, genre:alternative,opm, country:The Philippines
id:2765, name:90s90s Rock (HLS), genre:90s,alternative rock,hard rock,rock, country:Germany
id:3030, name:95.5 Buzz HD2, genre:2000s,90s,alternative,rock, country:The United States Of America
id:4521, name:Allzic Radio 90s, genre:90s,alternative,classic hits,pop,pop rock, country:France
```

An alternative is to use the `list` command. `list` will return all stations from the
database, you can then simply use `grep` to filter the list

```bash
$ cora list | grep -i 90s | grep -i alternative
id:976, name:1.FM 90s Alternative Radio, genre:90s,alternative,alternative rock,rock, country:Switzerland
id:1145, name:100Hitz - 90s Alternative Hitz, genre:90s alternative, country:The United States Of America
id:1618, name:113.FM Alt-X, genre:90s,alternative / indie,alternative rock, country:The United States Of America
id:1690, name:181.FM - 90s Alternative, genre:90s,alternative,waynesboro, country:The United States Of America
id:2734, name:90s Pinoy Alternative, genre:, country:The Philippines
id:2735, name:90s Pinoy Alternative, genre:alternative,opm, country:The Philippines
id:2765, name:90s90s Rock (HLS), genre:90s,alternative rock,hard rock,rock, country:Germany
id:3030, name:95.5 Buzz HD2, genre:2000s,90s,alternative,rock, country:The United States Of America
id:4521, name:Allzic Radio 90s, genre:90s,alternative,classic hits,pop,pop rock, country:France
```

# Show detailed infos about a station

The `info` command gives more information about a station. Just call the command with the
id of the station.

```bash
$ cora info 22420
      station: Big R Radio - 90s Alternative Rock
        genre: 90s,alternative rock
      country: The United States Of America
     language: english
  description: Big R Radio - 90s Alternative Rock
       url[0]: http://bigrradio.cdnstream1.com/5187_128
```

# Start playing

Use the play command and a list of keywords like this

```bash
$ cora play 90s Alternative Rock
playing Big R Radio - 90s Alternative Rock
```
cora will search for stations matching the keywords and select one
from the found results. The selection is randomly. So if you dont
like it. Just run the command again to pick another station matching
the keywords or edit your search.

## Select a specific station

If using keywords will not select the station you want, you can
select a specific station by id.

```bash
$ cora play 25719
playing Soundtrack Radio
Press enter to stop playing
Soldaat van Oranje (Rogier Van Otterloo) - Soldaat van Oranje
Edward Scissorhands (Danny Elfman) - Death!
Band of Brothers (Michael Kamen) - The Mission Begins
August Rush (Mark Mancina) - Augusts Rhapsody
```

While playing cora will list the played songs. To stop, just press _enter_ or _ctrl-c_

## Select a specific url

If you want to select a specific url of the station, shown by the output of the `show`
command, add the shown index of the url in the list of the `info` command.

```bash
$ cora info 22420
      station: Big R Radio - 90s Alternative Rock
        genre: Rock-Alternative
      country: USA
     language: English
  description: -
       url[0]: http://bigrradio.cdnstream1.com/5187_128
       url[1]: http://107.155.111.170:8030
       url[2]: http://bigrradio.cdnstream1.com/5187_48

$ cora play 22420 1
```

If the given position doest not exist the first url (position 0) is used.

## Play random station

If you just want to play a random station just call

```bash
$ cora play random
```

## Play last station

If you want to listen to the station you played the last time call

```bash
$ cora play last
```

## Help

To see all available commands, use the `--help` option without a command:

```bash
$ cora --help
Play internet radio streams on your console

Usage: cora [OPTIONS] <COMMAND>

Commands:
  import   Import stations from radio-browser.info
  search   Search for stations
  info     Show detailed information for a station
  list     List all available stations
  version  Show the version
  play     Play a station by ID or keywords
  help     Print this message or the help of the given subcommand(s)

Options:
  -d, --debug  
  -h, --help   Print help
```

Use `--help` with a command to see its options:

```bash
$ cora play --help
Play a station by ID or keywords

Usage: cora play [KEYWORDS]...

Arguments:
  [KEYWORDS]...

Options:
  -h, --help  Print help
```

## Build and install

### Prerequisites

Install the [Rust toolchain](https://www.rust-lang.org/tools/install), which
includes Cargo. libVLC is required at runtime for the `play` command.

Build the release binary:

```bash
$ cargo build --release
```

Install `cora` into Cargo's binary directory (usually `~/.cargo/bin`):

```bash
$ cargo install --path .
```

The resulting `target/release/cora` binary stores its database in
`~/.cora.sqlite`. VLC's
libVLC shared library is required for the `play` command; the CLI does not
launch `vlc` or `cvlc`.

To build an RPM, install the Cargo RPM generator once and run:

```bash
$ cargo install cargo-generate-rpm
$ cargo generate-rpm
```

The package is written to `target/generate-rpm/`.

Run the binary directly from the Cargo build tree:

```bash
$ cargo run -- --help
```

# Dependencies

- [vlc](https://code.videolan.org/videolan/vlc) for playback

## Run tests

```bash
$ cargo test
```

## Check code coverage

Install Tarpaulin once, then run it from the repository root:

```bash
$ cargo install cargo-tarpaulin --locked
$ cargo tarpaulin --workspace --exclude-files src/player.rs --fail-under 90 --out Stdout
```

Clean generated build artifacts with:

```bash
$ cargo clean
```
