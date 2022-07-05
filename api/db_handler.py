"""Handler for database queries."""
import os
from datetime import datetime

from dotenv import load_dotenv
from loguru import logger
import psycopg

from .models import TempData


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
            SELECT value FROM temperature ORDER BY date_at ASC LIMIT 1
        """
        self.cur.execute(query)
        records = self.cur.fetchall()
        return records[0][0]

    def register_value(self, data: TempData):
        """Register new value.

        Parameters
        ----------
        data: dict[str, str | float]
            Data containing the location and value obtatined.

        """
        query = """
            INSERT INTO temperature (date_at, location, value)
            VALUES (%s, %s, %s) RETURNING id
        """
        logger.info(f"data register: {data}")
        date_at = datetime.now()
        self.cur.execute(query, (date_at, data["location"], data["value"]))
        records = self.cur.fetchall()
        logger.info(f"records: {records}")
        self.conn.commit()
        self.cur.close()
        self.conn.close()
        return records[0][0]
