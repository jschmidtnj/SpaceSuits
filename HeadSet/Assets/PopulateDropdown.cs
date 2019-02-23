using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class PopulateDropdown : MonoBehaviour {
    public Dropdown mainDropdown;
    public List<string> dropOptions;

    // Use this for initialization
    void Start () {
        mainDropdown = GetComponent<Dropdown>();
        // On update do things?
        mainDropdown.ClearOptions();
        StartCoroutine(updateTasks());
	}
	
	IEnumerator updateTasks()
    {
        foreach (taskJSON task in Globals.BluetoothData.tasks) {
            dropOptions.Add(task.message);
        }
        mainDropdown.AddOptions(dropOptions);
        yield return new WaitForSeconds(2.0f);
    }
}
