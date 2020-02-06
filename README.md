cora
=====

cora (*Co*nsole *Ra*dio) is a commandline application to play internet radio streams.

# Play

Play a URL with

    cora http://example.org/streams

Without a URL cora will use a default radio station.

# Dependencies

cora uses [Qt Multimedia Module](https://doc.qt.io/qt-5/multimediaoverview.html).

# Build

The project can be build with

    mkdir build
    cd build
    cmake ..
    make
