using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Android.App;
using Android.Content;
using Android.OS;
using Android.Runtime;
using Android.Views;
using Android.Widget;

using Android.Net.Wifi;
using Android;
using Android.Content.PM;
using Android.Util;
using System.Net.NetworkInformation;
using Com.AugustCellars.CoAP.Net;
using Com.AugustCellars.CoAP.DTLS;
using Com.AugustCellars.COSE;
using PeterO.Cbor;
using Com.AugustCellars.CoAP;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace client.android
{
    // Looks for open AP with a particular IP address
    public class Discoverer : BroadcastReceiver
    {
        readonly WifiManager wifiManager;
        Context context;

        const string TAG = nameof(Discoverer);

        public Discoverer(Context context)
        {
            // TODO: As per
            // https://stackoverflow.com/questions/21686033/wifimanager-startscan-not-returning-any-results-need-some-guidance-please
            // Be sure WiFi is enabled
            wifiManager = (WifiManager)context.GetSystemService(Context.WifiService);
            Permission p = context.CheckSelfPermission(Manifest.Permission.ChangeWifiState);

            // According to
            //   https://developer.xamarin.com/api/namespace/Android.Net.Wifi/ and
            //   https://stackoverflow.com/questions/32151603/scan-results-available-action-return-empty-list-in-android-6-0
            //context.CheckSelfPermission("android.hardware.wifi");

            this.context = context;
        }


        public void Discover()
        {
            var intentFilter = new IntentFilter(WifiManager.ScanResultsAvailableAction);
            context.RegisterReceiver(this, intentFilter);
            wifiManager.StartScan();
        }

        public override void OnReceive(Context context, Intent intent)
        {
            foreach (ScanResult scanResult in wifiManager.ScanResults)
            {
                Log.Debug(TAG, $"Found ssid: {scanResult.Ssid} with capabilities {scanResult.Capabilities}");
            }

            context.UnregisterReceiver(this);

            DiscoverOurNodes();
        }

        protected void CoapConnect(string host)
        {
            //var ep = new CoAPEndPoint();

            Request request = Request.NewGet();

            request.URI = new Uri($"coap://{host}/.well-known/core");

            //ep.Start();

            request.Send();
            Response response = request.WaitForResponse(2000);

            if (response == null)
            {
                Log.Info(TAG, "COAP timeout");
                return;
            }

            Log.Info(TAG, $"COAP responded: format = {response.ContentType}");
            Log.Info(TAG, $"COAP responded: payload = {response.PayloadString}");
        }

        protected void CoapsConnect(string host)
        {
            // https://github.com/malachi-iot/CoAP-CSharp/blob/master/CoAP.Example/CoAP.Client/ExampleClient.cs
            // https://github.com/malachi-iot/CoAP-CSharp/blob/master/CoAP.Test/DTLS/DTLSClientEndPoint.cs
            //OneKey authKey = new OneKey();

            //CoAPEndPoint ep = new DTLSClientEndPoint(authKey);

            Log.Info(TAG, $"Attempting CoAP connection to {host}");

            var ep = new DTLSClientEndPoint(null);

            var req = new Request(Method.GET)
            {
                URI = new Uri($"coaps://{host}:5682/.well-known/core"),
                EndPoint = ep
            };

            ep.Start();

            req.Send();
            req.WaitForResponse(5000);

            //CBORObject.FromO
        }


        async protected Task<string> IdentifyHost()
        {
            // Lifting from https://msdn.microsoft.com/en-us/library/system.net.networkinformation.ping(v=vs.110).aspx
            // TODO: Do mDNS stuff also
            var ping = new Ping();
            var pingOptions = new PingOptions();
            string hostName = "176.16.0.1";

            pingOptions.DontFragment = true;

            var data = Enumerable.Repeat<byte>(65, 32).ToArray();

            var reply = await ping.SendPingAsync(hostName, 3000, data, pingOptions);

            return reply.Status == IPStatus.Success ? hostName : null;
        }

        static bool IsOpenWifi(ScanResult scanResult)
        {
            // lifted  and adapted from https://stackoverflow.com/questions/378415/how-do-i-extract-text-that-lies-between-parentheses-round-brackets
            // and here https://stackoverflow.com/questions/740642/c-sharp-regex-split-everything-inside-square-brackets
            const string matchPattern = @"\[(.*?)\]"; // old = @"\[([^\]]*)\]"
            MatchCollection matches = Regex.Matches(scanResult.Capabilities, matchPattern);

            Log.Debug(TAG, $"Capability review ssid: {scanResult.Ssid} with capabilities {scanResult.Capabilities}");

            foreach (Match match in matches)
            {
                var capability = match.Groups[1].Value.ToUpper();

                Log.Debug(TAG, $"capability: {capability}");

                // adapted from https://stackoverflow.com/questions/25662761/how-to-differentiate-open-and-secure-wifi-networks-without-connecting-to-it-in-a
                if (capability.Contains("WEP") ||
                   capability.Contains("WPA"))
                {
                    return false;
                }
            }

            return true;
        }


        public event Action<IList<CandidateRecord>> CandidateWiFiDiscovered;


        protected void DiscoverOurNodes()
        {
            var candidates = wifiManager.ScanResults.Where(IsOpenWifi).
                Select(scanResult => new CandidateRecord
                {
                    ScanResult = scanResult
                }).ToArray();

            // FIX: this is 100% the wrong place for this, but doing it
            // here just to keep the ball rolling
            CandidateWiFiDiscovered?.Invoke(candidates);

            Task.Run(async () => await DiscoverOurNodes(candidates));
        }

        void Switch(ScanResult scanResult)
        {
            // DHCP doesn't seem to be doling out when doing this
            // So either I'm not joining or I a not waiting long enough
            // it *was* working, as I saw a successful ping before
            // Not sure what's different
            // Manually connecting to ESP AP does dole out an AP

            var alreadyConfigured = wifiManager.ConfiguredNetworks.FirstOrDefault(x => x.Ssid == scanResult.Ssid);
            int netId;

            if (alreadyConfigured != null)
            {
                netId = alreadyConfigured.NetworkId;
            }
            else
            {
                // As per https://stackoverflow.com/questions/8818290/how-do-i-connect-to-a-specific-wi-fi-network-in-android-programmatically
                // For now we only handle open APs but a little more factory-ish and we could handle protected ones also
                var newConf = new WifiConfiguration();
                newConf.Ssid = scanResult.Ssid;
                netId = wifiManager.AddNetwork(newConf);
            }

            wifiManager.Disconnect();
            wifiManager.EnableNetwork(netId, true);
            wifiManager.Reconnect();
        }


        async protected Task DiscoverOurNodes(IEnumerable<CandidateRecord> candidates)
        {
            WifiInfo conn = wifiManager.ConnectionInfo;

            wifiManager.DisableNetwork(conn.NetworkId);

#if DEBUG_DIALOG
            var d = new AlertDialog.Builder(MainActivity.Singleton);

            d.SetTitle("Got here");
            d.SetPositiveButton("OK",
                delegate
                {
                    Log.Info(TAG, $"Finished dialog: Old conn = {conn}");
                    if (conn != null)
                        wifiManager.EnableNetwork(conn.NetworkId, true);
                });
            d.Show();
#endif
            //Toast.MakeText(Application.Context, "TEST", )

            foreach (var candidate in candidates)
            {
                var scanResult = candidate.ScanResult;

                candidate.Status = "Inspecting";

                Log.Info(TAG, $"Inspecting ssid: {scanResult.Ssid}");

                await Task.Run(() => Switch(scanResult));
                //Switch(scanResult);

                candidate.Status = "Ping host";

                string host = await IdentifyHost();

                Log.Info(TAG, $"Candidate host: {host}");

                if (host != null)
                {
                    candidate.Status = $"Foud: {host}";

                    CoapConnect(host);

                    candidate.Status = $"Connected";
                }
                else
                {
                    candidate.Status = "No host";
                }
            }

#if !DEBUG_DIALOG
            if (conn != null)
                wifiManager.EnableNetwork(conn.NetworkId, true);

#endif
        }
    }
}