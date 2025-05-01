
import json
import logging
import time
import traceback

import requests

logger = logging.getLogger(__name__)

# TODO: Add actual version
__USER_USER_AGENT__   = "cora/0.10.0 (com.github/sweintritt/cora)"
__RADIO_BROWSER_URL__ = "https://de1.api.radio-browser.info/json/stations"

def get_stations():
    headers = { 'User-Agent': __USER_USER_AGENT__ }
    response = requests.get(__RADIO_BROWSER_URL__, headers=headers)
    if not response.ok:
        raise Exception("Error while loading stations")
    return json.loads(response.content)

def import_stations(db):
    logger.debug("loading stations")
    start = time.time()
    data = get_stations()
    db.delete_all()
    logger.debug("recieved " + str(len(data)) + " stations in " + str(time.time() - start) + " s")
    count = 0
    start = time.time()
    try:
        db.begin_transaction()
        db.execute("delete from stations;")
        list = []
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
            list.append([name, 'radio-browser', genre, country, language, description, json.dumps(urls)])
            count += 1
        db.executemany('insert into stations (name, addedBy, genre, country, language, description, urls) values(?, ?, ?, ?, ?, ?, ?);', list)
        db.commit()
        logger.info("imported " + str(count) + " stations in " + str(time.time() - start) + " s")
    except Exception as e:
        logger.error("error: " + str(e))
        traceback.print_exc()
        db.rollback()
