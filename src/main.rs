use anyhow::Result;
use clap::{Parser, Subcommand};
use cora::{import_stations, Player, Settings, Stations, LAST_PLAYED};
use std::io;
use std::path::Path;

#[derive(Parser)]
#[command(name = "cora", about = "Play internet radio streams on your console")]
struct Cli {
    #[arg(short, long, action = clap::ArgAction::SetTrue)]
    debug: bool,
    #[command(subcommand)]
    command: Command,
}

#[derive(Subcommand)]
enum Command {
    Import {
        #[arg(long)]
        url: Option<String>,
    },
    Search {
        keywords: Vec<String>,
    },
    Info {
        id: i64,
    },
    List,
    Version,
    Play {
        keywords: Vec<String>,
    },
}

fn main() -> Result<()> {
    let cli = Cli::parse();
    let home = std::env::var_os("HOME").ok_or_else(|| anyhow::anyhow!("HOME is not set"))?;
    run_with_home(cli, Path::new(&home))
}

fn run_with_home(cli: Cli, home: &Path) -> Result<()> {
    env_logger::Builder::from_env(env_logger::Env::default().default_filter_or(if cli.debug {
        "debug"
    } else {
        "info"
    }))
    .format_timestamp(None)
    .init();
    run(cli, home)
}

fn run(cli: Cli, home: &Path) -> Result<()> {
    let path = home.join(".cora.sqlite");
    let stations = Stations::open(&path)?;
    let settings = Settings::open(&path)?;
    let mut player = Player::new();
    match cli.command {
        Command::Import { url } => println!(
            "imported {} stations",
            import_stations(&stations, url.as_deref())?
        ),
        Command::Search { keywords } => {
            let results = stations.find_all_by_keywords(&keywords)?;
            if results.is_empty() {
                println!("No stations found");
            }
            for station in results {
                print_station(&station);
            }
        }
        Command::Info { id } => match stations.find_by_id(id)? {
            Some(station) => print_info(&station),
            None => println!("No station found for {id}"),
        },
        Command::List => {
            for station in stations.get_all()? {
                print_station(&station);
            }
        }
        Command::Version => println!("cora - Version {}", env!("CARGO_PKG_VERSION")),
        Command::Play { keywords } => play(&keywords, &stations, &settings, &mut player)?,
    }
    player.stop()?;
    Ok(())
}

fn play(
    keywords: &[String],
    stations: &Stations,
    settings: &Settings,
    player: &mut Player,
) -> Result<()> {
    let first = keywords
        .first()
        .ok_or_else(|| anyhow::anyhow!("a station ID or keyword is required"))?;
    let station = if first == "last" {
        settings
            .get(LAST_PLAYED)?
            .and_then(|id| id.parse().ok())
            .map_or(Ok(None), |id| stations.find_by_id(id))?
    } else if first == "random" {
        stations.get_random()?
    } else if let Ok(id) = first.parse::<i64>() {
        stations.find_by_id(id)?
    } else {
        stations.find_by_keywords(keywords)?
    };
    let Some(station) = station else {
        println!("No station found for {}", keywords.join(" "));
        return Ok(());
    };
    let index = keywords
        .get(1)
        .and_then(|value| value.parse::<usize>().ok())
        .unwrap_or(0);
    let url = station
        .urls
        .get(index)
        .or_else(|| station.urls.first())
        .ok_or_else(|| anyhow::anyhow!("station has no stream URL"))?;
    println!("playing {}", station.name.trim());
    player.play(url)?;
    settings.save(LAST_PLAYED, station.id)?;
    println!("Press enter to stop playing");
    let mut input = String::new();
    io::stdin().read_line(&mut input)?;
    Ok(())
}

fn print_station(station: &cora::Station) {
    println!(
        "id:{}, name:{}, genre:{}, country:{}",
        station.id, station.name, station.genre, station.country
    );
}

