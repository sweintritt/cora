# Copilot instructions

## Project

cora (**Co**nsole **Ra**dio) is a very simple command line application to play internet radio streams. Python + VLC + SQLite.
Stations are imported from *https://www.radio-browser.info/* into a local sqlite database for faster and easier access.

## Architecture
- `/src/` - Application
- `/tests/` - unittest for tests

## Global Rules
- Never commit secrets or API keys
- All PRs require passing tests before merge
- Create tests
- Check for security issues

## Commands
- `make clean` - clean up the build directory
- `make rpm` - Build a rpm package
- `make test` - Run all tests