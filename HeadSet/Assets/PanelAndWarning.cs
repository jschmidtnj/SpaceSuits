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
        while (true)
        {
            while (Globals.Instance.menuPanels == null)
            {
                yield return new WaitForSeconds(1.0f);
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
                    switch (gloveInt)
                    {
                        case 1:
                            currentPanel += 1;
                            currentPanel %= 4;
                            Globals.Instance.currentPanel = currentPanel;
                            Debug.Log("Current Panel " + currentPanel);
                            foreach (GameObject panel in Globals.Instance.menuPanels)
                            {
                                panel.gameObject.SetActive(false);
                            }
                            Globals.Instance.menuPanels[currentPanel].SetActive(true);
                            break;
                        case 2:
                            if (currentPanel == 2)
                            {
                                Globals.Instance.currentTask += 1;
                            }
                            break;
                        case 4:
                            Globals.Instance.warningFlag = 1;
                            break;
                    }
                }
                yield return new WaitForSeconds(1.0f);
            }
        }
    }

}

