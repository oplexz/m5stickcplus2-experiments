pio := "uv run platformio"

[private]
default:
    @just --list

# build firmware
build:
    {{pio}} run

# flash firmware to device
push:
    {{pio}} run --target upload

# open serial monitor
monitor:
    {{pio}} device monitor

# flash and immediately open serial monitor
push-monitor:
    {{pio}} run --target upload --target monitor

# clean build artifacts
clean:
    {{pio}} run --target clean
