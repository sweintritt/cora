use anyhow::{bail, Context, Result};
use rusqlite::{params, Connection, OptionalExtension};
use serde::Deserialize;
use std::path::Path;

pub const LAST_PLAYED: &str = "last.played";
pub const RADIO_BROWSER_URL: &str = "https://de1.api.radio-browser.info/json/stations";

#[derive(Debug, Clone, PartialEq, Eq)]
pub struct Station {
    pub id: i64,
    pub name: String,
    pub genre: String,
    pub country: String,
    pub language: String,
    pub description: String,
    pub urls: Vec<String>,
}

pub struct Stations {
    pub connection: Connection,
}

impl Stations {
    pub fn open(path: impl AsRef<Path>) -> Result<Self> {
        let connection = Connection::open(path)?;
        connection.execute_batch(
            "CREATE TABLE IF NOT EXISTS stations (
                name TEXT NOT NULL, addedBy TEXT NOT NULL, genre TEXT NOT NULL,
                country TEXT NOT NULL, language TEXT NOT NULL, description TEXT,
                urls TEXT NOT NULL
            );",
        )?;
        Ok(Self { connection })
    }

    pub fn find_by_id(&self, id: i64) -> Result<Option<Station>> {
        self.connection
            .query_row(
                "SELECT rowid, name, genre, country, language, description, urls
                 FROM stations WHERE rowid = ?1",
                [id],
                station_from_row,
            )
            .optional()
            .context("looking up station")
    }

    pub fn find_all_by_keywords(&self, keywords: &[String]) -> Result<Vec<Station>> {
        let ids = self.keyword_ids(keywords, "ORDER BY rowid")?;
        let mut stations = Vec::with_capacity(ids.len());
        for id in ids {
            if let Some(station) = self.find_by_id(id)? {
                stations.push(station);
            }
        }
        Ok(stations)
    }

    pub fn find_by_keywords(&self, keywords: &[String]) -> Result<Option<Station>> {
        let ids = self.keyword_ids(keywords, "ORDER BY random() LIMIT 1")?;
        ids.first()
            .copied()
            .map_or(Ok(None), |id| self.find_by_id(id))
    }

    fn keyword_ids(&self, keywords: &[String], ordering: &str) -> Result<Vec<i64>> {
        let conditions = if keywords.is_empty() {
            "1".to_string()
        } else {
            keywords
                .iter()
                .map(|_| "searchstring LIKE ?")
                .collect::<Vec<_>>()
                .join(" AND ")
        };
        let query = format!(
            "SELECT rowid FROM
             (SELECT rowid, name || ' ' || COALESCE(description, '') || ' ' || genre ||
              ' ' || country || ' ' || language AS searchstring FROM stations)
             WHERE {conditions} {ordering}"
        );
        let values = keywords
            .iter()
            .map(|keyword| format!("%{keyword}%"))
            .collect::<Vec<_>>();
        let mut statement = self.connection.prepare(&query)?;
        let rows =
            statement.query_map(rusqlite::params_from_iter(values.iter()), |row| row.get(0))?;
        Ok(rows.collect::<rusqlite::Result<Vec<i64>>>()?)
    }

    pub fn delete_all(&self) -> Result<()> {
        self.connection.execute("DELETE FROM stations", [])?;
        Ok(())
    }

    pub fn get_random(&self) -> Result<Option<Station>> {
        self.connection
            .query_row(
                "SELECT rowid, name, genre, country, language, description, urls
                 FROM stations ORDER BY random() LIMIT 1",
                [],
                station_from_row,
            )
            .optional()
            .context("selecting random station")
    }

    pub fn get_all(&self) -> Result<Vec<Station>> {
        let mut statement = self.connection.prepare(
            "SELECT rowid, name, genre, country, language, description, urls
             FROM stations ORDER BY rowid",
        )?;
        let rows = statement.query_map([], station_from_row)?;
        Ok(rows.collect::<rusqlite::Result<Vec<_>>>()?)
    }

    pub fn insert(&self, stations: &[Station]) -> Result<usize> {
        let transaction = self.connection.unchecked_transaction()?;
        insert_uncommitted(&transaction, stations)?;
        transaction.commit()?;
        Ok(stations.len())
    }
}

