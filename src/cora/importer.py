
import json
import logging
import time

import requests

import cora
from cora.stations import Stations

logger = logging.getLogger(__name__)

__USER_USER_AGENT__   = "cora/" + cora.__version__  + " (com.github/sweintritt/cora)"
__RADIO_BROWSER_URL__ = "https://de1.api.radio-browser.info/json/stations"

def get_stations(url: str):
    headers = { 'User-Agent': __USER_USER_AGENT__ }
    # 60485 stations on 21-07-2026
    response = requests.get(url or __RADIO_BROWSER_URL__, params={'limit': '100000'}, headers=headers)
    # TODO Use offset and limit to iterate the list in smaller steps
    if not response.ok:
        raise Exception("Error while loading stations")
    return json.loads(response.content)

def import_stations(db: Stations, url: str):
    logger.debug("loading stations")
    start = time.time()
    data = get_stations(url)
    logger.debug("received " + str(len(data)) + " stations in " + str(time.time() - start) + " s")
    count = 0
    start = time.time()
    try:
        db.begin_transaction()
        db.delete_all()
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
        db.commit()
        logger.info("imported " + str(count) + " stations in " + str(time.time() - start) + " s")
    except Exception as e:
        logger.exception(str(e))
        db.rollback()
