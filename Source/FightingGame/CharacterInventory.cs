using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
public class CharacterInventory : MonoBehaviour
{
    [SerializeField] GameObject content;
    [SerializeField] GameObject characterButtonPrefeb;
    public Button checkButton;
    List<GameObject> characterButtons;
    public string SelectedCharacterName { get; private set; }

    private void Awake()
    {
        characterButtons = new List<GameObject>();
    }
    private void OnEnable()
    {
        foreach (var obj in characterButtons)
        {
            obj.GetComponent<Button>().interactable = true;
        }
    }
    private void Start()
    {
        foreach(var obj in CharacterStorage.Instance.characterPrefebs)
        {
            characterButtons.Add(Instantiate(characterButtonPrefeb, content.transform));
            characterButtons[characterButtons.Count - 1].name = obj.name+"_Button";
            characterButtons[characterButtons.Count - 1].GetComponent<Button>().onClick.AddListener(() => OnCharacterButtonClicked(obj.name));
        }
    }

    public void OnCharacterButtonClicked(string characterName)
    {
        SelectedCharacterName = characterName;
        foreach (var obj in characterButtons)
        {
            obj.GetComponent<Button>().interactable = true;
            if (obj.name==characterName+"_Button")
            {
                obj.GetComponent<Button>().interactable = false;
            }
        }
    }
}