fn station_from_row(row: &rusqlite::Row<'_>) -> rusqlite::Result<Station> {
    let urls: String = row.get(6)?;
    Ok(Station {
        id: row.get(0)?,
        name: row.get(1)?,
        genre: row.get(2)?,
        country: row.get(3)?,
        language: row.get(4)?,
        description: row.get(5)?,
        urls: serde_json::from_str(&urls).map_err(|error| {
            rusqlite::Error::FromSqlConversionFailure(
                6,
                rusqlite::types::Type::Text,
                Box::new(error),
            )
        })?,
    })
}

pub fn serialize_urls(urls: &[String]) -> Result<String> {
    Ok(serde_json::to_string(urls)?)
}

pub fn deserialize_urls(value: &str) -> Result<Vec<String>> {
    Ok(serde_json::from_str(value)?)
}

pub struct Settings {
    connection: Connection,
}

impl Settings {
    pub fn open(path: impl AsRef<Path>) -> Result<Self> {
        let connection = Connection::open(path)?;
        connection.execute_batch(
            "CREATE TABLE IF NOT EXISTS settings
             (key TEXT NOT NULL PRIMARY KEY, value TEXT NOT NULL);",
        )?;
        Ok(Self { connection })
    }

    pub fn get(&self, key: &str) -> Result<Option<String>> {
        Ok(self
            .connection
            .query_row("SELECT value FROM settings WHERE key = ?1", [key], |row| {
                row.get(0)
            })
            .optional()?)
    }

    pub fn save(&self, key: &str, value: impl ToString) -> Result<()> {
        self.connection.execute(
            "INSERT OR REPLACE INTO settings (key, value) VALUES (?1, ?2)",
            params![key, value.to_string()],
        )?;
        Ok(())
    }
}

#[derive(Debug, Deserialize)]
struct RadioBrowserStation {
    name: String,
    tags: String,
    country: String,
    language: String,
    url: String,
    url_resolved: String,
}

pub fn import_stations(stations: &Stations, url: Option<&str>) -> Result<usize> {
    let client = reqwest::blocking::Client::builder()
        .user_agent(format!(
            "cora/{} (com.github/sweintritt/cora)",
            env!("CARGO_PKG_VERSION")
        ))
        .build()?;
    let endpoint = url.unwrap_or(RADIO_BROWSER_URL);
    stations.connection.execute_batch("BEGIN TRANSACTION")?;
    stations.delete_all()?;
    let mut total = 0;
    let mut offset = 0;
    let result = (|| -> Result<usize> {
        loop {
            let batch: Vec<RadioBrowserStation> = client
                .get(endpoint)
                .query(&[("offset", offset), ("limit", 5000)])
                .send()?
                .error_for_status()?
                .json()?;
            let last = batch.len() < 5000;
            let mapped = batch
                .into_iter()
                .map(map_radio_browser_station)
                .collect::<Vec<_>>();
            insert_uncommitted(&stations.connection, &mapped)?;
            total += mapped.len();
            if last {
                break;
            }
            offset += 5000;
        }
        Ok(total)
    })();
    match result {
        Ok(total) => {
            stations.connection.execute_batch("COMMIT")?;
            Ok(total)
        }
        Err(error) => {
            stations.connection.execute_batch("ROLLBACK")?;
            Err(error)
        }
    }
}

fn map_radio_browser_station(station: RadioBrowserStation) -> Station {
    let mut urls = vec![station.url.clone()];
    if station.url != station.url_resolved {
        urls.push(station.url_resolved);
    }
    Station {
        id: 0,
        name: station.name.clone(),
        genre: station.tags,
        country: station.country,
        language: station.language,
        description: station.name,
        urls,
    }
}

fn insert_uncommitted(connection: &Connection, stations: &[Station]) -> Result<()> {
    let mut statement = connection.prepare(
        "INSERT INTO stations
         (name, addedBy, genre, country, language, description, urls)
         VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7)",
    )?;
    for station in stations {
        statement.execute(params![
            station.name,
            "radio-browser",
            station.genre,
            station.country,
            station.language,
            station.description,
            serialize_urls(&station.urls)?
        ])?;
    }
    Ok(())
}

pub struct Player {
    lib: Option<libloading::Library>,
    instance: *mut std::ffi::c_void,
    media: *mut std::ffi::c_void,
    player: *mut std::ffi::c_void,
}

