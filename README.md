# SmartPower2

![Build](https://github.com/ytrikoz/smartpower2/workflows/Build/badge.svg)

Firmware for PSU based on ESP2866 [SmartPower2](https://wiki.odroid.com/accessory/power_supply_battery/smartpower2)

## Serial or Telnet terminal connection

To start you should send `Line Feed`, usually to do this press `ENTER`. The command prompt should appear on the screen , when done to exit from cli press `ESC` twice.

### Commands

- **help**
list available commands
- **print** *file*
print contents of the file, for example: `print /etc/main`
- **rm** *file*
delete file 
- **show** *item*
where item is `ip`, `clients`, `power`, `network`, `info`, `config`, `status`, `uptime`
- **set** *parameter* *value* 
to change configuration parameter with name `wifi` to `1` , your should type `set wifi 1`
- **get** *parameter*
prints config parameter value
- **power** *action*
`on` and `off`, switches output power

## OTA firmware update

After uploading your should restart device manualy (using cli or hardware button). Only after restart, your device will be updated. 

## See wiki
