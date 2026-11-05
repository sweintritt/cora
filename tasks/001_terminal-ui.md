# 001 Terminal UI

Add an interactive terminal user interface (TUI) for browsing and playing
stations from cora's local SQLite database.

## Goal and scope

Running `cora tui` starts a keyboard-driven interface that lets users search,
select, and play internet radio stations without leaving the terminal. The
existing `search`, `list`, `info`, and `play` commands must continue to work
unchanged.

The TUI must:

- add a `tui` subcommand;
- read stations from the existing `~/.cora.sqlite` database;
- display a searchable, scrollable station list;
- show the selected station's name, genre, country, language, and playback
  status;
- use the existing `Player` implementation to start and stop playback;
- handle an empty database with instructions to run `cora import`; and
- restore the terminal cleanly after quit, Ctrl-C, fatal errors, and early
  setup failures.

## Terminal lifecycle and size

The TUI requires a terminal of at least 80 columns by 24 rows. At startup, a
smaller terminal must print the minimum required size and exit without entering
raw mode or the alternate screen.

After startup, the UI must react to resize events without requiring input or an
application restart. If the terminal becomes smaller than the minimum, render
only a minimum-size message until it is large enough again; continue to process
resize events and Ctrl-C during this state. A compact layout is out of scope.

Use textual for raw mode, the alternate screen, cursor visibility, keyboard
input, terminal-size detection, and resize events. Guard terminal setup so the
terminal is restored on every exit path.

## Main screen and navigation

The main screen contains the station list, a playback/history panel when
applicable, and a status bar with search input or keyboard hints.

```text
+- cora ---------------------------------------------------------------------+
| >  1. 1.FM 90s Alternative Radio       Rock, Alternative       Switzerland |
|    2. 181.FM - 90s Alternative         Alternative Rock        USA         |
|    3. 90s90s Rock                      Rock                    Germany     |
|    4. Allzic Radio 90s                 Pop Rock                France      |
|                                                                            |
|                                                                            |
+----------------------------------------------------------------------------+
| Press 'h' for help                                                         |
+----------------------------------------------------------------------------+
```

The name, genre, and country columns must have equal width. Truncate values
that do not fit. Keep the selected row visible while navigating a long result
set.

| Key | Action |
| --- | --- |
| `Up` / `k` | Select the previous station |
| `Down` / `j` | Select the next station |
| `/` | Enter search mode |
| `c` | Clear the search and show all stations |
| `Enter` | Play the selected station |
| `Space` | Pause or resume playback |
| `r` | Select and play a random station |
| `i` | Show details for the selected station |
| `h` | Show help |
| `q` | Quit the TUI |
| `Ctrl-C` | Quit the TUI through the normal cleanup path |

## Search and station selection

Pressing `/` enters search mode with an empty input. The user can edit the
input with character entry, Left, Right, Backspace, and Delete; paste support
is not required. Search is applied only when the user presses Enter, using the
existing keyword-search behavior. Escape cancels the input without changing the
current results.

Pressing `c` in normal mode clears the active search and displays all stations.
Applying or clearing a search resets selection to the first result. When no
stations match, show an empty list with `no results found`.

While search input is focused, `j` and `k` do not change selection, the info
dialog cannot be opened, and `q` does not quit; Ctrl-C remains available to
exit. Other printable keys are search input rather than TUI commands.

`r` selects a random station from the active filtered results, or from all
stations when no filter is active, and begins playback.

## Empty database

When no stations are available, display the following instruction instead of a
station list:

```text
+- cora ---------------------------------------------------------------------+
| No stations are available.                                                 |
|                                                                            |
| Import stations first:                                                     |
|                                                                            |
|   cora import                                                              |
|                                                                            |
| Press [q] to quit.                                                         |
+----------------------------------------------------------------------------+
```

`q` and Ctrl-C remain available in this state.

## Dialogs

### Station details

Pressing `i` in normal mode opens a dialog for the selected station. It shows
the name, database ID, genre, country, language, and description. URLs are not
shown and the TUI always plays the first URL. If the station has no URL, show
`Station has no URL` in the playback panel when playback is requested.

```text
+---------------------------------------------------+
|         name: Big R Radio - 90s Alternative Rock  |
|           id: 22420                               |
|        genre: Rock-Alternative                    |
|      country: USA                                 |
|     language: English                             |
|  description: -                                   |
+---------------------------------------------------+
```

Wrap multi-line genre and description values. Truncate other values that do
not fit within the dialog.

### Help and modality

Pressing `h` in normal mode opens a help dialog containing the keyboard
controls. Both the help and details dialogs close with `h` or Escape.

Dialogs are modal: pause/resume, search, station selection, random playback,
and `q` do not operate until the dialog is closed. Ctrl-C remains an
unconditional exit through the normal cleanup path.

## Playback display and history

The playback panel displays the active station and its current `now_playing`
value. The adjacent history panel displays the four most recent non-empty,
distinct metadata values. History entries contain only the `now_playing`
string, not the station name, and are retained when a new station is selected.

```text
+- playing --------------------------+- history -----------------------------+
| 1.FM 90s Alternative Radio         | Now playing 1                         |
| The Cure - Friday I'm In Love      | Now playing 2                         |
|                                    | Now playing 3                         |
|                                    | Now playing 4                         |
+------------------------------------+---------------------------------------+
```

