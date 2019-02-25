using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class taskJSON 
{
    public string id;
    public string message;
    public string time;
}

public class inertialData
{
    public float x;
    public float y;
    public float z;
}

public class suitTelem
{
    public int pressure;
    public int O21;
    public int O22;
    public int battery;
    public int heartrate;
}

public class CustomJSON 
{
    public int warning;
    public taskJSON[] tasks;
    public inertialData iData;
    public int glove;
    public suitTelem sTelem;
}

public class Globals : MonoBehaviour
{
    public CustomJSON BluetoothData;
    public GameObject[] menuPanels;

    public static Globals Instance
    {
        get;
        private set;
    }

    void Awake()
    {
        Instance = this;
    }
}
