# Copilot instructions

## Project

cora (**Co**nsole **Ra**dio) is a very simple commandline application to play internet radio streams written in python
It uses libVLC and SQLite. Stations are imported from *https://www.radio-browser.info/* into a local database for
faster and easier access.

## Architecture

- `/src/cora/` - Application
- `/tests/` - Unit tests
- `/tasks/` - Copilot tasks

## Global Rules

- Never commit secrets or API keys
- All PRs require passing tests before merge
- Add or update focused regression tests for changed behaviour
- Check for security issues

## Commands

- `make clean` - clean up the build directory
- `make rpm` - build an RPM package
- `make test` - run all tests