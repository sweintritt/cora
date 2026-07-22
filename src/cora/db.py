import logging
import sqlite3

logger = logging.getLogger(__name__)


class Db:

    def __init__(self: Db):
        self.connection = None
        self.cursor = None

    def open(self: Db, file: str):
        self.connection = sqlite3.connect(file)
        self.connection.isolation_level = None
        self.cursor = self.connection.cursor()

    def user(self: Db, connection: sqlite3.Connection):
        self.connection = connection
        self.connection.isolation_level = None
        self.cursor = self.connection.cursor()

    def close(self: Db):
        self.connection.close()

    def begin_transaction(self: Db):
        logger.debug("begin transaction")
        self.cursor.execute("begin transaction;")

    def commit(self: Db):
        logger.debug("commmit")
        self.connection.commit();

    def rollback(self: Db):
        logger.debug("rollback")
        self.connection.rollback();

    def execute(self: Db, query: str):
        self.cursor.execute(query)

    def executemany(self: Db, query: str, data):
        self.cursor.executemany(query, data)
        self.cursor.executemany(query, data)
