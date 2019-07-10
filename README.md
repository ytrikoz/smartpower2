# SmartPower2
Firmware for PSU based on ESP2866 [SmartPower2](https://wiki.odroid.com/accessory/power_supply_battery/smartpower2) 
## OTA firmware update
After uploading your should restart device manualy. Only after the restart, your device will be updated.
## CLI

### Serial, Telnet
To start cli you should send `Line Feed`, usually to do this press `ENTER` or `CTRL + J` keys. The cli command prompt should appear on the screen.
To exit from cli press `ESC` twice.

###  Commands: 
 - **help**
to list available commands, they are: `print`, `rm`, `show`, `set`, `get`, `system`
- **print** *file*
to print contents of the file, for example: `print /cfg/settings.ini`
- **rm** *file*
to delete file 
 - **show** *item*
where item is `ip`, `clients`, `power`, `network`, `info`, `config`, `status`, `uptime`
 - **set** *parameter* *value* 
to change configuration parameter with name `wifi` to `1` , your should type `set wifi 1`
 - **get** *parameter*
prints config parameter value
- **system** *action* *[parameter]* 
	`save` with parameter `config`, will store your runtime config to flash memory. 
	`load` with parameter `config`, to reload early stored config 
	`reset` with parameter `config`, to reset the config with "factory defaults"
	`restart` to restart device with optional parameter delay in seconds. 
	`power` with parameter value `on` and `off`, switches output power

to setup the connection to your existing wifi network
`config ssid` SSID
`config passwd` Password
`config dhcp 1`

to setup smartpower2 ap mode
`config ap_ssid` SmartPower2
`config ap_passwd` 12345678
`config ap_ipaddr 192.168.4.1

then use `config wifi 1` (sta) or `config wifi 2` (ap) to select mode, and last
`system save config`
`system restart`



