using Android.App;
using Android.Widget;
using Android.OS;

namespace client.android
{
    [Activity(Label = "client.android", MainLauncher = true, Icon = "@drawable/icon")]
    public class MainActivity : Activity
    {
        protected override void OnCreate(Bundle bundle)
        {
            base.OnCreate(bundle);

            // Set our view from the "main" layout resource
            SetContentView (Resource.Layout.Main);

            var button = FindViewById<Button>(Resource.Id.button1);

            button.Click += (s, e) =>
            {
                var d = new Discoverer(ApplicationContext);
            };
        }
    }
}

