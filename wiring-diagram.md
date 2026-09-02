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

| Second display pin | Function | MAX7219 pin | Note |
|---|---|---|---|
| 1 | COM1 (left digit) | 3 (DIG4) | new wire |
| 2 | COM2 (right digit) | 10 (DIG5) | new wire |
| 3 | Segment G | 17 (SEG G) | parallel to 1st display |
| 4 | Segment F | 15 (SEG F) | parallel to 1st display |
| 5 | Segment E | 21 (SEG E) | parallel to 1st display |
| 6 | Segment D | 23 (SEG D) | parallel to 1st display |
| 7 | Segment C | 20 (SEG C) | parallel to 1st display |
| 8 | Segment B | 16 (SEG B) | parallel to 1st display |
| 9 | Segment A | 14 (SEG A) | parallel to 1st display |
| 10 | DP (decimal point) | 22 (SEG DP) | parallel to 1st display |

![MAX7219CNG to 220801K display wiring](images/max7219.jpg)

## 4. MAX7219CNG #1 → MAX7219CNG #2 (cascade)

| MAX7219 #1 pin | Function | → MAX7219 #2 pin |
|---|---|---|
| 24 (DOUT) | serial data out | 1 (DIN) |
| 13 (CLK) | clock | 13 (CLK) — parallel, same wire from ESP8266 |
| 12 (LOAD/CS) | select | 12 (LOAD/CS) — parallel, same wire from ESP8266 |
| 19 (V+) | power | 19 (V+) — same power rail |
| 4, 9 (GND) | ground | 4, 9 (GND) — same ground |
| — | ISET resistor/pot | pin 18 (ISET) of chip #2 → own 10 kΩ resistor (+ optional 100 kΩ pot in series) → pin 19 (V+) of chip #2 |
| — | decoupling capacitor | between pin 19 (V+) and pins 4/9 (GND) of chip #2, as close to its body as possible — its own capacitor, not shared with chip #1 |

## 5. MAX7219CNG #2 → Display A (first SH5461AS / K288G, digits 1–4)

| Display A pin | Function | MAX7219 #2 pin |
|---|---|---|
| 12 | DIG1 (1st digit) | 2 (DIG0) |
| 9 | DIG2 (2nd digit) | 11 (DIG1) |
| 8 | DIG3 (3rd digit) | 6 (DIG2) |
| 6 | DIG4 (4th digit) | 7 (DIG3) |
| 11 | Segment A | 14 (SEG A) |
| 7 | Segment B | 16 (SEG B) |
| 4 | Segment C | 20 (SEG C) |
| 2 | Segment D | 23 (SEG D) |
| 1 | Segment E | 21 (SEG E) |
| 10 | Segment F | 15 (SEG F) |
| 5 | Segment G | 17 (SEG G) |
| 3 | DP (decimal point) | 22 (SEG DP) |

## 6. MAX7219CNG #2 → Display B (second SH5461AS / K288G, digits 5–8)

| Display B pin | Function | MAX7219 #2 pin |
|---|---|---|
| 12 | DIG1 (5th digit) | 3 (DIG4) |
| 9 | DIG2 (6th digit) | 10 (DIG5) |
| 8 | DIG3 (7th digit) | 5 (DIG6) |
| 6 | DIG4 (8th digit) | 8 (DIG7) |
| 11 | Segment A | 14 (SEG A) — shared with display A |
| 7 | Segment B | 16 (SEG B) — shared with display A |
| 4 | Segment C | 20 (SEG C) — shared with display A |
| 2 | Segment D | 23 (SEG D) — shared with display A |
| 1 | Segment E | 21 (SEG E) — shared with display A |
| 10 | Segment F | 15 (SEG F) — shared with display A |
| 5 | Segment G | 17 (SEG G) — shared with display A |
| 3 | DP (decimal point) | 22 (SEG DP) — shared with display A |
