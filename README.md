# arduino-status-signal
Show the hackerspace status (open/close) with industrial skookum lamps and an ESP8266.

## How it works
The Wemos D1 does this:

1. HTTP GET request on https://fixme.ch/status.json
2. Parse the Json
3. Check if the space is open or close
4. Change the lamp accordingly
5. Repeat every 5 seconds
