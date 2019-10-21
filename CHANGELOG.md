
# Change Log

## 0.6.8 (2019-10-20)

### send magic packet

`wol -ip` *x.x.x.x* `-mac` *xx:xx:xx:xx:xx:xx*

## 0.6.7 (2019-10-04)

## Serial / Telnet console connection

To start you should send `Line Feed`, usually to do this press `ENTER` key. The command prompt should appear on the screen , when done to exit from shell press `ESC` twice (and/or close telnet connection).

### available commands

`help`

### control lcd backlight

to operate

`system backlight` *value*

value '0' - to switch off (with any other will switch it back).
or disable backlight at config with:

`set backlight 0`

`config apply`

### diagnostic logs/data

`show` [`btn, clock, http, lcd, led, netsvc, ntp, psu, shell, telnet, update`]

### print file content

`print` *file name*

### delete file

`rm` *file name*

### configuration

`set` *param* *value*

not persisten without `config save` or `config apply`

`get` *param*

prints runtime value of parameter with name *param*

### control output power switch

`power` *action*

available actions are `on` and `off`

## blue wifi led

- **off** wifi switched off / network down
- **on** network up
- **blink** web/telnet client connected

## red power led

- **off** no input power/boot error
- **on** ready/output power off
- **blink** output power on
- **blink fast** alert/error
