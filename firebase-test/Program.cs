using System;
using Firebase.Database;
using Firebase.Database.Query;
using Json.net;

namespace SpaceSuits
{
    class Program
    {
        public void LoadJson(String configfile)
        {
            using (StreamReader r = new StreamReader("config.json"))
            {
                string json = r.ReadToEnd();
                dynamic array = JsonConvert.DeserializeObject(json);
                foreach (var item in array)
                {
                    Console.WriteLine("{0} {1}", item.temp, item.vcc);
                }
            }
        }
        static void Main(string[] args)
        {
            Console.WriteLine("Hello World!");
            var config = LoadJson("config.json");
            var url = config.DatabaseUrl;
            var secret = config.DatabaseAppSecret;
            var firebase = new FirebaseClient(
              url,
              new FirebaseOptions
              {
                  AuthTokenAsyncFactory = () => Task.FromResult(secret)
              });
            var observable = firebase
            .Child("dinosaurs")
            .AsObservable<Dinosaur>()
            .Subscribe(d => Console.WriteLine(d.Key));
            //Console.WriteLine("Hello World!");
        }
    }
}
