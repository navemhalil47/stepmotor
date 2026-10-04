# Double Clap Switch Presser

An Arduino project that listens for a **double clap** and, when it detects one, uses a stepper motor to press a switch or button and then return to its starting position.

## How It Works

1. The sound sensor is sampled continuously. In each cycle the program records the **peak value** over a 30 ms window.
2. If the peak exceeds the threshold, the **first clap** is registered and a timer starts.
3. If a **second clap** is detected within the allowed window (and not too soon after the first one), the motor is triggered.
4. The motor rotates forward to press the switch, waits, then rotates back to its original position.
5. The system enters a short cooldown period and then resets, ready for the next double clap.
6. If no second clap arrives in time, the counter resets automatically.

```
Clap 1 --(300 ms to 1200 ms)--> Clap 2 --> Motor presses switch --> Cooldown --> Ready
           \__ too late: reset _/
```

## Hardware

| Component | Notes |
|---|---|
| Arduino board | Uno / Nano or compatible |
| Stepper motor | 28BYJ-48 (2048 steps per revolution in this configuration) |
| Motor driver | ULN2003 driver board |
| Sound sensor | Analog output sound module (e.g. KY-038 or similar) |
| Power supply | External 5 V recommended for the motor |

### Wiring

| Component | Arduino Pin |
|---|---|
| Driver IN1 | 8 |
| Driver IN2 | 9 |
| Driver IN3 | 10 |
| Driver IN4 | 11 |
| Sound sensor (analog out) | A0 |
| Sensor VCC / GND | 5 V / GND |

> The stepper is initialized as `Stepper(2048, 8, 10, 9, 11)`. This pin order is required for the 28BYJ-48 to rotate correctly with the Stepper library, so keep the wiring consistent with the code.

## Software Requirements

- [Arduino IDE](https://www.arduino.cc/en/software)
- `Stepper` library (included with the Arduino IDE by default)

## Installation

1. Connect the hardware as described above.
2. Open the sketch in the Arduino IDE.
3. Select your board and port from the **Tools** menu.
4. Upload the sketch.
5. Open the Serial Monitor at **9600 baud** to see live feedback.

## Configuration

All settings are at the top of the sketch:

| Variable | Default | Description |
|---|---|---|
| `tamTurAdim` | 2048 | Steps per full revolution of the motor |
| `ESIK_DEGERI` | 80 | Sound threshold (0 to 1023). Lower = more sensitive |
| `CIFT_ALKIS_PENCERESI` | 1200 ms | Maximum time between the first and second clap |
| `ALKIS_ARASI_MIN_SURE` | 300 ms | Minimum gap between claps (prevents echo from counting as a clap) |
| `MOTOR_BEKLEME` | 500 ms | Cooldown after the motor runs |
| `stepMotor.setSpeed()` | 7 RPM | Motor speed |
| `step(256)` | 256 steps | Rotation amount (about 45 degrees) |

### Calibration Tips

- If the system triggers on random noise, **increase** `ESIK_DEGERI`.
- If claps are not detected, **decrease** `ESIK_DEGERI` or adjust the sensor's potentiometer.
- If the echo of the first clap is counted as the second one, **increase** `ALKIS_ARASI_MIN_SURE`.
- To change how far the motor presses, adjust the value in `step(256)` and `step(-256)` (they must be equal and opposite so the motor returns to its start position).

## Serial Monitor Output

```
Çift Alkış Sistemi Hazır! 1.2sn içinde 2 alkış bekleniyor...
1. ALKIŞ YAKALANDI! (Peak: 112) -> 2. Alkış bekleniyor...
2. ALKIŞ YAKALANDI! (Peak: 97) ---> MOTOR TETİKLENİYOR!
İşlem tamamlandı, sistem soğuma modunda...
```

If the second clap does not arrive in time:

```
-> Zaman aşımı! 2. alkış gelmedi, sıfırlandı.
```

## Limitations

- The motor movement uses blocking calls (`step()` and `delay()`), so claps are not detected while the motor is moving.
- Detection is based only on loudness, so any loud noise can act as a clap.
- Accuracy depends on the sensor's sensitivity and the room's background noise.

## Possible Improvements

- Add a status LED for clap detection feedback
- Use non-blocking motor control (e.g. the `AccelStepper` library)
- Add a triple-clap mode for a second action
- Make the threshold adjustable at runtime via a potentiometer

## License

This project is open source. Feel free to use and modify it.
