using Photon.Pun;
using Photon.Realtime;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

namespace StartScene
{
    public class StartSceneManager : MonoBehaviourPunCallbacks
    {
        static StartSceneManager instance;
        public static StartSceneManager Instance
        {
            get
            {
                if (instance == null)
                {
                    instance = FindObjectOfType<StartSceneManager>();
                }
                return instance;
            }
        }

        private string gameVersion = "1";
        // Start is called before the first frame update
        void Start()
        {
            PhotonNetwork.GameVersion = gameVersion;
            PhotonNetwork.ConnectUsingSettings();
        }

        public override void OnConnectedToMaster()
        {
            base.OnConnectedToMaster();
            StartSceneUIManager.Instance.SetActiveNickNameInputFieldObj(true);
            StartSceneUIManager.Instance.SetConnectLog("Conneted!");
        }

        public override void OnDisconnected(DisconnectCause cause)
        {
            StartSceneUIManager.Instance.SetActiveNickNameInputFieldObj(false);
            StartSceneUIManager.Instance.SetConnectLog("Connecting...");

            PhotonNetwork.ConnectUsingSettings();
        }

        public void LoadLobbyScene()
        {
            SceneManager.LoadScene("LobbyScene");
        }
    }
}
