"""Raspberry Pi API Center."""
from fastapi import FastAPI, Body, Request
from fastapi.responses import HTMLResponse
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates
from loguru import logger

from api import DBHandler, TempData

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
    version="0.1.7",
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

# app.include_router(unsplash.router)

app.mount("/static", StaticFiles(directory="static/css"), name="static")

templates = Jinja2Templates(directory="templates")


@app.get("/", response_class=HTMLResponse)
async def home(request: Request):
    data = {
        "kitchen": "20.13"
    }
    return templates.TemplateResponse("temperature.html", {"request": request, "data": data})

@app.get("/temperature/", response_class=HTMLResponse)
async def page(request: Request):
    logger.info(f"Request: {request}")
    db_handler = DBHandler()
    last_value = db_handler.last_value()
    return templates.TemplateResponse("temperature.html", {"request": request, "last_value": last_value})


@app.post("/temperature/register", response_class=HTMLResponse)
async def reg_temp(request: Request, data: TempData = Body(...)):
    logger.info(f"Request: {request}")
    logger.info(f"data: {data}")
    db_handler = DBHandler()
    db_handler.register_value(dict(data))
    return templates.TemplateResponse("temperature.html", {"request": request, "data": data})
