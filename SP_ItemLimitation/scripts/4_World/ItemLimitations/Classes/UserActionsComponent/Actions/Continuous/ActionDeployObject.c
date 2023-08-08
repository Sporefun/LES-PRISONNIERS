modded class ActionDeployObject {
	
	override bool SetupAction(PlayerBase player, ActionTarget target, ItemBase item, out ActionData action_data, Param extra_data = NULL)
	{
		string steamid = "";
		if (player && player.GetIdentity()) {
			steamid = player.GetIdentity().GetPlainId();
		}
		PlaceRestrictionEntry entry = GetPlaceRestrictionConfig().GetEntryByAny(item.GetType());
		if (!entry) {
			ItemBase item2;
			if (GetGame().IsServer() && GetGame().IsMultiplayer() && player && player.GetHologramServer() && Class.CastTo(item2, player.GetHologramServer().GetProjectionEntity()) && item2) {
				entry = GetPlaceRestrictionConfig().GetEntryByAny(item2.GetType());
				if (entry && entry.useHologramAsKit)
					item = item2;
			} else if (GetGame().IsClient() && GetGame().IsMultiplayer() && player && player.GetHologramLocal() && Class.CastTo(item2, player.GetHologramLocal().GetProjectionEntity()) && item2) {
				entry = GetPlaceRestrictionConfig().GetEntryByAny(item2.GetType());
				if (entry && entry.useHologramAsKit)
					item = item2;
			}
		}
		if (!entry)
			return super.SetupAction(player, target, item, action_data, extra_data );
		int max = entry.max;
		int current = GetPlaceRestrictionConfig().GetPlacedItemCount(entry, steamid);

		if (max <= current && !ItemBase.IsPlacementAdmin(steamid)) {
			if (GetGame().IsClient())
				NotificationSystem.AddNotificationExtended(5, "Placing", "#lb_text_max_items_of " + item.GetDisplayName() + " (" + current + "/" + max + ") #lb_text_reached  ! #lb_text_limits_info", "set:ccgui_enforce image:HudBuild");
			return false;
		}
		return super.SetupAction(player, target, item, action_data, extra_data );
	}
	
	override void End( ActionData action_data )
	{
		ItemBase item = action_data.m_MainItem;
		PlayerBase player = action_data.m_Player;
		if (action_data && item) {
			PlaceRestrictionEntry entry = GetPlaceRestrictionConfig().GetEntryByAny(item.GetType());
			if (entry) {
				item.OnPlacementCompleteBB(player);
			}
		}
		super.End(action_data);
	}
}