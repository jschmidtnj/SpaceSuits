using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Networking;
using UnityEngine.UI;

public class GetTasksData : MonoBehaviour {

    void Start()
    {
        StartCoroutine(Get_Request("https://nasasuits.appspot.com/gettaskdata"));
    }

    IEnumerator Get_Request(string uri)
    {
        while (true)
        {
            using (UnityWebRequest webRequest = UnityWebRequest.Get(uri))
            {
                // Request and wait for the desired page.
                yield return webRequest.SendWebRequest();

                if (webRequest.isNetworkError)
                {
                    Debug.Log("Error Reading \n");
                }
                else
                {
                    int length = webRequest.downloadHandler.text.Length;
                    string obj = webRequest.downloadHandler.text;
                    ArrayWrapper task = new ArrayWrapper();
                    try
                    {
                        task = JsonUtility.FromJson<ArrayWrapper>("{ \"tasks\" : " + obj + "}" );
                        Globals.Instance.server_data = task.tasks;
                        Debug.Log("Tasks Data from Server" + obj);
                    }
                    catch (Exception e) {
                        Debug.Log(e);
                        Debug.Log("Error " + "{ \"tasks\" : " + obj + "}");
                    }
                    
                }
            }
            yield return new WaitForSeconds(1f);
         
        }
    }
}
