import logging
import sqlite3

logger = logging.getLogger(__name__)


class Db:

    def __init__(self) -> None:
        self.connection = None
        self.cursor = None

    def open(self, file: str) -> None:
        self.connection = sqlite3.connect(file)
        self.connection.isolation_level = None
        self.cursor = self.connection.cursor()

    def user(self, connection: sqlite3.Connection) -> None:
        self.connection = connection
        self.connection.isolation_level = None
        self.cursor = self.connection.cursor()

    def close(self) -> None:
        self.connection.close()

    def begin_transaction(self) -> None:
        logger.debug("begin transaction")
        self.cursor.execute("begin transaction;")

    def commit(self) -> None:
        logger.debug("commmit")
        self.connection.commit()

    def rollback(self) -> None:
        logger.debug("rollback")
        self.connection.rollback()

    def execute(self, query: str) -> sqlite3.Cursor:
        return self.cursor.execute(query)

    def executemany(self, query: str, data) -> None:
        self.cursor.executemany(query, data)
