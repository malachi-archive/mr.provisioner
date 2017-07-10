using System;
using System.Collections.Generic;
using Android.App;
using Android.Net.Wifi;
using Android.Views;
using Android.Widget;

namespace client.android
{
    public class CandidateRecord
    {
        public ScanResult ScanResult { get; set; }
        public string Host { get; set; }
        public bool Provisionable { get; set; }
        public bool Provisioned { get; set; }
    }

    /// <summary>
    /// Interacts with Discover class to reflect progress of discovery
    /// </summary>
    public class CandidateAdapter : BaseAdapter<CandidateRecord>
    {
        readonly IList<ScanResult> scanResults;
        readonly Activity activity;

        public CandidateAdapter(Activity activity, IList<ScanResult> scanResults)
        {
            this.activity = activity;
            this.scanResults = scanResults;
        }

        public override CandidateRecord this[int position]
        {
            get
            {
                return new CandidateRecord()
                {
                    ScanResult = scanResults[position]
                };
            }
        }

        public override int Count => scanResults.Count;

        public override long GetItemId(int position)
        {
            // TODO: Might want to extract network id, if it's configured
            return position;
        }

        // Guidelines from https://developer.xamarin.com/recipes/android/data/adapters/create_a_custom_adapter_for_contacts/
        public override View GetView(int position, View convertView, ViewGroup parent)
        {
            var view = convertView ?? activity.LayoutInflater.Inflate(
                Resource.Layout.CandidateListItem, parent, false);

            var lblSSID = view.FindViewById<TextView>(Resource.Id.lblSSID);

            lblSSID.Text = scanResults[position].Ssid;

            return view;
        }
    }
}
