use anyhow::Result;
use clap::{Parser, Subcommand};
use cora::{import_stations, Player, Settings, Stations, LAST_PLAYED};
use std::io;

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
    env_logger::Builder::from_env(env_logger::Env::default().default_filter_or(if cli.debug {
        "debug"
    } else {
        "info"
    }))
    .format_timestamp(None)
    .init();
    let home = std::env::var_os("HOME").ok_or_else(|| anyhow::anyhow!("HOME is not set"))?;
    let path = std::path::PathBuf::from(home).join(".cora.sqlite");
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
