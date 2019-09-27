# !/usr/bin/python
# -*- coding: utf-8 -*-
from nrf24 import NRF24
import time
import logging
import json

logger = logging.getLogger()
logging.basicConfig(level=logging.INFO)

ADDRESS = [ord("1"), ord("t"), ord("r"), ord("a"), ord("n")]
CHANNEL = 109
BUS = 0
DEVICE = 0
CE = 25
CSN = 0
FILE_TO_WRITE = "/static_data.js"


class App:
    def __init__(self):
        logger.info("Setting up...\n")
        self.radio = NRF24()
        self.radio.begin(BUS, DEVICE, CE, CSN)
        self.radio.setRetries(15, 15)

        self.radio.setAutoAck(1)
        self.radio.enableAckPayload()
        self.radio.enableDynamicPayloads()

        self.radio.setChannel(CHANNEL)
        self.radio.setDataRate(NRF24.BR_250KBPS)
        self.radio.setPALevel(NRF24.PA_MAX)

        self.radio.openReadingPipe(1, ADDRESS)
        self.radio.startListening()

        self.radio.printDetails()

    def message_generator(self):
        while True:
            pipe = [0]
            while not self.radio.available(pipe):
                time.sleep(0.0001)

            recv_buffer = []
            self.radio.read(recv_buffer)
            logger.debug("%s %s" % (pipe, recv_buffer))

            if 0 in recv_buffer:
                recv_buffer = recv_buffer[:recv_buffer.index(0)]
            yield pipe[0], "".join(map(lambda x: chr(x), recv_buffer))

    def run(self):
        logger.info("\nReceiver started!\n")

        data = {}
        for clientId, message in self.message_generator():
            try:
                logger.debug("Message from client #%s: %s" % (clientId, message))

                temperature, humidity, co2 = map(float, message.split())
                logger.info("Received data from #%s: temp %s°C, hum %s%%, CO2 %s" % (clientId, temperature, humidity, co2))

                with open(FILE_TO_WRITE, "w") as f:
                    data[clientId] = {
                        "id": clientId,
                        "temperature": temperature,
                        "humidity": humidity,
                        "co2": co2
                    }

                    f.write("var data = %s;\n" % json.dumps(data.values()))

            except Exception as e:
                logger.exception(e)


if __name__ == "__main__":
    app = App()
    app.run()
