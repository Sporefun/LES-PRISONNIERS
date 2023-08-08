static ref ItemBaseRPCHandler g_ItemBaseRPCHandler;

static ItemBaseRPCHandler GetItemBaseRPCHandler() {
	if (!g_ItemBaseRPCHandler)
		g_ItemBaseRPCHandler = new ItemBaseRPCHandler();
	return g_ItemBaseRPCHandler;
}

static ref ItemRestrictionsMainConfigClient g_ItemRestrictionsMainConfigClient;

static ItemRestrictionsMainConfigClient GetItemRestrictionsMainConfigClient() {
	if (!g_ItemRestrictionsMainConfigClient)
		GetItemBaseRPCHandler().RequestRestrictions();
	return g_ItemRestrictionsMainConfigClient;
}
static bool IsItemRestrictionDebugOutput() {
	if (GetGame().IsClient() && GetGame().IsMultiplayer()) {
		if (!g_ItemRestrictionsMainConfigClient)
			return false;
		return g_ItemRestrictionsMainConfigClient.debugOutput;
	} else {
		return GetItemRestrictionsMainConfig().debugOutput;
	}
	return false;
}

class ItemBaseRPCHandler {
	
	bool receivedChange = false;
	
	void ItemBaseRPCHandler() {
		if (IsItemRestrictionDebugOutput())
			Print("Creating ItemBaseRPCHandler");
		GetDayZGame().Event_OnRPC.Insert(OnRPC);
		if (GetGame().IsClient())
			RequestRestrictions();
	}
	
	void ~ItemBaseRPCHandler() {
		if (IsItemRestrictionDebugOutput())
			Print("Deleting ItemBaseRPCHandler");
		GetDayZGame().Event_OnRPC.Remove(OnRPC);
	}	
	
	int lastSentRestrictionRequest = -100;
	
	void RequestRestrictions() {
		int currentTime = GetGame().GetTime();
		if (lastSentRestrictionRequest - 2000 < currentTime) {
			lastSentRestrictionRequest = currentTime;
			GetGame().RPCSingleParam(NULL, 89456132, new Param1<bool>(true), true);
			GetGame().RPCSingleParam(NULL, 89456134, new Param1<bool>(true), true);
			Print("Restriction List RPC Sent");
		}
	}
	
	void OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx) {
		if (rpc_type == 89456133) {
			if (GetGame().IsClient()) {
				Param2<string, ref vector> positionParam;
				ItemBase.placedPositions.Clear();
				while (ctx.Read(positionParam)) {
					string type = positionParam.param1;
					vector pos = positionParam.param2;
					ItemBase.placedPositions.Insert(new Param2<string, ref vector>(type, Vector(pos[0], pos[1], pos[2])));
				}
			}
		} else if (rpc_type == 89456132) {
			if (!GetGame().IsServer() || !GetGame().IsMultiplayer())
				return;
			if (!sender)
				return;
			ItemBase.SendBuildListToPlayer(sender);
		} else if (rpc_type == 89456134) {
			Param1<ref ItemRestrictionsMainConfigClient> configParam;
			if (GetGame().IsClient()) {
				if (ctx.Read(configParam)) {
					g_ItemRestrictionsMainConfigClient = configParam.param1;
					if (IsItemRestrictionDebugOutput()) {
						Print("Received ItemRestrictionsMainConfigClient");
						Print("canOpenIngamePanel: " + g_ItemRestrictionsMainConfigClient.canOpenIngamePanel);
						Print("canSeePlayerMarkerOnPanelMap: " + g_ItemRestrictionsMainConfigClient.canSeePlayerMarkerOnPanelMap);
						Print("canSeePlacedItemsMarkersOnPanelMap: " + g_ItemRestrictionsMainConfigClient.canSeePlacedItemsMarkersOnPanelMap);
						Print("canSeePlacedItemsMarkers3D: " + g_ItemRestrictionsMainConfigClient.canSeePlacedItemsMarkers3D);
						Print("disableCrateCraftingRecipe: " + g_ItemRestrictionsMainConfigClient.disableCrateCraftingRecipe);
						Print("ignoreRestrictions: " + g_ItemRestrictionsMainConfigClient.ignoreRestrictions);
					}
				}
			} else {
				if (!sender)
					return;
				string steamid = sender.GetPlainId();
				ItemRestrictionsMainConfigClient conf = GetItemRestrictionsMainConfig().GetClientConfig(steamid);
				configParam = new Param1<ref ItemRestrictionsMainConfigClient>(conf);
				GetGame().RPCSingleParam(NULL, 89456134, configParam, true, sender);
			}
		}
	}
	
	void HandlePlacementCompleteLater(Param4<ref TStringArray, ref vector, Man, float> parm) {
		if (!parm) {
			Print("HandlePlacementCompleteLater did not recieve valid arguments !");
			return;
		}
		TStringArray targetType = parm.param1;
		vector position = parm.param2;
		Man builder = parm.param3;
		float maxDist = parm.param4;
		if (IsItemRestrictionDebugOutput())
			Print("HandlePlacementCompleteLater 1");
		if (!builder || !position)
			return;
		if (IsItemRestrictionDebugOutput())
			Print("HandlePlacementCompleteLater 2 Searching " + ItemBase.itemList.Count());
		ItemBase nearestItem = NULL;
		float nearestDist = -1;
		for (int i = 0; i < ItemBase.itemList.Count(); i++) {
			ItemBase item = ItemBase.itemList.Get(i);
			if (!item || targetType.Find(item.GetType()) == -1)
				continue;
			float dist = vector.Distance(position, item.GetPosition());
			if (dist > maxDist)
				continue;
			if (IsItemRestrictionDebugOutput())
				Print("Found near Item: " + item.GetType() + " distance: " + dist + " Builder: " + item.m_builderID);
			if ((nearestDist == -1 || nearestDist > dist) && !item.IsBuilderSaved()) {
				nearestItem = item;
				nearestDist = dist;
				if (dist <= 0.0001)
					break;
			}
		}
		if (nearestItem) {
			if (IsItemRestrictionDebugOutput())
				Print("HandlePlacementCompleteLater Item Found !");
			nearestItem.manualOnPlacementComplete = true;
			nearestItem.OnPlacementCompleteBB(builder);
		} else {
			if (IsItemRestrictionDebugOutput())
				Print("HandlePlacementCompleteLater no Item Found !");
		}
	}
}