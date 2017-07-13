# mr.provisioner

Secure CoAP based WiFi provisioning suite.

Since many of our beloved embedded devices have such limited constraints, we approach provisioning in a very special way:

* If no WiFi is discovered, enter provisioning mode
* Then and only then, CoAPS is enabled
* WiFi credentials are shuttled down over aforementioned CoAP/DTLS link
* Device is RESET, if necessary to free up buffers which are most likely fully used or too fragmented
* Device should now be provisioned and fresh
