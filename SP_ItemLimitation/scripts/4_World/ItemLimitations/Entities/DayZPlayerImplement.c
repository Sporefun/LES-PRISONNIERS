modded class DayZPlayerImplement {

	override void CommandHandler(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		super.CommandHandler(pDt,pCurrentCommandID,pCurrentCommandFinished);
		//Print("CommandHandler: " + pDt + " " + pCurrentCommandID + " " + pCurrentCommandFinished);
		UAInput localInput1 = GetUApi().GetInputByName("UAOpenLimitationsPanel");
		if(localInput1.LocalPress()) {
			if (!GetItemRestrictionsMainConfigClient() || !GetItemRestrictionsMainConfigClient().canOpenIngamePanel) {
				if (IsItemRestrictionDebugOutput())
					Print("Item Restrictions Panel Key was pressed, but panel is disabled");
				return;
			}
			if (IsItemRestrictionDebugOutput())
				Print("Item Restrictions Panel Key was pressed");
			MissionBaseWorld mission = MissionBaseWorld.Cast(GetGame().GetMission());
			if (mission)
				mission.OnOpenLimitationsPanel();
		}
	}
	
	override void OnItemInHandsChanged ()
	{
		super.OnItemInHandsChanged();
		if (!GetHumanInventory() || !GetHumanInventory().GetEntityInHands())
			return;
		ItemBase item = ItemBase.Cast(GetHumanInventory().GetEntityInHands());
		if (!item)
			return;
		item.OnItemPickedUp();
	}
}