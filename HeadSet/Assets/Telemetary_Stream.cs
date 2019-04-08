using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Networking;
using UnityEngine.UI;


public class Telemetary_Stream : MonoBehaviour {
    public int warning_flag = 0;
    static Color Green = new Color(0f, 0.6980392f, .08930377f, 1f);
    static Color Red = new Color(0.6980392f, 0f, 0.02009222f, 255f);
    static Color Orange = new Color(0.6980392f, 0.4546653f, 0f, 255f);
    public class suitTelem
    {
        public string createDate;
        public int heart_bpm;
        public double p_sub;
        public int t_sub;
        public int v_fan;
        public int p_o2;
        public double rate_02;
        public double cap_battery;
        public int p_h2o_g;
        public int p_h2o_l;
        public int p_sop;
        public double rate_sop;
        public string t_battery;
        public string t_oxygen;
        public string t_water;
    }

  
    suitTelem input = new suitTelem();
    // Use this for initialization
    void Start () {
        StartCoroutine(Get_Request("http://18.234.179.222:3000/api/suit/recent"));
        StartCoroutine(Start_Interp());
    }

    IEnumerator Get_Request(string uri)
    {
        while (true)
        {
            using (UnityWebRequest webRequest = UnityWebRequest.Get(uri))
            {
                // Request and wait for the desired page.
                yield return webRequest.SendWebRequest();
                
                if (webRequest.isNetworkError)
                {
                    Debug.Log("Error Reading \n");
                }
                else
                {
         
                    int length = webRequest.downloadHandler.text.Length;
                    string obj = webRequest.downloadHandler.text.Substring(1, length - 2);
                    //Debug.Log(obj);
                    input = JsonUtility.FromJson<suitTelem>(obj);
                    Debug.Log(obj);
                }
            }
            yield return new WaitForSeconds(10.0f);
        }
    }


    public void changeColorTime(int first, int second, Text text)
    {
        string[] time = text.text.Split(':');
        int hours = Int32.Parse(time[0]);
        if (hours > first)
        {
            text.color = Green;
        }
        else if (hours > second)
        {
            text.color = Orange;
        }
        else
        {
            text.color = Red;
            warning_flag = 1;
        }
    }

