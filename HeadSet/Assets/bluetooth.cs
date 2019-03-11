using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using InTheHand.Networking.Sockets;
using InTheHand.Devices.Bluetooth;
using InTheHand.Devices.Enumeration;
using InTheHand.Devices.Bluetooth.Rfcomm;
using System.IO;
using System.Threading;
using UnityEngine;
using System.Collections;

public class bluetooth_module
{
    public string readBytes(Stream stream, int numBytes)
    {
        string ret = "";

        for (int i = 0; i < numBytes; ++i)
        {
            char current_char = (char)stream.ReadByte();
            if (current_char != '\n' && current_char != '\r' && current_char != '\t')
            {
                ret += current_char;
            }
            else
            {
                numBytes++;
            }
        }
        return ret;

    }

    public void writeBytes(string str, Stream stream)
    {
        foreach (var i in str)
        {
            stream.WriteByte((byte)i);
        }
    }

    public void connectBlueTooth(string Name, Semaphore semaphore, Queue<string> inputQueue)
    {
        var devices = DeviceInformation.FindAll(RfcommDeviceService.GetDeviceSelector(RfcommServiceId.SerialPort));
        Console.WriteLine(devices);
        int index = 0;
        Console.WriteLine("Devices Read");
        for (int i = 0; i < devices.Count; ++i)
        {
            Console.WriteLine(devices[i].Name);
            if (devices[i].Name == Name)
            {
                index = i;
                break;
            }
        }
        Console.WriteLine("Done reading");
        var deviceInfo = devices[index]; // this makes some assumptions about your paired devices so really the results should be enumerated and checked for the correct device
        var device = BluetoothDevice.FromDeviceInformation(deviceInfo);
        var serResults = device.GetRfcommServices(BluetoothCacheMode.Cached);
        RfcommDeviceService serv = serResults.Services[0];
        Stream stream = null;
        while (true)
        {
            try
            {
                stream = serv.OpenStream();
                break;
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
            }
        }

        while (true)
        {
            writeBytes("{\"id\":\"getdata\"}", stream);
            //stream.Write(Encoding.ASCII.GetBytes("{id:getdata}"),0,0);
            //Console.WriteLine("Wrote getData");
            string length = readBytes(stream, 4);
            stream.Flush();
            string input = readBytes(stream, Int16.Parse(length));
            stream.Flush();
            semaphore.WaitOne();
            inputQueue.Enqueue(input);
            semaphore.Release();
        }
    }
}

public class bluetooth : MonoBehaviour {
    public String BluetoothReading;
   
    Semaphore semaphore = new Semaphore(1, 1);
    Queue<string> inputQueue = new Queue<string>();
    // Use this for initialization
    void Awake () {
        
        Thread T1 = new Thread(delegate ()
        {
            bluetooth_module temp = new bluetooth_module();
            temp.connectBlueTooth("HC-06", semaphore, inputQueue);
        });
        T1.Start();
        StartCoroutine(change());

    }
    
    IEnumerator change()
    {
        while (true)
        {
            while (true)
            {
                semaphore.WaitOne();
                if (inputQueue.Count == 0)
                {
                    semaphore.Release();
                    yield return new WaitForSeconds(3.0f);
                    break;
                }
                string ret = inputQueue.Dequeue();
                //Debug.Log(ret);
                if (Globals.Instance.BluetoothData == null)
                {
                    Globals.Instance.BluetoothData = new CustomJSON();
                }
                Globals.Instance.BluetoothData = JsonUtility.FromJson<CustomJSON>(ret);
            
                semaphore.Release();
                yield return new WaitForSeconds(3.0f);
            }
        }
    }
}
