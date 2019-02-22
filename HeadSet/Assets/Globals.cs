using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CustomJSON
{
    int warning;
    string[] tasks;
    public class inertialData
    {
        float x;
        float y;
        float z;
    }
    inertialData iData;
    int glove;
    public class suitTelem
    {
        int pressure;
        int O21;
        int O22;
        int battery;
        int heartrate;
    }
    suitTelem sTelem;
}

public static class Globals {
    public static CustomJSON BluetoothData;
}
