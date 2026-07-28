import logging
import sqlite3

logger = logging.getLogger(__name__)


class Db:

    def __init__(self: Db) -> None:
        self.connection = None
        self.cursor = None

    def open(self: Db, file: str) -> None:
        self.connection = sqlite3.connect(file)
        self.connection.isolation_level = None
        self.cursor = self.connection.cursor()

    def user(self: Db, connection: sqlite3.Connection) -> None:
        self.connection = connection
        self.connection.isolation_level = None
        self.cursor = self.connection.cursor()

    def close(self: Db) -> None:
        self.connection.close()

    def begin_transaction(self: Db) -> None:
        logger.debug("begin transaction")
        self.cursor.execute("begin transaction;")

    def commit(self: Db) -> None:
        logger.debug("commmit")
        self.connection.commit();

    def rollback(self: Db) -> None:
        logger.debug("rollback")
        self.connection.rollback();

    def execute(self: Db, query: str) -> sqlite3.Cursor:
        return self.cursor.execute(query)

    def executemany(self: Db, query: str, data) -> None:
        self.cursor.executemany(query, data)
        self.cursor.executemany(query, data)
