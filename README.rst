RPi API Center
==============

This is and IoT project that focused on using a Raspberry Pi as the center
of supervision of temperature using `FastAPI <https://fastapi.tiangolo.com/>`_ to recieve
data from ESP32 microcontollers. This microcontollers query the sensors every minute and send
data to the Raspi using a server hosted there with FastAPI. This data will be showed in a
clear way afterwards.

TO DO
=====

* Define FastAPI structure
* Define HTML Template to show the data
* Security for the API
* ESP32 resquest to the API
* Set Database with Postgre SQL in a container with Docker in the Raspi
* Add documentation with Sphinx

Configuration
=============
This projects uses `Python Poetry <https://https://python-poetry.org/>`_ for package managment.
The development environment is controlled with `Mini-Conda <https://anaconda.org/anaconda/conda/>`_.

To build with docker, go to the project directory and execute: ``docker build -t 'rpi-center:fa' .``.
Then, to run it you use: ``docker run --rm --name rpi-center-fa1 -p 8000:8000 -it rpi-center:f``.

To build with docker-compose you can execute: ``docker-compose build`` and ``docker-compose up`` in the project's directory.

License
=======

Apache License, Version 2.0, January 2004.

Version
=======

.. role:: bash(code)
   :language: bash

* :bash:`V. 0.1.8`

Extra Links
===========
* `Python Poetry's dependencies specification <https://python-poetry.org/docs/dependency-specification/>`_

