"""Pydantic Models"""

from pydantic import BaseModel, Field


class TempData(BaseModel):
    location: str = Field(...,
                          title="Location Data",
                          alias="location",
                          min_length=3,
                          max_length=20,
                          description="Location from data",
                          )
    temperature: float = Field(...,
                               title="Temperature Data",
                               alias="temperature",
                               gt=-273.15,
                               description="Data obtained by the sensor",
                               )
