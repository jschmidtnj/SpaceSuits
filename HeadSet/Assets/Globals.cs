using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public class taskJSON 
{
    public int majorkey;
    public int subkey;
    public string data;
    public string time;
}
[System.Serializable]
public class inertialData
{
    public float accelx;
    public float accely;
    public float accelz;
    public float roll;
    public float yaw;
    public float pitch;
}

/*
public class suitTelem
{
    public int pressure;
    public int O21;
    public int O22;
    public int battery;
    public int heartrate;
}*/

public class CustomJSON 
{
    public int warning;
    public int glove;
    public inertialData inertialState;
    public taskJSON[] tasks;
    //public suitTelem sTelem;
}

public class Globals : MonoBehaviour
{
    public CustomJSON BluetoothData = null;
    public GameObject[] menuPanels;
    public int currentPanel = 1;
    public int currentTask = 1;
    public int warningFlag = 0;

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
