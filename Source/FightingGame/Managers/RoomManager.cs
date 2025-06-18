using Photon.Pun;
using Photon.Realtime;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class RoomManager : MonoBehaviourPunCallbacks
{
    [SerializeField] GameObject slotsUpdaterObj;

    public override void OnPlayerEnteredRoom(Player newPlayer)
    {
        if (!PhotonNetwork.IsMasterClient) return;
    }

    public override void OnPlayerLeftRoom(Player otherPlayer)
    {
        if (!PhotonNetwork.IsMasterClient) return;

        slotsUpdaterObj.GetComponent<ISlotsUpdatable>()?.UpdateSlots(false, otherPlayer.NickName);
    }

    private void Start()
    {
        string PlayerNickName = PhotonNetwork.NickName;
        photonView.RPC("UpdateRoom", RpcTarget.MasterClient, PlayerNickName);
    }

    [PunRPC]
    void UpdateRoom(string newPlayerNickName)
    {
        slotsUpdaterObj.GetComponent<ISlotsUpdatable>()?.UpdateSlots(true, newPlayerNickName);
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
