# ESP8266 specific target

Interesting tidbit:
http://www.esp8266.com/viewtopic.php?f=6&t=10624
clues on how esp8266 remembers credentials
http://bbs.espressif.com/viewtopic.php?t=574
how to reset the device
http://bbs.espressif.com/viewtopic.php?t=777
get reset reason
http://bbs.espressif.com/viewtopic.php?t=103
Good wifi setup/breakdown info overall

wifi_station_get_ap_info to pull all cached AP info structs
if it's 0, we know to enter provisioning mode immediately
My understanding is it is max 5 based on:
http://www.esp8266.com/viewtopic.php?f=6&t=3150