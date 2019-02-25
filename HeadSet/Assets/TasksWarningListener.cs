using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TaskWarningListener : MonoBehaviour
{
    public string BluetoothReading;

    // Use this for initialization
    void Start()
    {
        StartCoroutine(updates());
    }

    IEnumerator updates()
    {
        // Tasks
        if (BluetoothReading.Contains("warning:3"))
        {
            foreach (GameObject panel in Globals.Instance.menuPanels)
            {
                panel.gameObject.SetActive(false);
            }
            this.gameObject.SetActive(true);
        }
        yield return new WaitForSeconds(2.0f);
    }
}
