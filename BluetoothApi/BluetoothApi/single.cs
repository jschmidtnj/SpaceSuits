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
        public void connectBlueTooth(string UUID, Semaphore semaphore, Queue<int> inputQueue)
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
                int input = peerStream.ReadByte();
                if (input > 0)
                {
                    semaphore.WaitOne();
                    inputQueue.Enqueue(input);
                    semaphore.Release();
                }

            }
        }
        static void Main(string[] args)
        {
            Semaphore semaphore = new Semaphore(1, 1);
            Queue<int> inputQueue = new Queue<int>();
            Thread T1 = new Thread(delegate()
            {
                Program temp = new Program();
                temp.connectBlueTooth("20:16:12:12:75:82", semaphore, inputQueue);
            });

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
