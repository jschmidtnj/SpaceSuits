using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SuitStatusListener : MonoBehaviour {
    public string BluetoothReading;
 
    // Use this for initialization
    void Start () {
        StartCoroutine(updates());
	}
	
    IEnumerator updates() {
        // SuitStatus
        if (BluetoothReading.Contains("warning:1"))
        {
            foreach(GameObject panel in Globals.menuPanels)
            {
                panel.gameObject.SetActive(false);
            }
            this.gameObject.SetActive(true);
        }
        yield return new WaitForSeconds(2.0f);
    }
}
