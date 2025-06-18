using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PopupWindow : MonoBehaviour, IChildGettable
{
    public GameObject GetChildObject(string objName)
    {
        for (int i = 0; i < transform.childCount; i++)
        {
            GameObject childObj = transform.GetChild(i).gameObject;
            if (childObj.name == objName) return childObj;
        }

        return null;
    }
}
