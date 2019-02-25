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

public class bluetooth
{
    public string readBytes(Stream stream, int numBytes)
    {
        string ret = "";
        for (int i = 0; i < numBytes; ++i)
        {
            ret += (char)stream.ReadByte();
        }
        return ret;

    }
    public void connectBlueTooth(string Name, Semaphore semaphore, Queue<string> inputQueue)
    {
        var devices = DeviceInformation.FindAll(RfcommDeviceService.GetDeviceSelector(RfcommServiceId.SerialPort));
        Console.WriteLine(devices);
        int index = 0;
        Debug.Log("Devices Read");
        for (int i = 0; i < devices.Count; ++i)
        {
            Debug.Log(devices[i].Name);
            if (devices[i].Name == Name)
            {
                index = i;
                break;
            }
        }
        Debug.Log("Done reading");
        var deviceInfo = devices[index]; // this makes some assumptions about your paired devices so really the results should be enumerated and checked for the correct device
        var device = BluetoothDevice.FromDeviceInformation(deviceInfo);
        var serResults = device.GetRfcommServices(BluetoothCacheMode.Cached);
        RfcommDeviceService serv = serResults.Services[0];
        var stream = serv.OpenStream();
        while (true)
        {
            stream.WriteByte((byte)'R');
            string length = readBytes(stream, 4);
            string input = readBytes(stream, Int32.Parse(length));
            semaphore.WaitOne();
            inputQueue.Enqueue(input);
            semaphore.Release();
        }
    }
}

public class bluetooth_sample : MonoBehaviour {
    public String BluetoothReading;
   
    Semaphore semaphore = new Semaphore(1, 1);
    Queue<string> inputQueue = new Queue<string>();
    // Use this for initialization
    void Start () {
        
        Thread T1 = new Thread(delegate ()
        {
            bluetooth temp = new bluetooth();
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
                Debug.Log(ret);
                Globals.Instance.BluetoothData = JsonUtility.FromJson<CustomJSON>(ret);
                semaphore.Release();
                yield return new WaitForSeconds(3.0f);
            }
        }
    }
}
