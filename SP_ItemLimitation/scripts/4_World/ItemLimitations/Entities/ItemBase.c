modded class ItemBase {
	
	const int BUILDER_ID_LENGTH = 17;
	
	bool m_buildersaved = false;
	string m_builderID = "NONE";
	
	int lastSentRequest = 0;
	
	static ref array<ItemBase> itemList = new array<ItemBase>;
	static ref array<ref Param2<string, ref vector>> placedPositions = new array<ref Param2<string, ref vector>>();
	static bool receivedCanPlaceList = false;
	bool manualOnPlacementComplete = false;
	bool placeRestriction = false;
	
	PlaceRestrictionEntry cachedEntry = NULL;
	
	void ItemBase() {
		OnLoad();
	}
	
	void ~ItemBase() {
		if (GetGame() && HasPlaceRestriction()) {
			itemList.RemoveItem(this);
		}
		PlayerIdentity builderIdent = GetBuilderIdentity();
		if (builderIdent)
			SendBuildListToPlayer(builderIdent);
	}
	
	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;
		if (GetGame() && GetGame().IsServer() && GetItemRestrictionsMainConfig() && !GetItemRestrictionsMainConfig().installed)
			return true;
		if (!ctx.Read(m_buildersaved)) {
			m_buildersaved = false;
			Print("Unable to read m_buildersaved !");
			return false;
		}
		if (m_buildersaved) {
			if (!ctx.Read(m_builderID)) {
				m_builderID = "NONE";
				Print("Unable to read m_builderID");
				return false;
			}
			if (IsItemRestrictionDebugOutput())
				Print("Loading Item " + GetType() + " with builder Saved. Builer: " + m_builderID);
		}
		OnLoad();
		return true;
	}
	
	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		if (GetGame() && GetGame().IsServer() && GetItemRestrictionsMainConfig() && GetItemRestrictionsMainConfig().uninstallmode)
			return;
		bool saved = IsBuilderSaved();
		ctx.Write(saved);
		if (saved) {
			if (IsItemRestrictionDebugOutput())
				Print("Saved " + GetType() + " with builder " + m_builderID);
			ctx.Write(m_builderID);
		}
	}
	
	override void OnRPC( PlayerIdentity sender, int rpc_type, ParamsReadContext  ctx) {
		super.OnRPC(sender, rpc_type, ctx);
		if (rpc_type == 87986515) {
			Param1<string> params;
			if (GetGame().IsServer()) {
				if (!sender)
					return;
				params = new Param1<string>(m_builderID);
				GetGame().RPCSingleParam(this, 87986515, params, true, sender);
			} else if (ctx.Read(params)) {
				SetBuilder(params.param1);
			}
		}
	}
	
	PlayerIdentity GetBuilderIdentity(string builder = "") {
		if (!GetGame() || !GetGame().IsServer())
			return NULL;
		if (!IsBuilderSaved() && builder.Length() != BUILDER_ID_LENGTH)
			return NULL;
		else if (builder.Length() != BUILDER_ID_LENGTH)
			builder = m_builderID;
		if (builder.Length() == BUILDER_ID_LENGTH) {
			array<PlayerIdentity> players = new array<PlayerIdentity>();
			GetGame().GetPlayerIndentities(players);
			if (IsItemRestrictionDebugOutput())
				Print("Trying to get Identity of " + builder + " in list of " + players.Count() + " Players...");
			for (int i = 0; i < players.Count(); i++) {
				if (players.Get(i) && players.Get(i).GetPlainId() == builder) {
					if (IsItemRestrictionDebugOutput())
						Print("Found Identity of Builder " + builder + " at index " + i + " Name: " + players.Get(i).GetName());
					return players.Get(i);
				}
			}
			if (IsItemRestrictionDebugOutput())
				Print("Unable to find " + builder + " in list of " + players.Count() + " players");
		} else {
			if (IsItemRestrictionDebugOutput())
				Print("Not attempting to find Builder \"" + builder + "\" ! Saved: " + IsBuilderSaved());
		}
		return NULL;
	}
	
	void SyncWithClients() {
		if (IsItemRestrictionDebugOutput())
			Print("Syncing Builder with Clients. New builder: " + m_builderID + " Save: " + IsBuilderSaved());
		if (GetGame().IsMultiplayer() && GetGame().IsServer()) {
			Param1<string> params = new Param1<string>(m_builderID);
			GetGame().RPCSingleParam(this, 87986515, params, true);
		}
	}
	
	void RemoveBuilder() {
		string builder = m_builderID;
		
		m_builderID = "";
		m_buildersaved = false;
		SyncWithClients();
		
		if (GetGame().IsServer() && GetGame().IsMultiplayer()) {
			PlayerIdentity builderIdent = GetBuilderIdentity(builder);
			if (builderIdent) {
				if (IsItemRestrictionDebugOutput())
					Print("Informing Builder " + builder + " of Item removed builder");
				SendBuildListToPlayer(builderIdent);
			} else {
				if (IsItemRestrictionDebugOutput())
					Print("Unable to Inform Builder " + builder + " of Item removed Builder !");
			}
		}
	}
	
	void SetBuilder(PlayerBase player) {
		if (!player || !player.GetIdentity())
			return;
		SetBuilder(player.GetIdentity().GetPlainId());
	}
	
	void SetBuilder(string builderID) {
		m_builderID = builderID;
		if (m_builderID && m_builderID.Length() == BUILDER_ID_LENGTH)
			m_buildersaved = true;
		else
			m_buildersaved = false;
		SyncWithClients();
	}
	
	string GetBuilderId() {
		if (GetGame().IsClient() && (!m_builderID || m_builderID.Length() != BUILDER_ID_LENGTH || m_builderID == "00000000000000000")) {
			int currentTime = GetGame().GetTime();
			if (lastSentRequest - 2000 < currentTime) {
				lastSentRequest = currentTime;
				GetGame().RPCSingleParam(this, 87986515, NULL, true);
			}
			return "error";
		}
		return m_builderID;
	}
	
	bool IsBuilderSaved()
	{
		return m_buildersaved && m_builderID && m_builderID.Length() == BUILDER_ID_LENGTH;
	}
	
	static void SendBuildListToPlayer(PlayerIdentity player) {
		if (!player)
			return;
		string steamid = player.GetPlainId();
		GetPlaceRestrictionConfig().OnRPC(player, NULL, 8564572, NULL);
		array<ref Param> placePositions = new array<ref Param>();
		for (int i = 0; i < itemList.Count(); i++) {
			ItemBase itemBs = itemList.Get(i);
			if (!itemBs)
				continue;
			if (itemBs.m_builderID != steamid)
				continue;
			vector pos = itemBs.GetPosition();
			string type = itemBs.GetType();
			placePositions.Insert(new Param2<string, ref vector>(type, pos));
		}
		if (IsItemRestrictionDebugOutput())
			Print("Sending Can Place Entries to Client: " + placePositions.Count());
		GetGame().RPC(NULL, 89456133, placePositions, true, player);
	}
	
	static void SendAllBuildLists() {
		if (GetGame() && GetGame().IsServer()) {
			array<PlayerIdentity> identityList = new array<PlayerIdentity>;
			GetGame().GetPlayerIndentities(identityList);
			for (int i = 0; i < identityList.Count(); i++) {
				SendBuildListToPlayer(identityList.Get(i));
			}
		}
	}
	
	bool HasPlaceRestriction() {
		return GetPlaceRestrictionConfig().GetEntryByAny(GetType()) != NULL;
	}
	
	bool CanPlace(string steamid = "") {
		if (!HasPlaceRestriction())
			return true;
		
		return GetPlaceRestrictionConfig().CanPlaceItem(GetType(), steamid);
	}
	
	static bool IsPlacementAdmin(string steamid = "") {
		if (GetGame().IsServer() && GetGame().IsMultiplayer()) {
			ItemRestrictionsMainConfigClient conf = GetItemRestrictionsMainConfig().GetClientConfig(steamid);
			if (conf && conf.ignoreRestrictions)
				return true;
		} else {
			if (GetItemRestrictionsMainConfigClient() && GetItemRestrictionsMainConfigClient().ignoreRestrictions)
				return true;
		}
		return false;
	}
	
	#ifdef DAYZ_1_09
	override void OnPlacementComplete( Man player ) {
		if (GetGame().IsServer()) {
			PlaceRestrictionEntry entry = GetPlaceRestrictionConfig().GetEntryByAny(GetType());
			if (entry && entry.useHologramAsKit) {
				OnPlacementCompleteBB(player);
			}
		}
		super.OnPlacementComplete(player);
	}
	#endif
	#ifndef DAYZ_1_09
	override void OnPlacementComplete( Man player, vector position = "0 0 0", vector orientation = "0 0 0" ) {
		if (GetGame().IsServer()) {
			PlaceRestrictionEntry entry = GetPlaceRestrictionConfig().GetEntryByAny(GetType());
			if (entry && entry.useHologramAsKit) {
				OnPlacementCompleteBB(player, position, orientation);
			}
		}
		super.OnPlacementComplete(player, position, orientation);
	}
	#endif
	
	void OnPlacementCompleteBB(Man player, vector position = "0 0 0", vector orientation = "0 0 0" ) {
		if (IsItemRestrictionDebugOutput())
			Print("OnPlacementCompleteBB. Manual: " + manualOnPlacementComplete + " " + GetType());
		if (!GetGame().IsServer())
			return;
		if (IsBuilderSaved())
			RemoveBuilder();
		PlaceRestrictionEntry entry = GetPlaceRestrictionConfig().GetEntryByAny(GetType());
		if (!entry)
			return;
		TStringArray alternatives = entry.countItemNames;
		if (IsItemRestrictionDebugOutput())
			Print("Alternatives of " + GetType() + " are " + alternatives);
		if (!manualOnPlacementComplete && alternatives.Count() > 0 && (!GetPlaceRestrictionConfig().KitIsCounted(entry) || entry.useHologramAsKit)) {
			vector pos;
			if (position != vector.Zero) {
				pos = position;
			} else {
				pos = GetPosition();
				PlayerBase pb;
				if (Class.CastTo(pb, player)) {
					vector projectionPos = pb.GetLocalProjectionPosition();
					if (projectionPos && vector.Distance(projectionPos, pos) < 20)
						pos = projectionPos;
				}
			}
			if (IsItemRestrictionDebugOutput())
				Print("HandlePlacementCompleteLater delayed");
			Param4<ref TStringArray, ref vector, Man, float> parm = new Param4<ref TStringArray, ref vector, Man, float>(alternatives, Vector(pos[0], pos[1], pos[2]), player, 10.0);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(GetItemBaseRPCHandler().HandlePlacementCompleteLater, 500, false, parm);
		} else {
			if (player && player.GetIdentity()) {
				SetBuilder(PlayerBase.Cast(player));
				SendBuildListToPlayer(player.GetIdentity());
			}
		}
		OnLoad();
		manualOnPlacementComplete = false;
	}
	
	void OnItemPickedUp() {
		if (IsItemRestrictionDebugOutput())
			Print("Item " + GetType() + " was picked up");
		string builder = m_builderID;
		if (GetGame() && GetGame().IsServer() && GetGame().IsMultiplayer() && IsBuilderSaved())
			RemoveBuilder();
	}
	
	void OnLoad() {
		if (GetGame().IsServer() && GetPlaceRestrictionConfig().GetRestrictedItemCount(GetType()) != -1)
			this.placeRestriction = true;
		if ((IsBuilderSaved() || HasPlaceRestriction()) && itemList.Find(this) == -1) {
			if (IsItemRestrictionDebugOutput())
				Print("OnLoad adding " + GetType() + " to itemlist");
			itemList.Insert(this);
		}
	}
}