const coap = require('coap')

var req = coap.request({
	host: '224.0.1.186',
    pathname: '/.well-known/core',
	multicast: true
	, multicastTimeout: 2000
});

req.on('response', res =>
{
    // Returns back 0.0.0.0 presumably because of multicast behaviors
    // TODO: Get actual IP so that we can eventually truly discover our coap pals
    //console.log('response host: ', res.outSocket.address);
    console.log('response code: ', res.code);

    // spit payload out
    res.pipe(process.stdout)
});

req.end();
