using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class BatterySlider : MonoBehaviour {

    public Slider batteryBar;

	public void ChangeBatteryBar(int curVal, int maxVal)
    {
        batteryBar.maxValue = maxVal;
        batteryBar.value = curVal;
    }
}
