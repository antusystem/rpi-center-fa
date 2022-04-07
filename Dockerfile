FROM python:3.10.2-slim as os-base
LABEL maintainer="Alejandro Antunes <antusystem@vivaldi.net>"

ENV PYTHONUNBUFFERED=1
ENV PYTHONDONTWRITEBYTECODE=1
ENV TZ=America/Caracas
RUN apt-get update && apt-get upgrade -y && apt-get install -y --no-install-recommends curl nano

FROM os-base as poetry-base

RUN curl -sSL https://install.python-poetry.org | python3 -
ENV PATH="/root/.local/bin:$PATH"
RUN poetry config virtualenvs.create false

FROM poetry-base as app-base

ARG APPDIR=rpi
RUN mkdir $APPDIR
WORKDIR $APPDIR
COPY . .
RUN poetry lock
RUN poetry install --no-dev

FROM app-base as main

EXPOSE 8000
CMD ["uvicorn", "main:app", "--host", "0.0.0.0", "--port", "8000", "--reload" ]


