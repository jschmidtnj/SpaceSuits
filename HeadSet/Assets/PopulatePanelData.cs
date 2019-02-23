using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PopulatePanelData{
    // Use this for initialization
    void Start () {
        Globals.menuPanels = GameObject.FindGameObjectsWithTag("MenuPanel");
    }
}
