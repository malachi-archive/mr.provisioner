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

namespace client.android
{
    // Looks for open AP with a particular IP address
    public class Discoverer : BroadcastReceiver
    {
        readonly WifiManager wifiManager;

        public Discoverer(Context context)
        {
            wifiManager = (WifiManager) context.GetSystemService(Context.WifiService);
            wifiManager.StartScan();
            var intentFilter = new IntentFilter(WifiManager.ScanResultsAvailableAction);
            context.RegisterReceiver(this, intentFilter);

            // According to
            //   https://developer.xamarin.com/api/namespace/Android.Net.Wifi/ and
            //   https://stackoverflow.com/questions/32151603/scan-results-available-action-return-empty-list-in-android-6-0
            context.CheckSelfPermission("android.hardware.wifi");
        }

        public override void OnReceive(Context context, Intent intent)
        {
            throw new NotImplementedException();
        }
    }
}