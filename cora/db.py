import sqlite3

class Db:

    def __init__(self):
        self.cursor = None
        self.connection = None

    def open(self, file):
        self.connection = sqlite3.connect(file)
        self.cursor = self.connection.cursor()

    def close(self):
        self.connection.close()
        
    def begin_transaction(self):
        self.cursor.execute("BEGIN TRANSACTION;")

    def commit(self):
        self.cursor.execute("COMMIT;")

    def rollback(self):
        self.cursor.execute("ROLLBACK;")
        
    def execute(self, query):
        self.cursor.execute(query)

    def executemany(self, query, data):
        self.cursor.executemany(query, data)