using System;
using Firebase.Database;
using Firebase.Database.Query;
using Json;
using System.IO;
using Newtonsoft.Json;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace SuitData
{
    class Program
    {

        public static Dictionary<string, string> LoadJson(string configfile)
        {
            Dictionary<string, string> items;
            using (StreamReader r = new StreamReader("config.json"))
            {
                string json = r.ReadToEnd();
                //Console.WriteLine(json);
                items = JsonConvert.DeserializeObject<Dictionary<string, string>>(json);
            }
            return items;
        }

        static void Main(string[] args)
        {
            var config = LoadJson("config.json");
            var url = config["DatabaseUrl"];
            var secret = config["DatabaseAppSecret"];
            var firebase = new FirebaseClient(
              url,
              new FirebaseOptions
              {
                  AuthTokenAsyncFactory = () => Task.FromResult(secret)
              });
            //suitdata:
            var suitdata = firebase
            .Child("suitdata")
            .AsObservable<string>()
            .Subscribe(d =>
            {
                Console.WriteLine(d.Key);
                Console.WriteLine(d.Object);
            });
            Console.ReadKey();
        }
    }
}
