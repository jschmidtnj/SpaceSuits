using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TasksWarningListener : MonoBehaviour
{

    // Use this for initialization
    void Awake()
    {
        StartCoroutine(updates());
    }

    IEnumerator updates()
    {
        while (true)
        {
            // Not checking glove int here bc it might change later
            if (Globals.Instance.BluetoothData != null && Globals.Instance.BluetoothData.warning == 3)
            {
                foreach (GameObject panel in Globals.Instance.menuPanels)
                {
                    panel.gameObject.SetActive(false);
                }
                this.gameObject.SetActive(true);
            }
            // Glove int check
            if (Globals.Instance.BluetoothData != null)
            {
                int gloveInt = Globals.Instance.BluetoothData.glove;
                switch (gloveInt)
                {
                    case 3:
                        foreach (GameObject panel in Globals.Instance.menuPanels)
                        {
                            panel.gameObject.SetActive(false);
                        }
                        this.gameObject.SetActive(true);
                        break;
                }
            }
            yield return new WaitForSeconds(.5f);
        }
    }
}