fn print_info(station: &cora::Station) {
    println!("      station: {}", station.name.trim());
    println!("        genre: {}", station.genre.trim());
    println!("      country: {}", station.country.trim());
    println!("     language: {}", station.language.trim());
    println!("  description: {}", station.description.trim());
    for (index, url) in station.urls.iter().enumerate() {
        println!("       url[{index}]: {url}");
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn station() -> cora::Station {
        cora::Station {
            id: 0,
            name: "Test FM".into(),
            genre: "rock".into(),
            country: "Germany".into(),
            language: "German".into(),
            description: "A test station".into(),
            urls: vec!["https://example.test/stream".into()],
        }
    }

    fn temp_home() -> std::path::PathBuf {
        let path = std::env::temp_dir().join(format!(
            "cora-test-{}",
            std::time::SystemTime::now()
                .duration_since(std::time::UNIX_EPOCH)
                .unwrap()
                .as_nanos()
        ));
        std::fs::create_dir(&path).unwrap();
        path
    }

    fn run_command(command: Command) -> Result<()> {
        let home = temp_home();
        let result = run(
            Cli {
                debug: false,
                command,
            },
            &home,
        );
        std::fs::remove_dir_all(home).unwrap();
        result
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
    fn run_handles_database_commands() {
        let home = temp_home();
        let path = home.join(".cora.sqlite");
        let stations = Stations::open(&path).unwrap();
        stations.insert(&[station()]).unwrap();

        run(
            Cli {
                debug: false,
                command: Command::Search {
                    keywords: vec!["Test".into()],
                },
            },
            &home,
        )
        .unwrap();
        run(
            Cli {
                debug: false,
                command: Command::Info { id: 1 },
            },
            &home,
        )
        .unwrap();
        run(
            Cli {
                debug: false,
                command: Command::List,
            },
            &home,
        )
        .unwrap();
        std::fs::remove_dir_all(home).unwrap();
    }

    #[test]
    fn play_requires_keywords() {
        let db = Stations::open(":memory:").unwrap();
        let settings = Settings::open(":memory:").unwrap();
        let mut player = Player::new();

        assert!(play(&[], &db, &settings, &mut player).is_err());
    }

    #[test]
    fn play_handles_missing_station_and_stream_url() {
        let db = Stations::open(":memory:").unwrap();
        let settings = Settings::open(":memory:").unwrap();
        let mut player = Player::new();

        play(&["missing".into()], &db, &settings, &mut player).unwrap();

        let mut station = station();
        station.urls.clear();
        db.insert(&[station]).unwrap();
        assert!(play(&["1".into()], &db, &settings, &mut player).is_err());
        assert!(play(&["random".into()], &db, &settings, &mut player).is_err());
        settings.save(LAST_PLAYED, 1).unwrap();
        assert!(play(&["last".into()], &db, &settings, &mut player).is_err());
    }

    #[test]
    fn station_formatters_handle_all_fields() {
        let station = station();
        print_station(&station);
        print_info(&station);
    }

    #[test]
    fn run_handles_empty_results_and_version() {
        run_command(Command::Search {
            keywords: vec!["missing".into()],
        })
        .unwrap();
        run_command(Command::Info { id: 1 }).unwrap();
        run_command(Command::List).unwrap();
        run_command(Command::Version).unwrap();
    }

    #[test]
    fn run_imports_stations_and_dispatches_play() {
        let home = temp_home();
        let (url, server) = serve_response(
            r#"[{
                "name": "Imported",
                "tags": "",
                "country": "",
                "language": "",
                "url": "https://example.test/stream",
                "url_resolved": "https://example.test/stream"
            }]"#,
        );
        run(
            Cli {
                debug: false,
                command: Command::Import { url: Some(url) },
            },
            &home,
        )
        .unwrap();
        server.join().unwrap();

        assert_eq!(
            Stations::open(home.join(".cora.sqlite"))
                .unwrap()
                .get_all()
                .unwrap()[0]
                .name,
            "Imported"
        );
        assert!(run(
            Cli {
                debug: false,
                command: Command::Play { keywords: vec![] },
            },
            &home,
        )
        .is_err());
        std::fs::remove_dir_all(home).unwrap();
    }

    #[test]
    fn run_with_home_initializes_logging() {
        let home = temp_home();
        run_with_home(
            Cli {
                debug: false,
                command: Command::Version,
            },
            &home,
        )
        .unwrap();
        std::fs::remove_dir_all(home).unwrap();
    }
}
