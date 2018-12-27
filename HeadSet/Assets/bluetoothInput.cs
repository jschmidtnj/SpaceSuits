using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;
using System.IO.Ports;
using System.Threading;
using InTheHand.Net.Sockets;
using InTheHand.Net.Bluetooth;
using InTheHand.Net;
using System;

public class bluetoothInput : MonoBehaviour {
    Semaphore semaphore = new Semaphore(1,1);
    Queue<int> queue = new Queue<int>();

    void readBluetooth(string UUID)
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
            int ret = peerStream.ReadByte();
            if (ret > 0)
            {
                semaphore.WaitOne();
                queue.Enqueue(ret);
                semaphore.Release();
            }
        }
    }
    // Use this for initialization
    void Start () {
        Thread T = new Thread(delegate ()
        {
            readBluetooth("20:16:12:12:75:82");
        });
    }

    // Update is called once per frame
    void Update () {
		
	}
}
