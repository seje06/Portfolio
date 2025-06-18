using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using Photon.Pun;
using Photon.Realtime;
using static UnityEngine.Rendering.DebugUI;

public class LobbyManager : MonoBehaviourPunCallbacks
{
    static LobbyManager instance;
    public static LobbyManager Instance
    {
        get
        {
            if (instance == null)
            {
                instance = FindObjectOfType<LobbyManager>();
            }
            return instance;
        }
    }

    public override void OnDisconnected(DisconnectCause cause)
    {
        
        PhotonNetwork.ConnectUsingSettings();
    }

    public void JoinRoom(string roomTitle)
    {
        PhotonNetwork.JoinRoom(roomTitle);
    }

    public override void OnJoinedRoom()
    {
        foreach(var player in PhotonNetwork.PlayerList)
        {
            if(player.NickName.Equals(PlayerInformation.NickName))
            {
                PlayerInformation.NickName = PlayerInformation.NickName+"2";
                OnJoinedRoom();
                return;
            }
        }
        PhotonNetwork.NickName = PlayerInformation.NickName;

        if (PhotonNetwork.IsMasterClient)
        {
            PhotonNetwork.LoadLevel("RoomScene");
        }
        else
        {
            PhotonNetwork.LoadLevel("RoomScene");
        }
    }

    public override void OnJoinRandomFailed(short returnCode, string message)
    {
        
    }

    public void UpdateRoomList()
    {
        PhotonNetwork.JoinLobby();
    }

    public override void OnRoomListUpdate(List<RoomInfo> roomList)
    {
        base.OnRoomListUpdate(roomList);

        foreach (RoomInfo roomInfo in roomList) 
        {
            if (!roomInfo.RemovedFromList)
            {
                string roomTitle = roomInfo.Name + " (" + roomInfo.PlayerCount + "/" + roomInfo.MaxPlayers + ")";
                LobbyUIManager.Instance.InstantiateRoomButtonToContent(roomTitle);
            }
        }
    }

    public void CreateRoom(string roomTitle)
    {
        RoomOptions roomOptions = new RoomOptions();
        roomOptions.PlayerTtl = 10;
        roomOptions.MaxPlayers = 4;
        if (PhotonNetwork.CreateRoom(roomTitle, roomOptions))
        {

        }
    }

    
}
