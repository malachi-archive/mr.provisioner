using Android.App;
using Android.Widget;
using Android.OS;

namespace client.android
{
    [Activity(Label = "Mr. Provisioner", MainLauncher = true, Icon = "@drawable/icon")]
    public class MainActivity : Activity
    {
        // Semi-singleton
        public static MainActivity Singleton { get; private set; }

        protected override void OnCreate(Bundle bundle)
        {
            base.OnCreate(bundle);

            Singleton = this;

            // Set our view from the "main" layout resource
            SetContentView (Resource.Layout.Main);

            var button = FindViewById<Button>(Resource.Id.button1);
            var list = FindViewById<ListView>(Resource.Id.lstCandidates);

            //list.Adapter

            button.Click += (s, e) =>
            {
                var d = new Discoverer(ApplicationContext);

                d.CandidateWiFiDiscovered += scanResults =>
                {
                    list.Adapter = new CandidateAdapter(this, scanResults);
                };

                //list.Adapter = new CandidateAdapter(d.
            };
        }
    }
}

