using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PopulatePanelData : MonoBehaviour {
    // Use this for initialization
    void Awake () {
        Globals.Instance.menuPanels = GameObject.FindGameObjectsWithTag("MenuPanel");
    }
}
