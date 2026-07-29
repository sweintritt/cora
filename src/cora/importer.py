
import json
import logging
import time

import requests

import cora
from cora.stations import Stations

logger = logging.getLogger(__name__)

__USER_USER_AGENT__   = "cora/" + cora.__version__  + " (com.github/sweintritt/cora)"
__RADIO_BROWSER_URL__ = "https://de1.api.radio-browser.info/json/stations"

def get_stations(url: str, offset: int, limit: int):
    start = time.time()
    headers = { 'User-Agent': __USER_USER_AGENT__ }
    response = requests.get(url or __RADIO_BROWSER_URL__, params={'offset': str(offset), 'limit': str(limit)}, headers=headers)
    if not response.ok:
        raise Exception("Error while loading stations")
    data = json.loads(response.content)
    logger.debug(f"received {str(len(data))} stations in {str(time.time() - start)} s")
    return data


def insert(db: Stations, data) -> int:
    count = 0
    results = []
    for station in data:
        name = station["name"]
        genre = station["tags"]
        country = station["country"]
        language = station["language"]
        description = station["name"]
        if station["url"] != station["url_resolved"]:
            urls = [station["url"], station["url_resolved"]]
        else:
            urls = [station["url"]]
        results.append([name, 'radio-browser', genre, country, language, description, json.dumps(urls)])
        count += 1
    db.executemany('insert into stations (name, addedBy, genre, country, language, description, urls) values(?, ?, ?, ?, ?, ?, ?);', results)
    return count

def import_stations(db: Stations, url: str) -> None:
    logger.debug("loading stations")
    start = time.time()
    count = 0
    try:
        db.begin_transaction()
        db.delete_all()

        offset = 0
        step = 5000
        data = get_stations(url, offset, step)
        count += insert(db, data)

        while len(data) == step:
            offset += step
            data = get_stations(url, offset, step)
            logger.info(f"importing {str(len(data))} stations")
            count += insert(db, data)

        db.commit()
        logger.info(f"imported {str(count)} stations in {str(time.time() - start)} s")
    except Exception as e:
        logger.exception(str(e))
        db.rollback()