    public void changeColorRange(double first,double second, Text text)
    {
        double comp = Int32.Parse(text.text);
        if (comp > first)
        {
            text.color = Green;
        }
        else if (comp > second)
        {
            text.color = Orange;
        }
        else
        {
            text.color = Red;
            warning_flag = 1;
        }
    }
    // use suit telem input to make changes to the scene
    IEnumerator Start_Interp()
    {
        suitTelem previous = new suitTelem();
        while (true)
        {
            if (previous.heart_bpm != input.heart_bpm)
            {
                Text heart = GameObject.Find("HeartRate").GetComponent<Text>();
                heart.text = input.heart_bpm.ToString();
                previous.heart_bpm = input.heart_bpm;
            }
            if (previous.t_oxygen != input.t_oxygen)
            {
                Text oxygen = GameObject.Find("OxygenTime").GetComponent<Text>();
                oxygen.text = input.t_oxygen.ToString();
                previous.t_oxygen = input.t_oxygen;
                changeColorTime(3, 2, oxygen);
            }
            if (previous.t_battery != input.t_battery)
            {
                Text battery = GameObject.Find("BatteryLife").GetComponent<Text>();
                battery.text = input.t_battery.ToString();
                previous.t_battery = input.t_battery;
                changeColorTime(3, 2, battery);
            }
            if (previous.t_water != input.t_water)
            {
                Text water = GameObject.Find("WaterLife").GetComponent<Text>();
                water.text = input.t_water.ToString();
                previous.t_water = input.t_water;
                changeColorTime(3, 2, water);
            }
            if (previous.p_sub != input.p_sub)
            {
                Text sub_pressure = GameObject.Find("SubPressure").GetComponent<Text>();
              
                sub_pressure.text = input.p_sub.ToString();
                if (sub_pressure.text.Length > 7)
                {
                    sub_pressure.text = input.p_sub.ToString().Substring(0, 7);
                }
                previous.p_sub = input.p_sub;
                if (previous.p_sub >= 2 && previous.p_sub <= 4)
                {
                    sub_pressure.color = Green;
                }
                else
                {
                    warning_flag = 1;
                    sub_pressure.color = Red;
                }
            }
            if (previous.t_sub != input.t_sub)
            {
                Text temperature = GameObject.Find("SubTemperature").GetComponent<Text>();
                temperature.text = input.t_sub.ToString();
                previous.t_sub = input.t_sub;
                if (previous.t_sub >= 60 && previous.t_sub <= 90)
                {
                    temperature.color = Green;
                }
                else
                {
                    temperature.color = Red;
                    warning_flag = 1;
                }
            }
            if (previous.p_o2 != input.p_o2)
            {
                Text o2 = GameObject.Find("02Pressure").GetComponent<Text>();
                o2.text = input.p_o2.ToString();
                previous.p_o2 = input.p_o2;
                if (previous.p_o2 >= 750 && previous.p_o2 <= 950)
                {
                    o2.color = Green;
                }
                else
                {
                    warning_flag = 1; 
                    o2.color = Red;
                }
            }
            if (previous.rate_02 != input.rate_02)
            {
                Text o2 = GameObject.Find("O2Rate").GetComponent<Text>();
                o2.text = input.rate_02.ToString();
                previous.rate_02 = input.rate_02;
                if (previous.rate_02 >= .5 && previous.rate_02 <= 1)
                {
                    o2.color = Green;
                }
                else
                {
                    o2.color = Red;
                    warning_flag = 1;
                }
            }
            if (previous.cap_battery != input.cap_battery)
            {
                Text battery = GameObject.Find("BatteryCapacity").GetComponent<Text>();
                battery.text = input.cap_battery.ToString();
                previous.cap_battery = input.cap_battery;
                changeColorRange(5, 2, battery);
            }
            if (previous.p_h2o_g != input.p_h2o_g)
            {
                Text gas = GameObject.Find("H20GasPressure").GetComponent<Text>();
                gas.text = input.p_h2o_g.ToString();
                previous.p_h2o_g = input.p_h2o_g;
                if (previous.p_h2o_g >= 14 && previous.p_h2o_g <= 16)
                {
                    gas.color = Green;
                }
                else
                {
                    gas.color = Red;
                    warning_flag = 1;
                }
            }
            if (previous.p_h2o_l != input.p_h2o_l)
            {
                Text h20 = GameObject.Find("H20LiquidPressure").GetComponent<Text>();
                h20.text = input.p_h2o_l.ToString();
                previous.p_h2o_l = input.p_h2o_l;
                if (previous.p_h2o_l >= 14 && previous.p_h2o_l <= 16)
                {
                    h20.color = Green;
                }
                else
                {
                    h20.color = Red;
                    warning_flag = 1; 
                }
            }
            if (previous.p_sop != input.p_sop)
            {
                Text o2 = GameObject.Find("Secondary02Pressure").GetComponent<Text>();
                o2.text = input.p_sop.ToString();
                previous.p_sop = input.p_sop;
                if (previous.p_sop >= 750 && previous.p_sop <= 950)
                {
                    o2.color = Green;
                }
                else
                {
                    o2.color = Red;
                    warning_flag = 1;
                }
            }
            if (previous.rate_sop != input.rate_sop)
            {
                Text o2 = GameObject.Find("S02Rate").GetComponent<Text>();
                o2.text = input.rate_sop.ToString();
                if (o2.text.Length > 7)
                {
                    o2.text = o2.text.Substring(0, 7);
                }
                previous.rate_sop = input.rate_sop;
                if (previous.rate_sop >= .5 && previous.rate_sop <= 1.0)
                {
                    o2.color = Green;
                }
                else
                {
                    o2.color = Red;
                    warning_flag = 1;
                }
            }
            yield return new WaitForSeconds(5.0f);
        }
    }
}
