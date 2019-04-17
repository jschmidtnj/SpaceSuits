using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class PopulateTasks : MonoBehaviour {

    Text mainTask;
    Text subTasksTemp;
    Dictionary<int, Dictionary<int,string>> dictionary = new Dictionary<int, Dictionary<int,string>>();
   
    int currentPanel;
    int previousPanel = -1;
    float y_value;

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
        float temp_y = 0f;
        while (true)
        {
            currentPanel = Globals.Instance.currentTask;
            currentPanel %= dictionary.Count;
            if (dictionary.ContainsKey(currentPanel + 1))
            {
                if (previousPanel != currentPanel)
                {
                    y_value = subTasksTemp.transform.position.y;
                    temp_y = y_value;
                    GameObject[] temptaskList = GameObject.FindGameObjectsWithTag("subtasks");
                    foreach (GameObject obj in temptaskList)
                    {
                        Destroy(obj);
                    }
                    previousPanel = currentPanel;
                    Debug.Log("Switch Task Panels" + (currentPanel +1));
                }
                Dictionary<int, string> subtasks = dictionary[currentPanel + 1];
                mainTask.text = subtasks[0];           
                for (int i = 1; i < subtasks.Count; ++i)
                {
                
                    GameObject temp = GameObject.Find("subTasks" + i);
                    if (temp == null)
                    {
                        Text newText = Instantiate(subTasksTemp);
                        newText.transform.SetParent(this.transform);

                        newText.transform.localScale = new Vector3(1, 1, 1);
                        //newText.transform.position = new Vector3(15, temp_y, 0);
                        newText.transform.position = new Vector3(subTasksTemp.transform.position.x,temp_y, subTasksTemp.transform.position.z);
                        newText.enabled = true;
                        newText.name = "subTasks" + i;
                        newText.tag = "subtasks";
                        newText.text = subtasks[i];
                        temp_y -= 50;
                        Debug.Log("Offset " + temp_y);
                    }
                    else
                    {
                        if (temp.GetComponent<Text>().text != subtasks[i])
                        {
                            temp.GetComponent<Text>().text = subtasks[i];
                        }
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
            yield return new WaitForEndOfFrame();
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
            yield return new WaitForEndOfFrame();
        }
    }
}
