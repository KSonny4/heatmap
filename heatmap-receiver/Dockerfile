FROM arm32v7/python:2.7-slim-buster

COPY qemu-arm-static /usr/bin
COPY . /heatmap-receiver
WORKDIR /heatmap-receiver

RUN apt-get update \
    && apt-get install build-essential python-dev python-pip gcc git scons swig libzbar0 -y \
    && python -m pip install spidev==3.4 RPi.GPIO==0.7.0 --user \
    && git clone git://github.com/adafruit/adafruit-beaglebone-io-python.git \
    && cd adafruit-beaglebone-io-python \
    && git reset --hard b867bbffe39c1ae0ce25c31781f8f2da5152fce6 \
    && sed -i -e "s/-Werror', '-Wextra/-Wextra/g" setup.py \
    && python setup.py install \
    && apt-get remove -y file gcc git pkg-config make build-essential python-dev python-pip scons swig libzbar0 \
    && apt-get clean \
    && apt-get autoremove -y \
    && cd /heatmap-receiver \
    && rm -rf Dockerfile __pycache__/ adafruit-beaglebone-io-python/ /usr/bin/qemu-arm-static

ENTRYPOINT ["python", "receiver.py"]