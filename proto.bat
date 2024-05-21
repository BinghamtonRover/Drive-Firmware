@echo off
python Protobuf\generate_arduino.py -s Drive -p drive
python Protobuf\generate_arduino.py -s Drive -p version
python Protobuf\generate_arduino.py -s Drive -p status
python Protobuf\generate_arduino.py -s Drive -p utils
