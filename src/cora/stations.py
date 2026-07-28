import json
import logging

from typing import Sequence

from cora.db import Db

__CREATE_TABLE_STATIONS_SQL__ = "CREATE TABLE IF NOT EXISTS stations (name TEXT NOT NULL, addedBy TEXT NOT NULL, genre TEXT NOT NULL, country TEXT NOT NULL, language TEXT NOT NULL, description TEXT, urls TEXT NOT NULL)"
__FIND_STATION_BY_ID_SQL__ = "SELECT rowid, * FROM stations WHERE rowid = ?;"
__FIND_STATION_SQL__ = "SELECT rowid, * FROM stations WHERE name LIKE ? AND genre LIKE ? AND country LIKE ? ;"
__FIND_STATIONS_BY_KEYWORDS_SQL__ = "SELECT rowid FROM (SELECT rowid, name || ' ' || description || ' ' || genre || ' ' || country || ' ' || language as searchstring FROM stations) WHERE searchstring LIKE ? ORDER BY rowid;"
__FIND_STATION_BY_KEYWORDS_SQL__ = "SELECT rowid FROM (SELECT rowid, name || ' ' || description || ' ' || genre || ' ' || country || ' ' || language as searchstring FROM stations) WHERE searchstring LIKE ? ORDER BY random() LIMIT 1;"
__DELETE_ALL_SQL__ = "DELETE FROM stations;"
__INSERT_STATION_SQL__ = "INSERT INTO stations (addedBy, name, genre, country, language, description, urls) VALUES (?, ?, ?, ?, ?, ?, ?);"
__GET_ALL_IDS_SQL__ = "SELECT rowid FROM stations;"
__SELECT_RANDOM_STATION_SQL__ = (
    "SELECT rowid, * FROM stations ORDER BY random() limit 1;"
)

logger = logging.getLogger(__name__)


class Station:

    def __init__(self, id, name, genre, country, language, description, urls):
        self.id = id
        self.name = name
        self.genre = genre
        self.country = country
        self.language = language
        self.description = description
        self.urls = urls


def serialize_urls(urls):
    return json.dumps(urls)


def deserialize_urls(value):
    return json.loads(value)


class Stations(Db):

    def __init__(self: Stations) -> None:
        super().__init__()

    def open(self: Stations, file) -> None:
        super().open(file)
        self.execute(__CREATE_TABLE_STATIONS_SQL__)
        self.commit()

    def find_by_id(self: Stations, stationid: int) -> Station | None:
        result = self.cursor.execute(__FIND_STATION_BY_ID_SQL__, (stationid,))
        values = result.fetchone()
        logger.debug("values: %s", str(values))
        if values is not None:
            stationid, name, added_by, genre, country, language, description, urls = values
            return Station(
                stationid, name, genre, country, language, description, deserialize_urls(urls)
            )

    def find_all_by_keywords(self: Stations, keywords: Sequence[str]) -> Sequence[Station]:
        result = self.cursor.execute(__FIND_STATIONS_BY_KEYWORDS_SQL__, (keywords,))
        ids = result.fetchall()
        logger.debug("ids:%s", str(ids))
        results = []

        for id in ids:
            results.append(self.find_by_id(id[0]))

        return results

    def find_by_keywords(self: Stations, keywords: Sequence[str]) -> Station | None:
        result = self.cursor.execute(__FIND_STATION_BY_KEYWORDS_SQL__, (keywords,))
        stationid = result.fetchone()
        logger.debug("id:%s", str(stationid))
        if stationid is not None:
            return self.find_by_id(stationid[0])
        else:
            return None

    def delete_all(self: Stations) -> None:
        self.cursor.execute(__DELETE_ALL_SQL__)

    def get_random(self: Stations) -> Station | None:
        result = self.execute(__SELECT_RANDOM_STATION_SQL__)
        stationid, name, added_by, genre, country, language, description, urls = (
            result.fetchone()
        )
        # TODO What if nothing is found
        return Station(stationid, name, genre, country, language, description, urls)

    def get_all_ids(self: Stations) -> Sequence[int]:
        result = self.execute(__GET_ALL_IDS_SQL__)
        return result.fetchall()
