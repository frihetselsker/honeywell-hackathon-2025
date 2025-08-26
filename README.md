# FutureShapers 2025

## Sensors

This section contains the following list of sensors controlled by Arduino Uno A:

- Gas sensor (pin: A0)
- Noise sensor (pin: A1)
- DHT11 sensor, i.e. temperature + humidity sensor (pin: 4)
- Servo motor, aka "window" (pin: 5)
- DC motor, aka "cooler" (pin: 6)
- Buzzer for playing music (pin: TBD)

*more sensors to be added in the near future*

## Controller

This section contains the following list of sensors controlled by Arduino Uno B:

- RGB LED for visual notifications (pin: TBD)
- Pinpad for entering the password (pin: TBD)
- Buzzer for acoustic notifications (pin: TBD)
- Potentiometer for ??? (pin: TDB)
- Button for ??? (pin: TBD)
- LCD Display for outputing information (pin: TBD)

## Communication
Host and sensors can 'communicate' using `SoftwareSerial` at baud rate 9600 as the most reliable rate.

Using this channel boards can talk and send information/commands.

