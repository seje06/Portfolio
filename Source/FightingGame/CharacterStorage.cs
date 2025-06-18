using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CharacterStorage : MonoBehaviour
{
    static CharacterStorage instance;
    public static CharacterStorage Instance
    {
        get
        {
            if (instance == null)
            {
                instance = FindObjectOfType<CharacterStorage>();
            }
            return instance;
        }
    }

    public enum CharacterName
    {
        Catcher_Big,Catcher_Medium, Catcher_Small,
        Fishguard_Big, Fishguard_Medium, Fishguard_Small,
        Imp_Big, Imp_Medium, Imp_Small
    }

    public GameObject[] characterPrefebs;

    public GameObject GetCharacter(string characterName)
    {
        foreach (GameObject character in characterPrefebs) 
        {
            if (character.name == characterName||character.name+ "(Clone)"==characterName)
            {
                return character;
            }
        }
        return null;
    }
}
