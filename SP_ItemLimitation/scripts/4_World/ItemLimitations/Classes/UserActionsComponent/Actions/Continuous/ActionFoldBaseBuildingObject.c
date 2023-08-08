modded class ActionFoldBaseBuildingObject {
	
	override void OnExecuteServer( ActionData action_data )
	{
		Object targetParent = action_data.m_Target.GetParent();
		if (IsItemRestrictionDebugOutput())
			Print("OnItemPickedUp. Target: " + targetParent);
		super.OnExecuteServer(action_data);
		if (targetParent && targetParent.IsInherited(ItemBase))
			ItemBase.Cast(targetParent).OnItemPickedUp();
	}
}