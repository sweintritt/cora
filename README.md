cora
=====

cora (**Co**nsole **Ra**dio) is a commandline application to play internet radio streams. cora can import
stations from a [RadioSure](http://www.radiosure.com/stations/).

# Import a stations database

If you run any cora command for the first time, it will create an empty database. To add some stations
you have to download and unzip the current version of the RadioSure database and import it.

```bash
$ wget http://www.radiosure.com/rsdbms/stations2.zip
$ unzip stations2.zip
$ cora import --input stations-2021-07-28.rsd --type radio-sure 
```

# Search for station

The `list` command will return all stations from the database, you can then simply use `grep` to filter
the list

```bash
$ cora list | grep -i 90s | grep -i alternative
id:1323", name:"a-0 Alternative Nation (90s)", addedBy:"radio-sure", genre:"Rock-Alternative", country:"USA", language:"English"
id:9470", name:"AceRadio.Net - 90s Alternative Rock", addedBy:"radio-sure", genre:"Rock-Alternative", country:"USA", language:"English"
id:9830", name:"Bestnet Radio - 90s Alternative", addedBy:"radio-sure", genre:"90s", country:"USA", language:"English"
id:10930", name:"iHeart Smells Like the 90s", addedBy:"radio-sure", genre:"Rock-Alternative", country:"USA", language:"English"
id:17085", name:"90s90s Grunge", addedBy:"radio-sure", genre:"Rock-Alternative", country:"Germany", language:"German"
id:20661", name:"GotRadio - Alternative Attic (90s Alternative)", addedBy:"radio-sure", genre:"90s", country:"USA", language:"English"
id:22420", name:"Big R Radio - 90s Alternative Rock", addedBy:"radio-sure", genre:"Rock-Alternative", country:"USA", language:"English"
id:25400", name:"100Hitz - 90s Alternative Hitz", addedBy:"radio-sure", genre:"90s", country:"USA", language:"English"
```

**WRITE**

# Show detailed infos about a station

The `info` command gives more information about a station

```bash
$ cora info 22420
      station: Big R Radio - 90s Alternative Rock
        genre: Rock-Alternative
      country: USA
     language: English
  description: -
     added by: radio-sure
       url[0]: http://bigrradio.cdnstream1.com/5187_128
       url[1]: http://107.155.111.170:8030
       url[2]: http://bigrradio.cdnstream1.com/5187_48  
```

# Start playing

To play a station run the play command with the id of the station

```bash
$ cora play 22420
```

If you want to select a url of the station, shown by the output of the show command, add a colon
and the shown position of the url in the list

```bash
$ cora play 22420:1
```

If the given position doest not exist the first url is used.

## Play random station

If you just want to play a random station

```bash
$ cora play random
```

# Build

The project can be build with

    mkdir build
    cd build
    cmake ..
    make

## Dependencies

* [gstreamer](https://gstreamer.freedesktop.org/) for playback,
* [sqlite](https://sqlite.org/index.html) as database,
* [plog](https://github.com/SergiusTheBest/plog) for logging and
* [cpputest](https://cpputest.github.io/) for tests.
