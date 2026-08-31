# ESP8266 + MAX7219CNG + 220801K (8021A) — Wiring Diagram

Final, working wiring for a NodeMCU-style ESP8266 driving a 2-digit
common-cathode 7-segment display (Hadex K288Q / 220801K, schematic
variant 8021A) through a bare MAX7219CNG driver chip.

## Components

- ESP8266 dev board (NodeMCU-style, pins labeled D0–D8)
- MAX7219CNG (24-pin DIP)
- 220801K / 8021A 2-digit 7-segment display, common cathode
- 1x resistor, 10–22 kΩ (sets segment brightness via ISET)
- 1x capacitor, 10 µF+ electrolytic (decoupling — a 4700 µF/16V works fine too)
- Optional: 100 nF ceramic capacitor (extra high-frequency decoupling)

## 1. ESP8266 → MAX7219CNG

| MAX7219 pin | Function | ESP8266 pin | GPIO |
|---|---|---|---|
| 1 | DIN | D7 | GPIO13 |
| 13 | CLK | D5 | GPIO14 |
| 12 | LOAD/CS | D4 | GPIO2 |
| 4, 9 | GND | GND | — |
| 19 | V+ | VIN (5V) | — |
| 18 | ISET | via 10–22 kΩ resistor → V+ | — |

![ESP8266 to MAX7219CNG wiring](images/220801k.jpg)

## 2. MAX7219CNG → Display (220801K / 8021A)

Display pins as seen from the **front** (digits facing you, decimal
points pointing down):
- Top row, left→right: 10, 9, 8, 7, 6
- Bottom row, left→right: 1, 2, 3, 4, 5

| Display pin | Function | MAX7219 pin |
|---|---|---|
| 1 | COM1 (left digit, common cathode) | 2 (DIG0) |
| 2 | COM2 (right digit, common cathode) | 11 (DIG1) |
| 3 | Segment G | 17 (SEG G) |
| 4 | Segment F | 15 (SEG F) |
| 5 | Segment E | 21 (SEG E) |
| 6 | Segment D | 23 (SEG D) |
| 7 | Segment C | 20 (SEG C) |
| 8 | Segment B | 16 (SEG B) |
| 9 | Segment A | 14 (SEG A) |
| 10 | DP (decimal point) | 22 (SEG DP) |

![MAX7219CNG to 220801K display wiring](images/max7219.jpg)