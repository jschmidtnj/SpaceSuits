using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using InTheHand.Net.Sockets;
using InTheHand.Net;
using InTheHand.Net.Bluetooth;
using System.IO;
using System.Threading;
using System;
using UnityEngine.UI;

public class bluetooth
{
    public void connectBlueTooth(string UUID, Semaphore semaphore, Queue<string> inputQueue)
    {
        BluetoothAddress address = BluetoothAddress.Parse(UUID);
        BluetoothDeviceInfo info = new BluetoothDeviceInfo(address);
        Guid serviceClass = BluetoothService.SerialPort;
        var endPoint = new BluetoothEndPoint(address, serviceClass);
        var bc = new BluetoothClient();
        bc.Connect(endPoint);
        Stream peerStream = bc.GetStream();
        while (true)
        {
            peerStream.WriteByte((byte)('1'));
            int receive = peerStream.ReadByte();
            if (receive != -1)
            {
                char input = (char)receive;
                int val = peerStream.ReadByte();
                semaphore.WaitOne();
                inputQueue.Enqueue(input + " " + val);
                semaphore.Release();
            }
        }
    }
}

public class multipleBluetooth : MonoBehaviour {
    UnityEngine.UI.Text text;
   
    Semaphore semaphore = new Semaphore(1, 1);
    Queue<string> inputQueue = new Queue<string>();
    // Use this for initialization
    void Start () {
        
        Thread T1 = new Thread(delegate ()
        {
            bluetooth temp = new bluetooth();
            temp.connectBlueTooth("20:16:12:12:75:82", semaphore, inputQueue);
        });
        Thread T2 = new Thread(delegate ()
        {
            bluetooth temp = new bluetooth();
            temp.connectBlueTooth("98:d3:c1:fd:2f:1d", semaphore, inputQueue);
        });
        T1.Start();
        T2.Start();
        StartCoroutine("change");
    }
    
    IEnumerator change()
    {
        while (true) {
            semaphore.WaitOne();
            if (inputQueue.Count == 0)
            {
                semaphore.Release();
                yield return null;
            }
            string ret = inputQueue.Dequeue();
            text.text = ret;
            semaphore.Release();
            yield return null;
        }
    }
}
