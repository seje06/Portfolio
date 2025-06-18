using Photon.Pun;
using System;
using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class CharacterSlotManager : MonoBehaviourPunCallbacks,ISlotsUpdatable
{
    public int aiCount { get; private set; }

    [SerializeField]List<GameObject> slots;
    [SerializeField]List<GameObject> aICreateButtonObjs;
    
    List<string> slotOwnerNickNames = new List<string>();
    List<string> currentCharacterNames = new List<string>();
    List<bool> readyStates = new List<bool>();

    public void SetSlotsValue(int slotIndex, string characterName, bool readyState)
    {
        currentCharacterNames[slotIndex] = characterName;
        readyStates[slotIndex] = readyState;
    }

    public void UpdateSlots(bool isCharacterAdded=false, string nickName= null,int slotNum=-1)
    {
        if (nickName != null)
        {
            if (isCharacterAdded)
            {
                slotOwnerNickNames.Add(nickName);
                currentCharacterNames.Add(CharacterStorage.CharacterName.Catcher_Medium.ToString());
                readyStates.Add(false);
            }
            else
            {
                Debug.Log(nickName);
                int index;
                if (nickName == CharacterSlot.OwnerType.AI.ToString()&& slotNum!=-1)
                {
                    index = slotNum;
                }
                else
                {
                    if (!slotOwnerNickNames.Contains(nickName)) return;

                    index = slotOwnerNickNames.IndexOf(nickName);
                }
                slotOwnerNickNames.RemoveAt(index);
                currentCharacterNames.RemoveAt(index);
                readyStates.RemoveAt(index);
            }
        }

        string addedNickName="";
        string addedCharacterName = "";
        string addedReadyState = "";

        for(int i=0;i< slotOwnerNickNames.Count;i++)
        {
            addedNickName += slotOwnerNickNames[i];
            addedNickName += "/";
            addedCharacterName += currentCharacterNames[i];
            addedCharacterName += "/";
            addedReadyState += readyStates[i]? '1':'0';
        }

        photonView.RPC("OnSlotsUpdated", RpcTarget.All, addedNickName, addedCharacterName, addedReadyState);
    }

    [PunRPC]
    public void OnSlotsUpdated(string addedNickName,string addedCharacterName,string addedReadyState)
    {
        slotOwnerNickNames.Clear();
        currentCharacterNames.Clear();
        readyStates.Clear();

        for (int i=0;i<slots.Count;i++)
        {
            slots[i].gameObject.SetActive(false);
            if (i != 5) 
            {   
                aICreateButtonObjs[i].gameObject.SetActive(false); 
            }
        }

        string nickName = "";
        for(int i=0;i<addedNickName.Length;i++)
        {
            if (addedNickName[i] != '/')
            {
                nickName += addedNickName[i];
            }
            else 
            {
                slotOwnerNickNames.Add(nickName);
                nickName = "";
            }
        }

        string name = "";
        for(int i=0;i<addedCharacterName.Length;i++)
        {
            if (addedCharacterName[i]!='/')
            {
                name+= addedCharacterName[i];
            }
            else
            {
                currentCharacterNames.Add(name);
                name = "";
            }
        }

        for (int i = 0; i < addedReadyState.Length; i++)
        {
            readyStates.Add((addedReadyState[i] == '0') ? false : true);
        }

        for (int i=0;i< slotOwnerNickNames.Count;i++)
        {
            slots[i].SetActive(true);

            CharacterSlot slotManager = slots[i].GetComponent<CharacterSlot>();
            slotManager.InitSlot();

            if (slotOwnerNickNames[i] == CharacterSlot.OwnerType.AI.ToString())
            {
                slotManager.UpdateSlot(CharacterSlot.OwnerType.AI, currentCharacterNames[i], slotOwnerNickNames[i], readyStates[i],this);
            }
            else
            {
                slotManager.UpdateSlot(CharacterSlot.OwnerType.Player, currentCharacterNames[i], slotOwnerNickNames[i], readyStates[i],this);
            }
            
        }
        if(slotOwnerNickNames.Count!=6) 
        {
            GameObject aICreateButtonObj = aICreateButtonObjs[slotOwnerNickNames.Count - 1];
            Button aICreateButton = aICreateButtonObj.GetComponent<Button>();

            aICreateButtonObj.SetActive(true);
            aICreateButton.interactable = true;

            if (!PhotonNetwork.IsMasterClient) aICreateButton.interactable = false;
        }
    }

    public void OnAICreateButtonClicked()
    {
        if (!PhotonNetwork.IsMasterClient) return;

        UpdateSlots(true, CharacterSlot.OwnerType.AI.ToString());
    }
}
