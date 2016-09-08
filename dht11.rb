#!/usr/bin/ruby

require 'dht-sensor-ffi'
require 'rpi_gpio'

PIN = 7
DHT_MODEL = 11 # 11 or 22

RPi::GPIO.set_numbering :board
RPi::GPIO.set_warnings(false)
RPi::GPIO.setup PIN, :as => :output

loop do
  val = DhtSensor.read(PIN, DHT_MODEL)
  system("sudo /home/pi/display #{val.temp}")
  if val.temp > 33
    RPi::GPIO.set_high PIN
  else
    RPi::GPIO.set_low PIN
  sleep 3
  end
end
