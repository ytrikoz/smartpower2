
# Change Log

## 0.6.9 (2019-10-27)

### Crash reports

`crash list`

`crash test`

`crash print`

### Time on lcd display

### Total consumption

(/etc/main) parameter 'store_wh' (needs `config apply`)

To change, use the admin web interface or command  `set store_wh` *value*

Where *value*:

**0** - Off; Will reset counter every power on.

**1** - On; To save the counter between turning the device on and off (stored once a minute in / var / wh)

## 0.6.8 (2019-10-23)

### Boot Up power state

(/etc/main) parameter 'bootpwr' (needs `config apply`)

To change, use the admin web interface or command `set bootpwr` *value*

Where *value*:

**0** - Off

**1** - On

**2** - Last state (uninterruptible power during reboots and restoration of last state after switching on)

### Restart(reboot) device

`restart` *delay*

Where the *delay* time before the device reboots (in seconds)

### Send magic packet

`wol -ip` *x.x.x.x* `-mac` *xx:xx:xx:xx:xx:xx*

## 0.6.7 (2019-10-04)

## Serial / Telnet console connection

To start you should send `Line Feed`, usually to do this press `ENTER` key. The command prompt should appear on the screen , when done to exit from shell press `ESC` twice (and/or close telnet connection).

### available commands

`help`

### Backlight control

To operate the device lcd backlight use command `system backlight` *value*

Where *value*:

**0** - Off

**1** - On

(/etc/main) parameter 'backlight' (needs `config apply`)

to disable backlight:

`set backlight 0`

`config apply`

### diagnostic logs/data

`show` [`btn, clock, http, lcd, led, netsvc, ntp, psu, shell, telnet, update`]

### print file

`print` *file*

Path and file name to print, ex: /etc/main.

### delete file

`rm` *file*

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
