
import json
import time
import traceback

import requests
import stations

# TODO Add actual version
__USER_USER_AGENT__   = "cora/0.10.0 (com.github/sweintritt/cora)"
__RADIO_BROWSER_URL__ = "https://de1.api.radio-browser.info/json/stations"

def get_stations():
    headers = { 'User-Agent': __USER_USER_AGENT__ }
    response = requests.get(__RADIO_BROWSER_URL__, headers=headers)
    if not response.ok:
        raise Exception("Error while loading stations")
    return json.loads(response.content)

def import_stations(db):
    print("loading stations")
    start = time.time()
    data = get_stations()
    # TODO clear database
    print("recieved " + str(len(data)) + " stations in " + str(time.time() - start) + " s")
    count = 0
    start = time.time()
    try:
        #db.begin_transaction()
        db.execute("delete from stations;")
        list = []
        for station in data:
            name = station["name"]
            genre = station["tags"]
            country = station["country"]
            language = station["language"]
            description = station["name"]
            # TODO only add the second if it differs
            urls = [station["url"], station["url_resolved"]]
            list.append([name, 'radio-browser', genre, country, language, description, json.dumps(urls)])
            count += 1
        #db.commit()
        db.executemany('insert into stations (name, addedBy, genre, country, language, description, urls) values(?, ?, ?, ?, ?, ?, ?);', list)
        print("imported " + str(count) + " stations in " + str(time.time() - start) + " s")
    except Exception as e:
        print("error: " + str(e))
        traceback.print_exc()
        #db.rollback()
