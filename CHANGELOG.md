
# Change Log

## 0.6.7 (2019-10-04)

### print diagnostic logs/data to console

shell command `show` with parameter `btn, clock, http, lcd, led, netsvc, ntp, psu, shell, telnet, update`

example:
> 02:32:12> show psu
> power: off
> output: 5.00
> 02:32:14> show netsvc
> network up for 1726128ms
> active: true
> dns: true
> mdns: true
> netbios: true

### blue wifi led

- **off** wifi switched off / network down
- **on** network up
- **blink** web/telnet client connected

### red power led

- **off** no input power/boot error
- **on** ready/output power off
- **blink** output power on
- **blink fast** alert/error

## 0.6.7 (2019-09-17)

### Feature: added

- **clock** initial state of the device clock is firmware build time
- **shell**  `system backlight` [0,1]
- **config**  `backlight`
- **shell**  `show clock`
- **shell**  `show ntp`
- **lcd** visual improvment
