using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PanelAndWarning : MonoBehaviour {

    private void Start()
    {
        StartCoroutine(updatePanels());
    }

    IEnumerator updatePanels()
    {
        while (Globals.Instance.menuPanels == null)
        {
            yield return new WaitForEndOfFrame();
        }
        foreach (GameObject panel in Globals.Instance.menuPanels)
        {
            panel.gameObject.SetActive(false);
        }
        int currentPanel = Globals.Instance.currentPanel;
        Globals.Instance.menuPanels[currentPanel].gameObject.SetActive(true);
        // Glove int check
        while (true)
        {
            if (Globals.Instance.BluetoothData != null)
            {
                int gloveInt = Globals.Instance.BluetoothData.glove;
                if (gloveInt == 1 && Globals.Instance.swap == 0) {
                    currentPanel += 1;
                    currentPanel %= 3;
                    Globals.Instance.currentPanel = currentPanel;
                    foreach (GameObject panel in Globals.Instance.menuPanels)
                    {
                        panel.gameObject.SetActive(false);
                    }
                    Globals.Instance.menuPanels[currentPanel].SetActive(true);
                    Globals.Instance.BluetoothData.glove = 0;
                    Globals.Instance.swap = 1;
                }
                else if (gloveInt == 2 && Globals.Instance.swap == 0) {

                    if (currentPanel == 2)
                    {
                        Globals.Instance.currentTask++;
                    }
                    Globals.Instance.swap = 1;
                    Globals.Instance.BluetoothData.glove = 0;
                }
                else if (gloveInt == 4 && Globals.Instance.swap == 0) { 
                    Globals.Instance.warningFlag = 1;
                    Globals.Instance.swap = 1;
                    Globals.Instance.BluetoothData.glove = 0;
                }
                
            }
            yield return new WaitForSeconds(.25f);
        }
        
    }

}

