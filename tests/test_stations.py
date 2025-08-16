import logging
import sys
import unittest

import cora.stations as stations


class TestStringMethods(unittest.TestCase):

    def setUp(self):
        logging.basicConfig(stream=sys.stdout, level=logging.DEBUG,
                            format="%(asctime)s %(levelname)-5s [%(name)-20s] %(message)s")
        self.stations = stations.Stations()
        self.stations.open(':memory:')

    # FIXME: Error: trancsaction not active
    def test_transaction(self):
        self.stations.begin_transaction()
        station = stations.Station(1, "Cinemix", "Soundtracks", "USA",
                                   "English", "Best of Soundtracks", "http://cinemix.us/cine.asx")
        self.stations.save(station)
        # self.stations.commit()

    def test_save(self):
        station = stations.Station(2, "Cinemix", "Soundtracks", "USA",
                                   "English", "Best of Soundtracks", "http://cinemix.us/cine.asx")
        self.stations.save(station)

    def test_serialize_urls(self):
        urls = ["http://gitarrenradio.stream.laut.fm/gitarrenradio?t302=2022-12-15_04-07-05&uuid=42e7cbbd-073f-4bad-b732-5a16e13f3538",
                "http://stream.laut.fm/gitarrenradio",
                "http://gitarrefm.stream.laut.fm/gitarrefm?t302=2022-12-15_19-19-28&uuid=7568ea93-45a9-4f8e-8e14-52539e298353",
                "http://stream.laut.fm/gitarrefm"]
        self.assertEqual(stations.serialize_urls(
            urls), "[\"http://gitarrenradio.stream.laut.fm/gitarrenradio?t302=2022-12-15_04-07-05&uuid=42e7cbbd-073f-4bad-b732-5a16e13f3538\", \"http://stream.laut.fm/gitarrenradio\", \"http://gitarrefm.stream.laut.fm/gitarrefm?t302=2022-12-15_19-19-28&uuid=7568ea93-45a9-4f8e-8e14-52539e298353\", \"http://stream.laut.fm/gitarrefm\"]")

    def test_deserialize_urls(self):
        urls = "[\"http://gitarrenradio.stream.laut.fm/gitarrenradio?t302=2022-12-15_04-07-05&uuid=42e7cbbd-073f-4bad-b732-5a16e13f3538\",\"http://stream.laut.fm/gitarrenradio\",\"http://gitarrefm.stream.laut.fm/gitarrefm?t302=2022-12-15_19-19-28&uuid=7568ea93-45a9-4f8e-8e14-52539e298353\",\"http://stream.laut.fm/gitarrefm\"]"
        self.assertEqual(stations.deserialize_urls(urls), ["http://gitarrenradio.stream.laut.fm/gitarrenradio?t302=2022-12-15_04-07-05&uuid=42e7cbbd-073f-4bad-b732-5a16e13f3538",
                                                           "http://stream.laut.fm/gitarrenradio",
                                                           "http://gitarrefm.stream.laut.fm/gitarrefm?t302=2022-12-15_19-19-28&uuid=7568ea93-45a9-4f8e-8e14-52539e298353",
                                                           "http://stream.laut.fm/gitarrefm"])
