"""Raspberry Pi API Center."""
from fastapi import FastAPI, Query, Path, Body, Request
from fastapi.responses import HTMLResponse
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates
from loguru import logger
from pydantic import BaseModel, Field
from api.db_handler import DBHandler

logger.trace("Starting FastAPI")
description = """
Raspberry Pi API Center 🚀.

This API is to setup a web interface to
check the temperature obtained by some sensors.

## Items

You can **read temperature**.

## Users

You will be able to:

* **Check Temperature** (_not implemented_).

"""
app = FastAPI(
    title="RPi-API-Center",
    description=description,
    version="0.1.2",
    # terms_of_service="http://example.com/terms/",
    contact={
        "name": "Alejandro Antunes",
        "url": "https://github.com/antusystem",
        "email": "antusystem@vivaldi.net",
    },
    license_info={
        "name": "Apache 2.0",
        "url": "https://www.apache.org/licenses/LICENSE-2.0.html",
    },
)


class TempData(BaseModel):
    location: str = Field(...,
                          title="Location Data",
                          alias="loc-data",
                          min_length=3,
                          max_length=20,
                          description="Location from data",
                          )
    temperature: float = Body(...,
                              title="Temperature Data",
                              alias="temp-data",
                              gt=-273.15,
                              description="Data obtained by the sensor",
                              )

# db_handler = DBHandler()
# app.include_router(unsplash.router)

app.mount("/static", StaticFiles(directory="static/css"), name="static")

templates = Jinja2Templates(directory="templates")

@app.get("/", response_class=HTMLResponse)
async def home(request: Request):
    data = {
        "kitchen": "20.13"
    }
    return templates.TemplateResponse("temperature.html", {"request": request, "data": data})

@app.post("/temperature/", response_class=HTMLResponse)
async def page(request: Request, data: TempData):
    logger.info(f"Request: {request}")
    logger.info(f"data: {data}")
    return templates.TemplateResponse("temperature.html", {"request": request, "data": data})
