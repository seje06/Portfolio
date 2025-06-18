
using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class LobbyUIManager : MonoBehaviour
{
    static LobbyUIManager instance;
    public static LobbyUIManager Instance
    {
        get
        {
            if(instance == null)
            {
                instance = FindObjectOfType<LobbyUIManager>();
            }
            return instance;
        }
    }

    [SerializeField] GameObject inputWindowPanelObj;
    [SerializeField] TextMeshProUGUI inputWindowTitleText;
    [SerializeField] TMP_InputField inputWindowInputField;
    [SerializeField] Button inputWindowCheckButton;

    [SerializeField] GameObject roomButtonPrefeb;
    [SerializeField] GameObject roomListContentObj;

    Button currentSelectedRoomButton;

    public void OnRoomCreateButtonClick()
    {
        inputWindowPanelObj.SetActive(true);
        inputWindowInputField.text = "";
        inputWindowTitleText.text = "持失";
    }

    public void OnRoomEnterButtonClick()
    {
        if (currentSelectedRoomButton == null || currentSelectedRoomButton.interactable) return;

        string roomTitle = currentSelectedRoomButton.transform.GetChild(0).GetComponent<TextMeshProUGUI>().text;
        string realRoomTile="";
        for(int i = 0;i<roomTitle.Length-6;i++) 
        {
            realRoomTile += roomTitle[i];
        }

        LobbyManager.Instance.JoinRoom(realRoomTile);
    }

    public void OnRoomSearchButtonClick()
    {
        inputWindowPanelObj.SetActive(true);
        inputWindowInputField.text = "";
        inputWindowTitleText.text = "伊事";
    }

    public void OnRoomListUpdateButtonClick()
    {
        for (int i = 0; i < roomListContentObj.transform.childCount; i++)
        {
            Destroy(roomListContentObj.transform.GetChild(i).gameObject);
        }

        LobbyManager.Instance.UpdateRoomList();
    }

    public void InstantiateRoomButtonToContent(string roomTitle)
    {
        GameObject roomButtonObj=Instantiate(roomButtonPrefeb, roomListContentObj.transform);

        roomButtonObj.transform.GetChild(0).GetComponent<TextMeshProUGUI>().text = roomTitle;

        Button roomButton = roomButtonObj.GetComponent<Button>();
        roomButton.onClick.AddListener(()=>OnRoomButtonClick(roomButton));
    }

    void OnRoomButtonClick(Button roomButton)
    {
        if (currentSelectedRoomButton != null) currentSelectedRoomButton.interactable = true;
        currentSelectedRoomButton = roomButton;
        currentSelectedRoomButton.interactable = false;
    }

    public void OnInputWindowCheckButtonClick()
    {
        switch(inputWindowTitleText.text)
        {
            case "持失": 
                string roomTitle = inputWindowInputField.text;

                if (roomTitle == "") return ;
                if (string.IsNullOrWhiteSpace(roomTitle)) return;
                if (roomTitle.Length > 9 || roomTitle.Length <= 1) return;

                LobbyManager.Instance.CreateRoom(roomTitle);
                break;

            case "伊事": LobbyManager.Instance.JoinRoom(inputWindowInputField.text); break;
        }
    }
}
