// pushes dummy SSID name down to device

const coap = require('coap')
var _addr = process.env.ESP_IP;

if(_addr)
    addr = "coap://" + _addr + "/";
else
{
    _addr = '192.168.2.12';
    // TODO: Make this an error instead, so that we always specify ESP_IP
    addr = "coap://192.168.2.12/";
}

// +++ optional, just discovering it
// this code works well, just disabling it to purify test
/* 
var req = coap.request(addr + ".well-known/core");

req.on('response', res =>
{
    console.log('response code: ', res.code);
    res.pipe(process.stdout)
});

req.end(); */
// ---

// Need to do this explicit form, because
// as far as I can see there's no req.method = 'PUT' which works
var coapConnection = {
    host: _addr,
    pathname: '/ssid/name',
    method: 'PUT',
    confirmable: true
};

var req = coap.request(coapConnection);

// This is pushing down (and works) properly to ESP8266, but it appears response code
// confuses things.  Getting error:
// Error: No matching format found
// while processing input message
req.write('DUMMY_SSID');
req.on('response', res =>
{
    console.log('response code: ', res.code);
    res.pipe(process.stdout);
});

req.end();