impl Player {
    pub fn new() -> Self {
        Self {
            lib: None,
            instance: std::ptr::null_mut(),
            media: std::ptr::null_mut(),
            player: std::ptr::null_mut(),
        }
    }

    pub fn play(&mut self, url: &str) -> Result<()> {
        self.stop()?;
        let library = load_libvlc().context("loading libVLC (install VLC)")?;
        let url = std::ffi::CString::new(url).context("stream URL contains a NUL byte")?;
        unsafe {
            let new_instance = symbol::<
                unsafe extern "C" fn(i32, *const *const i8) -> *mut std::ffi::c_void,
            >(&library, b"libvlc_new\0")?;
            let instance = new_instance(0, std::ptr::null());
            if instance.is_null() {
                bail!("libVLC could not create an instance");
            }
            let new_media = symbol::<
                unsafe extern "C" fn(*mut std::ffi::c_void, *const i8) -> *mut std::ffi::c_void,
            >(&library, b"libvlc_media_new_location\0")?;
            let media = new_media(instance, url.as_ptr());
            if media.is_null() {
                release_instance(&library, instance);
                bail!("libVLC could not create media for the stream");
            }
            let new_player = symbol::<
                unsafe extern "C" fn(*mut std::ffi::c_void) -> *mut std::ffi::c_void,
            >(&library, b"libvlc_media_player_new\0")?;
            let player = new_player(instance);
            if player.is_null() {
                release_media(&library, media);
                release_instance(&library, instance);
                bail!("libVLC could not create a media player");
            }
            let set_media = symbol::<
                unsafe extern "C" fn(*mut std::ffi::c_void, *mut std::ffi::c_void),
            >(&library, b"libvlc_media_player_set_media\0")?;
            set_media(player, media);
            let play = symbol::<unsafe extern "C" fn(*mut std::ffi::c_void) -> i32>(
                &library,
                b"libvlc_media_player_play\0",
            )?;
            if play(player) != 0 {
                release_player(&library, player);
                release_media(&library, media);
                release_instance(&library, instance);
                bail!("libVLC could not start playback");
            }
            self.instance = instance;
            self.media = media;
            self.player = player;
        }
        self.lib = Some(library);
        Ok(())
    }

    pub fn stop(&mut self) -> Result<()> {
        if let Some(library) = self.lib.take() {
            unsafe {
                if !self.player.is_null() {
                    let stop = symbol::<unsafe extern "C" fn(*mut std::ffi::c_void)>(
                        &library,
                        b"libvlc_media_player_stop\0",
                    )?;
                    stop(self.player);
                    release_player(&library, self.player);
                }
                if !self.media.is_null() {
                    release_media(&library, self.media);
                }
                if !self.instance.is_null() {
                    release_instance(&library, self.instance);
                }
            }
        }
        self.player = std::ptr::null_mut();
        self.media = std::ptr::null_mut();
        self.instance = std::ptr::null_mut();
        Ok(())
    }
}

impl Drop for Player {
    fn drop(&mut self) {
        let _ = self.stop();
    }
}

fn load_libvlc() -> Result<libloading::Library> {
    for name in ["libvlc.so.5", "libvlc.so", "libvlc.dylib", "libvlc.dll"] {
        if let Ok(library) = unsafe { libloading::Library::new(name) } {
            return Ok(library);
        }
    }
    bail!("libVLC shared library was not found")
}

unsafe fn symbol<'a, T>(
    library: &'a libloading::Library,
    name: &[u8],
) -> Result<libloading::Symbol<'a, T>> {
    Ok(library.get(name)?)
}

unsafe fn release_player(library: &libloading::Library, player: *mut std::ffi::c_void) {
    if let Ok(release) = symbol::<unsafe extern "C" fn(*mut std::ffi::c_void)>(
        library,
        b"libvlc_media_player_release\0",
    ) {
        release(player);
    }
}

unsafe fn release_media(library: &libloading::Library, media: *mut std::ffi::c_void) {
    if let Ok(release) =
        symbol::<unsafe extern "C" fn(*mut std::ffi::c_void)>(library, b"libvlc_media_release\0")
    {
        release(media);
    }
}

