"""Handler for database queries."""
import os
from datetime import datetime

from dotenv import load_dotenv
from loguru import logger
import psycopg


class DBHandler:
    """Handler for database queries.

    Methods
    -------

    """

    def __init__(self):
        """Class initialization.

        Creates the connection to the database.
        """
        logger.info("Constructing DBHandler")
        load_dotenv()
        host = os.getenv("host")
        port = os.getenv("port")
        username = os.getenv("username")
        password = os.getenv("password")
        name = os.getenv("name")
        logger.trace("Connecting to DB")
        self.conn = psycopg.connect(dbname=name,
                                    user=username,
                                    password=password,
                                    host=host,
                                    port=port)
        # Creating cursor
        self.cur = self.conn.cursor()
        logger.success("DBHandler created")

    def last_value(self) -> float:
        """Get last value from database.
        
        Return
        ------
        float
            Last value of the database.

        """
        query = """
            SELECT value FROM temp_reg LIMIT 1 ORDER BY ID ASC
        """
        self.cur.execute(query)
        records = self.cur.fetchall()
        return records[0][0]

    def register_value(self, data: dict[str, str | float]):
        """Register new value.

        Parameters
        ----------
        data: dict[str, str | float]
            Data containing the location and value obtatined.

        """
        query = """
            INSERT INTO temp_reg (date_at, fk_location, value)
            VALUES (%s, %s, %s, %s)
        """
        self.cur.execute(query)
        records = self.cur.fetchall()
        return records[0][0]
