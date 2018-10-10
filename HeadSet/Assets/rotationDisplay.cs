using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class rotationDisplay : MonoBehaviour {
    public Text textElement;
    public Camera camera;

	// Use this for initialization
	void Start () {
		
	}
	
	// Update is called once per frame
	void Update () {
        textElement.text = camera.transform.localEulerAngles.ToString() ;
	}

}
