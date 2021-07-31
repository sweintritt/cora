# Changelog

## [Unreleased]

## [v0.7.0] - 2021-07-31

### Added

* Help command.
* Showing the currently playing song.
* Play a random station.
* Info and List command now show addedBy.
* Help is showing the default value for the file option.

### Changed

* Switch to gstreamer.
* Removed 'now playing', because it looks strange in a long list of songs.
* The description in the info output is now wrapped.
* Transaction for radio sure import.

### Fixed

* The radio-sure import now removes all entries added by itself before import new data.
* Exit playing with any key did not work because play in the gstreamer implementation blocks.

## [v0.6.0] - 2021-04-10

### Added

* Info command.
* Version command.
* Help command.

### Removed

* Debug option for commands.
* find command.
* stop command.
* interactive mode.

### Fixed

* Deserialization of urls.

## [v0.5.0] - 2021-02-26

### Added

* debug flag for logging configuration.
* Added Cli parser and help option.
* Search command.
* Show usage of commands.

## [v0.3.0] - 2020-12-17

### Added

* In-Memory database.
* Demo stations.
* Simple Command-line

### Fixed

* Segmentation Fault on exit.

## [v0.2.0] - 2020-02-12

### Changed

* CMake build.
* Switched to QtMediaPlayer.

## [v0.1] - 2020-01-31

### Added

* Initial version.
