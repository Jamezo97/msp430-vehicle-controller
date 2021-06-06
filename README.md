# Vehicle Controller

This repository contains code, and other relevant artifacts for an interface unit between my vehicle, and my stereo head-unit.

Based on relevant CAN messages, the system shall:
- Turn on/off a 12V signal to dim the head-unit lighting
- Turn on/off a 12V signal to power on/off the head-unit
- Vary a resistance to trigger multimedia controls

## Reasoning
My vehicle, although old (2004), heavily relies on the use of a CAN bus to transmit information around the vehicle. There is no

- 12V accessory line
- 12V dimmer line
- Multimedia control line

The original head-unit which came with the vehicle seems to retrieve all of this information via the CAN bus.

The purchased head unit does not support CAN, and thus requires physical signals to perform the above described functionality. The plan is to develop a module to perform this translation.

## Components

I'm using an [MSP430 G2553](https://www.ti.com/product/MSP430G2553) ultra low power microcontroller.

I'm using an MCP2515 CAN control module, via SPI.

I have a [Sony WX-900BT](https://www.sony.com.au/electronics/support/mobile-cd-players-digital-media-players-wx-series/wx-900bt)

I have a `Holden Astra AH CD 2004` vehicle.