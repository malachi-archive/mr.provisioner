﻿using System;
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
            p = context.CheckSelfPermission(Manifest.Permission.AccessWifiState);

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

        // Attempting to use this https://android.googlesource.com/platform/frameworks/base/+/gingerbread/wifi/java/android/net/wifi/WifiStateTracker.java
        // to track DHCP updates, but no dice
        public class DhcpRenewReceiver : BroadcastReceiver
        {
            public static readonly int DHCP_RENEW = 0;
            public static readonly string ACTION_DHCP_RENEW = "android.net.wifi.DHCP_RENEW";

            public override void OnReceive(Context context, Intent intent)
            {
                Log.Info(TAG, "DHCP RENEW");
            }


            public DhcpRenewReceiver(Context context)
            {
                var intent = new Intent(ACTION_DHCP_RENEW);
                //var intentFilter = new IntentFilter(ACTION_DHCP_RENEW);
                var _intent = PendingIntent.GetBroadcast(context, DHCP_RENEW, intent, PendingIntentFlags.OneShot);
                context.RegisterReceiver(this, new IntentFilter(ACTION_DHCP_RENEW));
            }
        }

        public override void OnReceive(Context context, Intent intent)
        {
            var renewer = new DhcpRenewReceiver(context);

            foreach (ScanResult scanResult in wifiManager.ScanResults)
            {
                Log.Debug(TAG, $"Found ssid: {scanResult.Ssid} with capabilities {scanResult.Capabilities}");
            }

            context.UnregisterReceiver(this);

            DiscoverOurNodes();
        }

        // FIX: Something odd
        // This works first time, then never works again until app is restarted...
        // It's UDP so pretty darned connectionless... and I can see everything seems to recycle on ESP
        // (and anyways ESP always responds after app is restarted)
        protected bool CoapConnect(string host)
        {
            //var ep = new CoAPEndPoint();

            Request request = Request.NewGet();
            //Request request = new Request(Method.GET); // Doesn't change behavior 

            request.URI = new Uri($"coap://{host}/.well-known/core");

            //ep.Start();

            request.Send();
            Response response = request.WaitForResponse(3000);

            if (response == null)
            {
                Log.Info(TAG, "COAP timeout");
                return false;
            }

            Log.Info(TAG, $"COAP responded: format = {response.ContentType}");
            Log.Info(TAG, $"COAP responded: payload = {response.PayloadString}");

            return true;
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
            string hostName = "172.16.0.1";

            pingOptions.DontFragment = true;

            var data = Enumerable.Repeat<byte>(65, 32).ToArray();

            try
            {
                var reply = await ping.SendPingAsync(hostName, 3000, data, pingOptions);

                return reply.Status == IPStatus.Success ? hostName : null;
            }
            catch(Exception e)
            {
                Log.Error(TAG, "Error: " +e);
                return null;
            }
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

            CandidateWiFiDiscovered?.Invoke(candidates);

            Task.Run(async () => await DiscoverOurNodes(candidates));
        }

        void Switch(ScanResult scanResult)
        {
            //MainActivity.Singleton.RunOnUiThread(() =>
            //{
                // DHCP doesn't seem to be doling out when doing this
                // So either I'm not joining or I a not waiting long enough
                // it *was* working, as I saw a successful ping before
                // Not sure what's different
                // Manually connecting to ESP AP does dole out an AP

                var alreadyConfigured = wifiManager.ConfiguredNetworks.FirstOrDefault(x => x.Ssid == scanResult.Ssid);
                int netId;

                // NOTE: Be careful with this, make sure we only rip out open unsecured network
                // NOTE: We might not actually need to rip anything out anyway...
                if (alreadyConfigured != null)
                {
                    netId = alreadyConfigured.NetworkId;
                    wifiManager.RemoveNetwork(netId);
                }
                //else
                {
                    // As per https://stackoverflow.com/questions/8818290/how-do-i-connect-to-a-specific-wi-fi-network-in-android-programmatically
                    // For now we only handle open APs but a little more factory-ish and we could handle protected ones also
                    var newConf = new WifiConfiguration();
                    newConf.Ssid = scanResult.Ssid;
                    // OK thinking we need to do this but it's deprecated and doesn't work
                    //newConf.AllowedKeyManagement.
                    newConf.AllowedKeyManagement.Set((int)KeyManagementType.None);
                    netId = wifiManager.AddNetwork(newConf);
                }

                wifiManager.Disconnect();
                wifiManager.EnableNetwork(netId, true);
                wifiManager.Reconnect();
            //});
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

                // TODO: link into DhcpStateMachine and get dhcp asap
                // instead of waiting around like this
                await Task.Delay(1500);
                //Switch(scanResult);

                var newConn = wifiManager.ConnectionInfo;

#if UNUSED
                var dhcpInfo = wifiManager.DhcpInfo;

                int _ip = dhcpInfo.IpAddress;
                string __ip = Android.Text.Format.Formatter.FormatIpAddress(_ip);

#endif
                // https://forums.xamarin.com/discussion/2260/get-current-ip-address
                // https://stackoverflow.com/questions/16730711/get-my-wifi-ip-address-android
                //Java.Net.InetAddress.GetByAddress()
                string ip = Android.Text.Format.Formatter.FormatIpAddress(newConn.IpAddress);

                candidate.Status = $"Our IP: {ip}";

                //candidate.Status = "Ping host";

                string host = await IdentifyHost();

                //Log.Info(TAG, $"Candidate host: {host}");

                if (host != null)
                {
                    candidate.Status = $"Found: {host}";

                    var success = await Task.Run(() => CoapConnect(host));

                    candidate.Status = success ? "Connected" : "No COAP endpoint";
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