Poll `now_playing` every 15 seconds. When a non-empty value changes, update
the current-track display and append it to history unless it is already
present. Empty metadata is not an error and is not added to history.

The playback panel title reflects the current state:

- `playing` while active;
- `paused` while paused;
- `loading...` while a stream is opening or buffering; and
- `error` for a recoverable playback failure.

Space pauses a playing stream and resumes a paused stream. It is a no-op while
loading, stopped, or in an error state.

## Playback and persistence

Introduce a playback abstraction so TUI state can be tested with a fake
implementation without loading libVLC. The existing `Player` implements this
abstraction, which provides operations to start, pause, resume, and stop
playback; retrieve the current state; and retrieve `now_playing` metadata.

The abstraction must distinguish:

- `Stopped`, when no stream is active;
- `Loading`, after a play request is accepted and before playback starts;
- `Playing`, when libVLC reports active playback;
- `Paused`, when playback has been paused; and
- `Error`, with a user-displayable message for a stream-specific failure.

`Player.play` initially sets `LOADING`. Map libVLC `OPENING` and `BUFFERING`
to `LOADING`, `PLAYING` to `PLAYING`, `PAUSED` to `PAUSED`, `STOPPED` and
`ENDED` to `STOPPED`, and `ERROR` to `ERROR`.

Track state changes with libVLC media-player events and update the TUI promptly.
Use `MediaPlayer.get_state()` after playback setup and as a consistency
check when processing state-related events. Metadata polling remains separate
from state updates.

Use libVLC's explicit pause control. Pause or resume requests outside their
valid states are no-ops. The player remains responsible for releasing its
libVLC resources when stopping and on drop.

Persist `last_played` for both Enter and random playback after the selected
stream reaches `Playing`. Do not require `now_playing` metadata, because a
playing stream may not provide it.

## Errors

Recoverable errors keep the TUI open, preserve the current station list and
selection, and show a concise message. Fatal errors exit through the normal
terminal-cleanup path and are displayed after the terminal is restored.

### libVLC errors

Treat failures isolated to the selected stream as recoverable playback errors.
This includes a station URL containing a NUL byte, an unsuccessful
`MediaPlayer.play()` call, and a later libVLC `Error` state. Display the
error in the playback panel, stop and release the failed media player, and
allow the user to select another station. Clear the error when another station
successfully begins loading.

Treat failures to initialize or load Python vlc binding and vlc runtime, media,
or media player, or query the required playback state as fatal. The `Player`
API must expose recoverable and fatal failures through errors rather
than requiring the TUI to classify formatted messages.

### SQLite errors

Configure every SQLite connection with a five-second busy timeout. Treat
`SQLITE_BUSY` and `SQLITE_LOCKED` after that timeout as recoverable: retain the
last successfully displayed station data, show a status-bar message, and retry
the operation on the next relevant user action. Do not show database errors in
the playback panel.

All other SQLite errors are fatal, including database opening or initialization
errors, invalid schema or station data, `SQLITE_CORRUPT`, `SQLITE_IOERR`,
`SQLITE_CANTOPEN`, `SQLITE_READONLY`, `SQLITE_FULL`, and memory or internal
SQLite failures. An empty database is not an error.

## Presentation

Use textual to render layouts, bordered blocks, selected rows, truncated
text, and dialogs.

Use an Atom One Dark-inspired theme. Color supplements, but never replaces,
text labels, borders, and the selection marker so the UI remains usable in
monochrome terminals and by users who cannot distinguish colors.

| UI element | Color | Hex value |
| --- | --- | --- |
| Main background | Dark gray | `#282C34` |
| Standard text and borders | Light gray | `#ABB2BF` |
| Selected row and focused search input | Blue | `#61AFEF` |
| Search prompt and keyboard hints | Cyan | `#56B6C2` |
| Playing state | Green | `#98C379` |
| Loading state | Yellow | `#E5C07B` |
| Paused state | Purple | `#C678DD` |
| Playback errors | Red | `#E06C75` |
| De-emphasized text, including empty history entries | Muted gray | `#5C6370` |

Use RGB colors when the terminal supports them. Otherwise, use the nearest ANSI
color or the terminal's default foreground and background rather than failing.

Keep TUI state separate from rendering and libVLC. The state layer manages
search state, selection, scrolling, dialogs, playback display state, and
metadata history, and must be unit-testable without a terminal or libVLC.

## Acceptance criteria

- Do not make changes to the existing code, if not necessary. Ask first.
- `cora tui` opens the interface on a terminal of the minimum supported size.
- An undersized terminal prints the required-size message and exits before
  entering terminal UI mode.
- Search filters stations using the existing keyword-search behavior.
- The selected row remains visible while navigating long result sets.
- Enter plays the selected station, and Space pauses or resumes it when valid.
- The playing station and current track title, when available, are visible.
- The terminal is restored after quit, Ctrl-C, or an error.
- Existing CLI commands and their tests continue to pass.
- Unit tests cover TUI state transitions and search/filter behavior.
- `README.md` describes the terminal UI.