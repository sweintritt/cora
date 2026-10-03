use anyhow::{Context, Result};
use rusqlite::{params, Connection, OptionalExtension};
use serde::Deserialize;
use std::path::Path;

pub mod player;

pub use player::Player;

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

#[cfg(test)]
mod tests {
    use super::*;

    fn station_db() -> Stations {
        Stations::open(":memory:").unwrap()
    }

    fn serve_response(body: &str) -> (String, std::thread::JoinHandle<()>) {
        use std::io::{Read, Write};
        use std::net::TcpListener;

        let listener = TcpListener::bind("127.0.0.1:0").unwrap();
        let address = listener.local_addr().unwrap();
        let body = body.to_owned();
        let handle = std::thread::spawn(move || {
            let (mut stream, _) = listener.accept().unwrap();
            let mut request = [0; 1024];
            stream.read(&mut request).unwrap();
            write!(
                stream,
                "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: {}\r\nConnection: close\r\n\r\n{}",
                body.len(),
                body
            )
            .unwrap();
        });
        (format!("http://{address}/stations"), handle)
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

    #[test]
    fn stations_find_by_keywords_and_get_random() {
        let db = station_db();
        db.insert(&[Station {
            id: 0,
            name: "Jazz FM".into(),
            genre: "jazz".into(),
            country: "France".into(),
            language: "French".into(),
            description: "Smooth jazz".into(),
            urls: vec!["https://example.test/jazz".into()],
        }])
        .unwrap();

        assert_eq!(
            db.find_by_keywords(&["jazz".into()]).unwrap().unwrap().name,
            "Jazz FM"
        );
        assert!(db.find_by_keywords(&["metal".into()]).unwrap().is_none());
        assert_eq!(db.get_random().unwrap().unwrap().name, "Jazz FM");
    }

    #[test]
    fn invalid_url_json_returns_errors() {
        assert!(deserialize_urls("not JSON").is_err());

        let db = station_db();
        db.connection
            .execute(
                "INSERT INTO stations
                 (name, addedBy, genre, country, language, description, urls)
                 VALUES ('Broken', 'test', '', '', '', '', 'not JSON')",
                [],
            )
            .unwrap();
        assert!(db.find_by_id(1).is_err());
    }

    #[test]
    fn import_replaces_stations_and_rolls_back_on_request_failure() {
        let db = station_db();
        db.insert(&[Station {
            id: 0,
            name: "Existing".into(),
            genre: String::new(),
            country: String::new(),
            language: String::new(),
            description: String::new(),
            urls: vec![],
        }])
        .unwrap();

        let (url, server) = serve_response(
            r#"[{
                "name": "Imported",
                "tags": "jazz",
                "country": "Germany",
                "language": "German",
                "url": "http://example.test/stream",
                "url_resolved": "https://example.test/stream"
            }]"#,
        );
        assert_eq!(import_stations(&db, Some(&url)).unwrap(), 1);
        server.join().unwrap();
        assert_eq!(db.get_all().unwrap()[0].name, "Imported");

        assert!(import_stations(&db, Some("http://127.0.0.1:1")).is_err());
        assert_eq!(db.get_all().unwrap()[0].name, "Imported");
    }
}
