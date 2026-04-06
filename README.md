# SainSmart solder fume extractor modification

This project is about replacing the default AC fan in the [SainSmart solder fume extractor](https://www.sainsmart.com/products/sainsmart-solder-smoke-absorber-remover-fume-extractor-for-soldering) with a DC fan like [Noctua NF-A14](https://www.noctua.at/en/products/nf-a14-industrialppc-3000-pwm).

## Why?

- the built-in fan is way too loud
- it runs at constant speed only
- the power cable is way too stiff

## Features

- toggle switch to power the device on or off integrated in the housing
- dual LED to indicate power on and standby
- MOSFET switch to turn the fan on or off using MCU
- pot with a large knob 🫠 to select the fan speed

## Thoughts

### Electrical

In fact, the Noctua fan draws around 2A during the initial turn. I decided to use a logic-level MOSFET to switch the fan on. Adding a cap between 10uF and 100uF increases the Miller effect, forcing the fan to start up slowly and reducing the initial current as well. But in this case, an NPN transistor is needed to ground the PWM signal against the N-MOSFET drain.

I'm unsure whether a DC fan requires a flyback diode. I decided to skip it for now.

### Mechanical

The JST connectors are intentionally placed on the edges, which puts mechanical stress on the board. There is certainly a better solution for connector placement.

## How it looks like

![](Assets/Front.jpg)
![](Assets/Back.jpg)
![](Assets/FanMounting.jpg)
![](Assets/PcbMounting.jpg)
