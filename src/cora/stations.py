import json
import logging

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

    def __init__(self):
        super().__init__()

    def open(self, file):
        super().open(file)
        self.execute(__CREATE_TABLE_STATIONS_SQL__)
        self.connection.commit()

    def find_by_id(self, id):
        result = self.cursor.execute(__FIND_STATION_BY_ID_SQL__, (id,))
        values = result.fetchone()
        logger.debug("values: %s", str(values))
        if values is not None:
            id, name, added_by, genre, country, language, description, urls = values
            return Station(
                id, name, genre, country, language, description, deserialize_urls(urls)
            )

    def find_all_by_keywords(self, keywords):
        result = self.cursor.execute(__FIND_STATIONS_BY_KEYWORDS_SQL__, (keywords,))
        ids = result.fetchall()
        logger.debug("ids:%s", str(ids))
        results = []

        for id in ids:
            results.append(self.find_by_id(id[0]))

        return results

    def find_by_keywords(self, keywords):
        result = self.cursor.execute(__FIND_STATION_BY_KEYWORDS_SQL__, (keywords,))
        id = result.fetchone()
        logger.debug("id:%s", str(id))
        if id is not None:
            return self.find_by_id(id[0])
        else:
            return None

    def save(self, station):
        self.cursor.execute(
            __INSERT_STATION_SQL__,
            (
                "radio-browser",
                station.name,
                station.genre,
                station.country,
                station.language,
                station.description,
                serialize_urls(station.urls),
            ),
        )
        self.connection.commit()

    def delete_all(self):
        self.cursor.execute(__DELETE_ALL_SQL__)
        self.connection.commit()

    def get_random(self):
        result = self.cursor.execute(__SELECT_RANDOM_STATION_SQL__)
        id, name, added_by, genre, country, language, description, urls = (
            result.fetchone()
        )
        return Station(id, name, genre, country, language, description, urls)

    def get_all_ids(self):
        result = self.cursor.execute(__GET_ALL_IDS_SQL__)
        return result.fetchall()
