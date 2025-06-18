using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public interface ISlotsUpdatable
{
    void SetSlotsValue(int slotIndex, string characterName, bool readyState);
    void UpdateSlots(bool isCharacterAdded = false, string nickName = null,int slotNum=-1);
}
