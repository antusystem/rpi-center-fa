"""Raspberry Pi API Center."""
from fastapi import FastAPI, Query, Path, Body
from loguru import logger
from pydantic import BaseModel

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
    version="0.1.0",
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


@app.get("/")
async def root():
    """Root."""
    logger.info("Starting Root")
    return {"message": "Hello World"}

""" 
@app.get("/items/{item_id}")
    async def read_item(item_id: int):
    return {"item_id": item_id}

@app.get("/models/{model_name}")
async def get_model(model_name: ModelName):
    if model_name == ModelName.alexnet:
        return {"model_name": model_name, "message": "Deep Learning FTW!"}

    if model_name.value == "lenet":
        return {"model_name": model_name, "message": "LeCNN all the images"}

    return {"model_name": model_name, "message": "Have some residuals"}

@app.get("/files/{file_path:path}")
async def read_file(file_path: str):
    return {"file_path": file_path}

fake_items_db = [{"item_name": "Foo"}, {"item_name": "Bar"}, {"item_name": "Baz"}]  

@app.get("/items/{item_id}")
async def read_item(item_id: str, q: str | None = None, short: bool = False):
    item = {"item_id": item_id}
    if q:
        item.update({"q": q})
    if not short:
        item.update(
            {"description": "This is an amazing item that has a long description"}
        )
    return item

class Item(BaseModel):
    name: str
    description: str | None = None
    price: float
    tax: float | None = None

@app.post("/items/")
async def create_item(item: Item):
    item_dict = item.dict()
    if item.tax:
        price_with_tax = item.price + item.tax
        item_dict.update({"price_with_tax": price_with_tax})
    return item_dict


@app.get("/items/")
async def read_items(
    q: str | None = Query(None,
                          title="My Query string",
                          alias="the-item-query",
                          min_length=3,
                          description=("1Query string for the items to search"
                                       "in the database that have a good match"),
                          max_length=50, #regex="^fixedquery$")
                          )):
    results = {"items": [{"item_id": "Foo"}, {"item_id": "Bar"}]}
    if q:
        results.update({"q": q})
    return results

@app.get("/itemss/")
async def read_itemss(q: list[str] = Query(["foo", "bar"])):
    query_items = {"q": q}
    return query_items

@app.get("/itemsss/{item_id}")
async def read_items(
    *, item_id: int = Path(..., title="The ID of the item to get", ge=1),
    q: str | None = Query(None, alias="item-query"),
    size: float = Query(..., gt=0, lt=10.5),

):
    results = {"item_id": item_id}
    if q:
        results.update({"q": q})
    return results

class Item(BaseModel):
    name: str
    description: str | None = None
    price: float
    tax: float | None = None

    class Config:
        schema_extra = {
            "example": {
                "name": "Foo",
                "description": "A very nice Item",
                "price": 35.4,
                "tax": 3.2,
            }
        }


@app.put("/cosas/{item_id}",
         tags=["items", "users"],
         summary="Create an item",
         description=("Se puede usar description o el docstring para esto "
                      "price, tax and a set of unique tags"),
         response_description="The created item",
         )
async def update_item(item_id: int, item: Item):
    results = {"item_id": item_id, "item": item}
    return results
"""