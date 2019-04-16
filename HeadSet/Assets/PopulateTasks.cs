using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class PopulateTasks : MonoBehaviour {

    Text mainTask;
    Text subTasksTemp;
    Dictionary<int, Dictionary<int,string>> dictionary = new Dictionary<int, Dictionary<int,string>>();
   
    public int currentPanel = 1;
    int previousPanel = -1;
    int y_value = 40;

    // Use this for initialization
    void Start () {
        mainTask = GameObject.Find("MainTask").GetComponent<Text>();
        subTasksTemp = GameObject.Find("SubTaskTemplate").GetComponent<Text>();

        subTasksTemp.enabled = false;
        StartCoroutine(readJsonData());
        StartCoroutine(loadTasks());
	}

    IEnumerator loadTasks()
    {
        while (true)
        {
            currentPanel = Globals.Instance.currentTask;
            if (dictionary.Count == 0)
                continue;
            currentPanel %= dictionary.Count;
            if (dictionary.ContainsKey(currentPanel))
            {
                if (previousPanel != currentPanel)
                {
                    GameObject[] temptaskList = GameObject.FindGameObjectsWithTag("subtasks");
                    foreach (GameObject obj in temptaskList)
                    {
                        Destroy(obj);
                    }
                    previousPanel = currentPanel;
                }
                Dictionary<int, string> subtasks = dictionary[currentPanel];
                mainTask.text = subtasks[0];
                Debug.Log(subtasks[0]);
                int temp_y = y_value;
                for (int i = 1; i < subtasks.Count; ++i)
                {
                    if (GameObject.Find("subTasks" + i) == null)
                    {
                        Text newText = Instantiate(subTasksTemp);
                        newText.transform.SetParent(this.transform);

                        newText.transform.localScale = new Vector3(1, 1, 1);
                        //newText.transform.position = new Vector3(15, temp_y, 0);
                        newText.transform.position = new Vector3(subTasksTemp.transform.position.x,subTasksTemp.transform.position.y, subTasksTemp.transform.position.z);
                        newText.enabled = true;
                        newText.name = "subTasks" + i;
                        newText.tag = "subtasks";
                        newText.text = subtasks[i];
                        temp_y -= 10;
                    }
                }
                // Should handle deletions 
                GameObject[] taskList = GameObject.FindGameObjectsWithTag("subtasks");
                if (taskList.Length > subtasks.Count)
                {
                    
                    int start = subtasks.Count + 1;
                    while (start <= taskList.Length)
                    {
                        Destroy(taskList[start]);
                        ++start;
                    }
                }
            }
            yield return new WaitForSeconds(.50f);
        }
    }
    IEnumerator readJsonData()
    {
        while (true) {
            if (Globals.Instance.BluetoothData != null && Globals.Instance.BluetoothData.tasks!=null)
            {
                taskJSON[] JSONList = Globals.Instance.BluetoothData.tasks;
                foreach (taskJSON task in JSONList)
                {
                    if (!dictionary.ContainsKey(task.majorkey))
                    {
                        dictionary[task.majorkey] = new Dictionary<int, string>();
                    }
                    Dictionary<int, string> subtask = dictionary[task.majorkey];
                    if (!subtask.ContainsKey(task.subkey) || subtask[task.subkey] != task.data)
                    {
                        subtask[task.subkey] = task.data;
                    }  
                }
            }
            yield return new WaitForSeconds(1.0f);
        }
    }
}
