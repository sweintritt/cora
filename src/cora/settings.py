import logging

from cora.db import Db

__CREATE_TABLE_SQL__ = """CREATE TABLE IF NOT EXISTS settings
    (key TEXT NOT NULL PRIMARY KEY, value TEXT NOT NULL);"""
__GET_SETTING_SQL__ = "SELECT value FROM settings WHERE key = ?;"
__SAVE_SETTING_SQL__ = """INSERT OR REPLACE INTO settings (key, value)
    VALUES (?, ?);"""

logger = logging.getLogger(__name__)


class Settings(Db):
    """
    Store and retrieve settings
    """

    def open(self, file):
        super().open(file)
        self.execute(__CREATE_TABLE_SQL__)
        self.connection.commit()

    def get(self, key):
        result = self.cursor.execute(__GET_SETTING_SQL__, (key,))
        value = result.fetchone()
        logger.debug("get %s: %s", key, value)
        if value is not None:
            logger.debug("returning %s", value[0])
            return value[0]
        else:
            return None

    def save(self, key, value):
        self.cursor.execute(__SAVE_SETTING_SQL__, (key, value))
