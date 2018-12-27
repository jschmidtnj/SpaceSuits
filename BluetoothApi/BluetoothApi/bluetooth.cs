using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using InTheHand.Net.Sockets;
using InTheHand.Windows;
using InTheHand.Net;
using InTheHand.Net.Bluetooth;
using System.IO;
using System.Threading;

namespace BluetoothApi
{
    class Program
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
        static void Main(string[] args)
        {
            Semaphore semaphore = new Semaphore(1, 1);
            Queue<string> inputQueue = new Queue<string>();
            Thread T1 = new Thread(delegate ()
            {
                Program temp = new Program();
                temp.connectBlueTooth("20:16:12:12:75:82", semaphore, inputQueue);
            });
            Thread T2 = new Thread(delegate ()
            {
                Program temp = new Program();
                temp.connectBlueTooth("98:d3:c1:fd:2f:1d", semaphore, inputQueue);
            });
            T1.Start();
            T2.Start();

            while (true)
            {
                semaphore.WaitOne();
                if (inputQueue.Count != 0)
                {
                    Console.WriteLine(inputQueue.Dequeue());
                }
                semaphore.Release();
            }
        }
    }
}