unsafe fn release_instance(library: &libloading::Library, instance: *mut std::ffi::c_void) {
    if let Ok(release) =
        symbol::<unsafe extern "C" fn(*mut std::ffi::c_void)>(library, b"libvlc_release\0")
    {
        release(instance);
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn station_db() -> Stations {
        Stations::open(":memory:").unwrap()
    }

    #[test]
    fn urls_round_trip() {
        let urls = vec![
            "http://one.example".to_string(),
            "http://two.example".to_string(),
        ];
        assert_eq!(
            deserialize_urls(&serialize_urls(&urls).unwrap()).unwrap(),
            urls
        );
    }

    #[test]
    fn keyword_search_is_order_independent() {
        let db = station_db();
        db.insert(&[Station {
            id: 0,
            name: "Morning Jazz".into(),
            genre: "jazz".into(),
            country: "Germany".into(),
            language: "English".into(),
            description: "Relaxing music".into(),
            urls: vec![],
        }])
        .unwrap();
        assert_eq!(
            db.find_all_by_keywords(&["jazz".into(), "morning".into()])
                .unwrap()[0]
                .name,
            "Morning Jazz"
        );
        assert_eq!(
            db.find_all_by_keywords(&["morning".into(), "jazz".into()])
                .unwrap()[0]
                .name,
            "Morning Jazz"
        );
    }

    #[test]
    fn empty_database_has_no_random_station() {
        assert!(station_db().get_random().unwrap().is_none());
    }

    #[test]
    fn station_crud_preserves_fields_and_delete_all() {
        let db = station_db();
        let station = Station {
            id: 0,
            name: "Test FM".into(),
            genre: "rock".into(),
            country: "Germany".into(),
            language: "German".into(),
            description: "A test station".into(),
            urls: vec!["https://example.test/stream".into()],
        };
        db.insert(std::slice::from_ref(&station)).unwrap();

        let stored = db.find_by_id(1).unwrap().unwrap();
        assert_eq!(stored.id, 1);
        assert_eq!(stored.name, station.name);
        assert_eq!(stored.genre, station.genre);
        assert_eq!(stored.country, station.country);
        assert_eq!(stored.language, station.language);
        assert_eq!(stored.description, station.description);
        assert_eq!(stored.urls, station.urls);
        assert_eq!(db.get_all().unwrap().len(), 1);

        db.delete_all().unwrap();
        assert!(db.find_by_id(1).unwrap().is_none());
    }

    #[test]
    fn keyword_search_with_no_keywords_returns_all_stations() {
        let db = station_db();
        db.insert(&[
            Station {
                id: 0,
                name: "One".into(),
                genre: "rock".into(),
                country: "Germany".into(),
                language: "German".into(),
                description: "First".into(),
                urls: vec![],
            },
            Station {
                id: 0,
                name: "Two".into(),
                genre: "jazz".into(),
                country: "France".into(),
                language: "French".into(),
                description: "Second".into(),
                urls: vec![],
            },
        ])
        .unwrap();

        assert_eq!(db.find_all_by_keywords(&[]).unwrap().len(), 2);
    }

    #[test]
    fn settings_round_trip_and_replace_existing_value() {
        let settings = Settings::open(":memory:").unwrap();
        assert_eq!(settings.get("volume").unwrap(), None);
        settings.save("volume", 25).unwrap();
        assert_eq!(settings.get("volume").unwrap(), Some("25".into()));
        settings.save("volume", "80").unwrap();
        assert_eq!(settings.get("volume").unwrap(), Some("80".into()));
    }

    #[test]
    fn radio_browser_mapping_uses_resolved_url_only_when_different() {
        let station = map_radio_browser_station(RadioBrowserStation {
            name: "Example".into(),
            tags: "rock,pop".into(),
            country: "Germany".into(),
            language: "German".into(),
            url: "http://example.test/stream".into(),
            url_resolved: "https://example.test/stream".into(),
        });
        assert_eq!(station.description, "Example");
        assert_eq!(
            station.urls,
            vec!["http://example.test/stream", "https://example.test/stream",]
        );

        let same_url = map_radio_browser_station(RadioBrowserStation {
            name: "Same URL".into(),
            tags: "".into(),
            country: "".into(),
            language: "".into(),
            url: "https://example.test/live".into(),
            url_resolved: "https://example.test/live".into(),
        });
        assert_eq!(same_url.urls, vec!["https://example.test/live"]);
    }
}
