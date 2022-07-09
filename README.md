cora
=====

cora (**Co**nsole **Ra**dio) is a very simple command line application to play internet radio streams. 

# Import a stations database

If you run any cora command for the first time, it will create an empty database.

## RadioBrowser

To import station data jus run `cora import`. This will import all stations from https://www.radio-browser.info/ and store it in the local
databse. The database will not be updated automatically. To update the database, simply run `import` again.

## RadioSurce

If you have a copy of a CSV from RadioSure (because the page is no longer available), you can run the import to add the stations into your
local database. You have to unzip the current version of the RadioSure database and import it.

```bash
$ wget http://www.radiosure.com/rsdbms/stations2.zip
$ unzip stations2.zip
$ cora import --input stations-2021-07-28.rsd --type radio-sure 
```

# Search for station

The `list` command will return all stations from the database, you can then simply use 
`grep` to filter the list

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

or use the `find` command

# Show detailed infos about a station

The `info` command gives more information about a station. Just call the command with the
id of the station.

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

To play a station run the `play` command with the id of the station

```bash
$ cora play 22420
```

If you want to select a specific url of the station, shown by the output of the show
command, add a colon and the shown position of the url in the list of the `info` command.

```bash
$ cora play 22420:1
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

To see all available commands just use the `help` command

```bash
$ cora help
cora - listen to internet radio stations
   help     Show help page
   import   Import radio stations from different sources
   info     Show information about a station, given by id
   list     List all available stations
   play     Play a station, given by id
   version  Show the version
```

and the `--help` option so see all about a specific command

```bash
$ cora import --help
NAME
  import - Import radio stations from different sources

SYNOPSIS
  import [OPTIONS]

DESCRIPTION
  -h, --help
              Show help page

  -f, --file <VALUE> 
              Database file. Default is /home/sweintritt/.cora.sqlite

  -i, --input <VALUE> 
              Input to import stations from. Depends on the type an could be
              a file or URL.

  -t, --type <VALUE> 
              Type of imported data. Supported types are: 'radio-sure', which
              requires a file as input and 'radio-browser',
              which requires a URL. 'radio-browser' is the default
              and provides a default url.
```

# Build

The project can be build with

```bash
$ mkdir build
$ cd build
$ cmake ..
$ make
```

## Install

To install cora, run `cpack` in the build folder to create a installable package for your
system and install it. For example an RPM package

```bash
$ cpack -G RPM
```

User `cpack --help` to see other available generators.

## Dependencies

* [gstreamer](https://gstreamer.freedesktop.org/) for playback,
* [sqlite](https://sqlite.org/index.html) as database,
* [plog](https://github.com/SergiusTheBest/plog) for logging and
* [cpputest](https://cpputest.github.io/) for tests.
* [cur](https://github.com/curl/curl) for download of the RadioBrowser stations.
* [json](https://github.com/nlohmann/json) processing of the RadioBrowser data.

## cppcheck

The build will add a make target ´cppcheck´, which will run *cppcheck* on all source files.
The result is stored in `<build-dir>/cppcheck` as html and xml.
This required *cppcheck* to be installed.

## Coverage

After running the unit tests, code coverage can be viewed by running ´make coverage´. The
report will be available in `<build-dir/coverage` as html and xml.
This requires *gcovr* to be installed.
