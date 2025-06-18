using Photon.Pun;
using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using Indestructible;
using TMPro;
using UnityEngine.SceneManagement;

public class CharacterSlot : MonoBehaviourPunCallbacks, IChildGettable
{
    public enum OwnerType
    {
        Player, AI
    }
    public OwnerType ownerType;
    public string ownerName = "";
    public bool isReady = false;
    public GameObject currentCharacterObj;
    ISlotsUpdatable slotsUpdatable;
    int slotNum;
    [SerializeField] GameObject charactersScrollViewObj;

    public CharacterSlot()
    {
        ownerType = OwnerType.Player;
    }

    private void Awake()
    {
        int otherObjectCount = 0;
        for(int i = 0;i<transform.parent.childCount;i++)
        {
            if(transform.parent.GetChild(i)==transform)
            {
                slotNum = i- otherObjectCount;
                break;
            }
            else if(transform.parent.GetChild(i).gameObject.name!=gameObject.name)
            {
                otherObjectCount--;
            }
        }

        gameObject.SetActive(false);
    }

    private void Start()
    {
        GetChildObject("Delete_Button")?.GetComponent<Button>().onClick.AddListener(OnDeleteButtonClicked);
        GetChildObject("Ready_Button")?.GetComponent<Button>().onClick.AddListener(OnReadyButtonClicked);
        GetChildObject("CharacterChange_Button")?.GetComponent<Button>().onClick.AddListener(OnCharacterChangeButtonClicked);
    }

    public void InitSlot()
    {
        GetChildObject("Delete_Button")?.SetActive(false);
        GetChildObject("Captain_Image")?.SetActive(false);
    }

    public void UpdateSlot(OwnerType ownerType, string characterName, string ownerName, bool isReady,ISlotsUpdatable slotsUpdatable)
    {
        this.ownerName = ownerName;
        this.ownerType = ownerType;
        this.isReady = isReady;
        this.slotsUpdatable = slotsUpdatable;

        GetChildObject("Name_Panel").GetComponentInChildren<TextMeshProUGUI>().text = ownerName;

        if (currentCharacterObj != null)
            if (currentCharacterObj.name != characterName)
            {
                Destroy(currentCharacterObj);
                currentCharacterObj = null;
            }
        switch (ownerType)
        {
            case OwnerType.Player:
                if (PhotonNetwork.MasterClient.NickName != ownerName) GetChildObject("Ready_Button")?.SetActive(true);
                else
                {
                    GetChildObject("Ready_Button")?.SetActive(false);
                    GetChildObject("Captain_Image")?.SetActive(true);
                }

                GetChildObject("Ready_Button").GetComponent<Button>().interactable = true;
                GetChildObject("Ready_Button").GetComponent<Image>().color = isReady ? Color.grey : Color.white;

                if (ownerName == PhotonNetwork.NickName)
                {
                    GetChildObject("CharacterChange_Button")?.SetActive(true);
                }
                else
                {
                    GetChildObject("CharacterChange_Button")?.SetActive(false);
                    GetChildObject("Ready_Button").GetComponent<Button>().interactable = false;
                }

                //GetChildObject("Ready_Button").GetComponent<Image>().color = isReady ? Color.grey : Color.white;

                break;
            case OwnerType.AI:
                if (PhotonNetwork.IsMasterClient) GetChildObject("CharacterChange_Button")?.SetActive(true);
                else GetChildObject("CharacterChange_Button")?.SetActive(false);

                GetChildObject("Ready_Button")?.SetActive(false);
                break;
        }

        if (PhotonNetwork.IsMasterClient && ownerName != PhotonNetwork.NickName) GetChildObject("Delete_Button")?.SetActive(true);
        else GetChildObject("Delete_Button")?.SetActive(false);

        if (currentCharacterObj == null)
        {
            currentCharacterObj = Instantiate(CharacterStorage.Instance.GetCharacter(characterName), transform);
        }
    }

    public GameObject GetChildObject(string objName)
    {
        for (int i = 0; i < transform.childCount; i++)
        {
            GameObject childObj = transform.GetChild(i).gameObject;
            if (childObj.name == objName) return childObj;
        }

        return null;
    }

    public void OnReadyButtonClicked()
    {
        slotsUpdatable.SetSlotsValue(slotNum, currentCharacterObj.name, !isReady);
        slotsUpdatable.UpdateSlots();
    }

    public void OnDeleteButtonClicked()
    {
        GameObject popupWindowObj = IndestructibleObjects.Instance.popupWindowObj;
        popupWindowObj.SetActive(true);

        TextMeshProUGUI contentText = popupWindowObj.GetComponent<IChildGettable>().GetChildObject("Content_Text")?.GetComponent<TextMeshProUGUI>();
        contentText.text = "정말 추방 하시겠습니까?";

        Button checkButton = popupWindowObj.GetComponent<IChildGettable>().GetChildObject("Check_Button")?.GetComponent<Button>();
        Button exitButton = popupWindowObj.GetComponent<IChildGettable>().GetChildObject("Exit_Button")?.GetComponent<Button>();

        checkButton.onClick.RemoveAllListeners();
        checkButton.onClick.AddListener(() => { OnDeleteCheckButtonClicked(); popupWindowObj.SetActive(false); });

        exitButton.onClick.RemoveAllListeners();
        exitButton.onClick.AddListener(() => { popupWindowObj.SetActive(false); });
    }

    public void OnDeleteCheckButtonClicked()
    {
        if (!PhotonNetwork.IsMasterClient) return;

        switch(ownerType)
        {
            case OwnerType.AI:
                slotsUpdatable.UpdateSlots(false, OwnerType.AI.ToString(),slotNum);
                break;
            case OwnerType.Player:
                photonView.RPC("LeaveRoom", RpcTarget.Others);
                break;
        }
    }

    [PunRPC]
    private void LeaveRoom()
    {
        if (ownerName == PhotonNetwork.NickName)
        {
            PhotonNetwork.LeaveRoom();
            SceneManager.LoadScene("LobbyScene");
        }
    }

    void OnCharacterChangeButtonClicked()
    {
        charactersScrollViewObj.SetActive(true);
        var charactersScrollViewManager= charactersScrollViewObj.GetComponent<CharacterInventory>();
        charactersScrollViewManager.checkButton.onClick.RemoveAllListeners();
        charactersScrollViewManager.checkButton.onClick.AddListener(() => OnCharacterChangeCheckButtonClicked(charactersScrollViewManager));
    }

    void OnCharacterChangeCheckButtonClicked(CharacterInventory charactersScrollViewManager)
    {
        slotsUpdatable.SetSlotsValue(slotNum, charactersScrollViewManager.SelectedCharacterName, isReady);
        slotsUpdatable.UpdateSlots();
        charactersScrollViewManager.gameObject.SetActive(false);
    }
}
