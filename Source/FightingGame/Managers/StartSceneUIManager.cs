using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;
using UnityEngine.UI;

namespace StartScene
{
    public class StartSceneUIManager : MonoBehaviour
    {
        [SerializeField] GameObject nickNameCreatePanelObj;
        [SerializeField] GameObject nickNameInputFieldTextObj;
        [SerializeField] GameObject connetLogTextObj;

        static StartSceneUIManager instance;
        public static StartSceneUIManager Instance
        {
            get
            {
                if (instance == null)
                {
                    instance = FindObjectOfType<StartSceneUIManager>();
                }
                return instance;
            }
        }

        public void SetConnectLog(string log)
        {
            connetLogTextObj.GetComponent<TextMeshProUGUI>().text = log;
        }

        public void SetActiveNickNameInputFieldObj(bool active)
        {
            nickNameCreatePanelObj.SetActive(active);
            if (active)
                nickNameInputFieldTextObj.GetComponent<TextMeshProUGUI>().text = PlayerInformation.NickName;
        }

        public void OnNickNameButtonClicked()
        {
            string nickName = nickNameInputFieldTextObj.GetComponent<TextMeshProUGUI>().text;

            if (nickName == "AI") return;
            if (nickName.Contains(" ")) return;
            if (string.IsNullOrWhiteSpace(nickName)) return;
            if (nickName.Length > 10 || nickName.Length == 1) return;

            PlayerInformation.NickName = nickName;
            StartSceneManager.Instance.LoadLobbyScene();
        }
    }
}
