# Copilot instructions

## Project

cora (**Co**nsole **Ra**dio) is a very simple Rust command line application to play internet radio streams. libVLC + SQLite.
Stations are imported from *https://www.radio-browser.info/* into a local sqlite database for faster and easier access.

## Architecture
- `/src/` - Application
- `/src/` - Rust application and unit tests

## Global Rules
- Never commit secrets or API keys
- All PRs require passing tests before merge
- Create tests
- Check for security issues

## Commands
- `cargo clean` - clean up the build directory
- `cargo generate-rpm` - build an RPM package
- `cargo test` - run all tests