using System;
using Firebase.Database;
using Firebase.Database.Query;
using Json;
using System.IO;
using Newtonsoft.Json;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace FirebaseConnect
{
    class Program
    {

        public static Dictionary<string, string> LoadJson(string configfile)
        {
            Dictionary<string, string> items;
            using (StreamReader r = new StreamReader(configfile))
            {
                string json = r.ReadToEnd();
                //Console.WriteLine(json);
                items = JsonConvert.DeserializeObject<Dictionary<string, string>>(json);
            }
            return items;
        }

        public class TaskObj
        {
            public string data { get; set; }
        }

        static void Main(string[] args)
        {
            var currentdirectory = System.IO.Directory.GetCurrentDirectory();
            Console.WriteLine(currentdirectory);
            var config = LoadJson(currentdirectory + "/config.json");
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
                //Console.WriteLine(d.Key);
                //Console.WriteLine(d.Object);
                switch (d.Key)
                {
                    case "accelx":
                        //update accelx
                        Console.WriteLine("update accelx: " + d.Object);
                        break;
                    case "accely":
                        //update accely
                        Console.WriteLine("update accely: " + d.Object);
                        break;
                    case "accelz":
                        //update accelz
                        Console.WriteLine("update accelz: " + d.Object);
                        break;
                    case "battery":
                        //update battery
                        Console.WriteLine("update battery: " + d.Object);
                        break;
                    case "heartrate":
                        //update heartrate
                        Console.WriteLine("update heartrate: " + d.Object);
                        break;
                    case "moisture":
                        //update moisture
                        Console.WriteLine("update moisture: " + d.Object);
                        break;
                    case "pitch":
                        //update pitch
                        Console.WriteLine("update pitch: " + d.Object);
                        break;
                    case "primaryo2":
                        //update primaryo2
                        Console.WriteLine("update primaryo2: " + d.Object);
                        break;
                    case "roll":
                        //update roll
                        Console.WriteLine("update roll: " + d.Object);
                        break;
                    case "secondaryo2":
                        //update secondaryo2
                        Console.WriteLine("update secondaryo2: " + d.Object);
                        break;
                    case "yaw":
                        //update yaw
                        Console.WriteLine("update yaw: " + d.Object);
                        break;
                }

            });
            //tasks:
            dynamic taskdata = new Newtonsoft.Json.Linq.JObject();
            var gettasks = firebase
            .Child("tasks")
            .AsObservable<string>()
            .Subscribe(tasks =>
            {
                //Console.WriteLine(tasks.Key);
                //Console.WriteLine(tasks.Object);
                if (tasks.Key == "data")
                {
                    Console.WriteLine("update tasks: ");
                    taskdata = Newtonsoft.Json.Linq.JObject.Parse(tasks.Object);
                    Console.WriteLine(taskdata);
                }
            });


            Console.ReadKey();
        }
    }
}
