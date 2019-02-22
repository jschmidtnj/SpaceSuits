using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SuitStatusListener : MonoBehaviour {
    public string BluetoothReading;
    public GameObject[] menuPanels;
    // Use this for initialization
    void Start () {
        menuPanels = GameObject.FindGameObjectsWithTag("MenuPanel");
        StartCoroutine(updates());
	}
	
    IEnumerator updates() {
        if (BluetoothReading.Contains("warning:1"))
        {
            foreach(GameObject panel in menuPanels)
            {
                panel.gameObject.SetActive(false);
          
            }
            this.gameObject.SetActive(true);
        }
        yield return new WaitForSeconds(2.0f);
    }
}
