using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Indestructible
{
    public class IndestructibleObjects : MonoBehaviour
    {
        public GameObject popupWindowObj; 

        private static IndestructibleObjects instance;
        public static IndestructibleObjects Instance
        {
            get
            {
                if(instance == null) instance=FindObjectOfType<IndestructibleObjects>();
                return instance;
            }
        }

        private void Awake()
        {
            if (instance == null)
            {
                instance = this;
                DontDestroyOnLoad(gameObject);
            }
            else
            {
                Destroy(gameObject);
            }
        }
    }
}
