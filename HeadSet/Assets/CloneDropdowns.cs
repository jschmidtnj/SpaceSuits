using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class CloneDropdowns : MonoBehaviour {
    public Dropdown emptyDD;
    Dictionary<int, Dropdown> taskToDropdown =
        new Dictionary<int, Dropdown>();
    taskJSON[] JSONList;

	// Use this for initialization
	void Awake () {
        emptyDD = GetComponent<Dropdown>();
        // shh go to sleep
        emptyDD.Hide();
        StartCoroutine(maintainTasks());
	}
	
	IEnumerator maintainTasks()
    {
        if (Globals.Instance.BluetoothData != null)
        {
            JSONList = Globals.Instance.BluetoothData.tasks;
            foreach (taskJSON task in JSONList)
            {
                if (!taskToDropdown.ContainsKey(task.majorkey))
                {

                }
            }
        }
        yield return new WaitForSeconds(2.0f);
    }
}
