import sqlite3


class Db:

    def __init__(self):
        self.cursor = None
        self.connection = None

    def open(self, file):
        self.connection = sqlite3.connect(file)
        self.connection.isolation_level = None
        self.cursor = self.connection.cursor()

    def close(self):
        self.connection.close()

    def begin_transaction(self):
        print("begin transaction")
        self.cursor.execute("begin transaction;")

    def commit(self):
        print("commmit")
        self.cursor.execute("commit;")

    def rollback(self):
        print("rollback")
        self.cursor.execute("rollback;")

    def execute(self, query):
        self.cursor.execute(query)

    def executemany(self, query, data):
        self.cursor.executemany(query, data)

