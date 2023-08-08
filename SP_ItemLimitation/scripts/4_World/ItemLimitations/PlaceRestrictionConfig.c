static ref PlaceRestrictionConfig g_PlaceRestrictionConfig;
static PlaceRestrictionConfig GetPlaceRestrictionConfig() {
	if (!g_PlaceRestrictionConfig)
		g_PlaceRestrictionConfig = new PlaceRestrictionConfig();
	return g_PlaceRestrictionConfig;
}
class PlaceRestrictionEntry
{
	string displayName;
	ref TStringArray kitItemNames = new TStringArray;
	bool useHologramAsKit = false;
	ref TStringArray countItemNames = new TStringArray;
	int max;
	bool showOnMap;
	bool show3D;
	
	string ToOutputString() {
		return displayName + " " + kitItemNames + " " + countItemNames + " " + max + " " + showOnMap + " " + show3D;
	}
}
class PlaceRestrictionEntryClient : PlaceRestrictionEntry {
	int placed;
	
	static PlaceRestrictionEntryClient fromPlaceRestrictionEntry(PlaceRestrictionEntry entry) {
		PlaceRestrictionEntryClient clientEntry = new PlaceRestrictionEntryClient();
		clientEntry.displayName = entry.displayName;
		clientEntry.kitItemNames = entry.kitItemNames;
		clientEntry.countItemNames = entry.countItemNames;
		clientEntry.max = entry.max;
		clientEntry.showOnMap = entry.showOnMap;
		clientEntry.show3D = entry.show3D;
		clientEntry.placed = 0;
		return clientEntry;
	}
	
	override string ToOutputString() {
		return displayName + " " + kitItemNames.Count() + " " + countItemNames.Count() + " " + placed + "/" + max + " " + showOnMap + " " + show3D;
	}
}
class PlaceRestrictionEntry_v1
{
	string itemName;
	string alternativeItemName;
	int max;
	bool showOnMap;
	bool show3D;
}

class PlaceRestrictionConfig {
	
	ref array<ref PlaceRestrictionEntry> restrictions = new array<ref PlaceRestrictionEntry>();
	bool receivedRestrictions = false;
	
	void PlaceRestrictionConfig() {
		GetDayZGame().Event_OnRPC.Insert(OnRPC);
		if (!GetGame().IsServer()) {
			GetGame().RPCSingleParam(NULL, 8564572, new Param1<bool>(true), true);
			return;
		}
		if (!FileExist("$profile:LBmaster")) {
			MakeDirectory("$profile:LBmaster");
			Print("Created Directory for Item Restrictions");
		} else {
			Print("Directory for Item Restrictions already exists");
		}
		ConvertConfigs();
		JsonFileLoader<ref array<ref PlaceRestrictionEntry>>.JsonLoadFile( "$profile:LBmaster/itemrestrictions_v2.json", restrictions );
		Print("Read " + restrictions.Count() + " item restrictions");
		PrintAllRestrictions();
		receivedRestrictions = true;
	}
	
	void PrintAllRestrictions() {
		foreach(PlaceRestrictionEntry entry : restrictions) {
			if (!entry)
				Print("Restriction == null !");
			else
				Print(entry.ToOutputString());
		}
	}
	
	void ConvertConfigs() {
		if (FileExist("$profile:LBmaster/itemrestrictions.json")) {
			array<ref PlaceRestrictionEntry_v1> restrictions_old = new array<ref PlaceRestrictionEntry_v1>();
			JsonFileLoader<ref array<ref PlaceRestrictionEntry_v1>>.JsonLoadFile( "$profile:LBmaster/itemrestrictions.json", restrictions_old );
			foreach (PlaceRestrictionEntry_v1 entry : restrictions_old) {
				if (!entry)
					continue;
				PlaceRestrictionEntry newEntry = new PlaceRestrictionEntry;
				newEntry.max = entry.max;
				newEntry.showOnMap = entry.showOnMap;
				newEntry.show3D = entry.show3D;
				newEntry.countItemNames.Insert(entry.itemName);
				if (entry.alternativeItemName == "")
					newEntry.kitItemNames.Insert(entry.itemName);
				else
					newEntry.kitItemNames.Insert(entry.alternativeItemName);
				newEntry.displayName = entry.itemName;
				restrictions.Insert(newEntry);
			}
			JsonFileLoader<ref array<ref PlaceRestrictionEntry>>.JsonSaveFile( "$profile:LBmaster/itemrestrictions_v2.json", restrictions );
			DeleteFile("$profile:LBmaster/itemrestrictions.json");
		} else if (!FileExist("$profile:LBmaster/itemrestrictions_v2.json")) {
			JsonFileLoader<ref array<ref PlaceRestrictionEntry>>.JsonSaveFile( "$profile:LBmaster/itemrestrictions_v2.json", restrictions );
		}
	}
	
	void ~PlaceRestrictionConfig() {
		GetDayZGame().Event_OnRPC.Remove(OnRPC);
	}
	
	void OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx) {
		if (rpc_type == 8564572) {
			if (GetGame().IsClient()) {
				restrictions.Clear();
				PlaceRestrictionEntryClient entry = new PlaceRestrictionEntryClient();
				while (ctx.Read(entry)) {
					restrictions.Insert(entry);
					entry = new PlaceRestrictionEntryClient();
				}
				if (IsItemRestrictionDebugOutput()) {
					Print("Client received " + restrictions + " item Restrictions");
					PrintAllRestrictions();
				}
				receivedRestrictions = true;
			} else if (GetGame().IsServer() && GetGame().IsMultiplayer()) {
				if (!sender)
					return;
				ScriptRPC rpc = new ScriptRPC();
				if (IsItemRestrictionDebugOutput()) {
					Print("Client requested item restrictions. Sending to client " + restrictions.Count() + " restrictions");
					PrintAllRestrictions();
				}
				foreach (PlaceRestrictionEntry entry2 : restrictions) {
					if (!entry2)
						continue;
					PlaceRestrictionEntryClient entryClient = PlaceRestrictionEntryClient.fromPlaceRestrictionEntry(entry2);
					entryClient.placed = GetPlacedItemCount(entry2, sender.GetPlainId());
					rpc.Write(entryClient);
				}
				rpc.Send(NULL, 8564572, true, sender);
			}
		}
	}
	
	PlaceRestrictionEntry GetEntryByKitItem(string type) {
		foreach (PlaceRestrictionEntry entry : restrictions) {
			if (!entry)
				continue;
			if (entry.kitItemNames.Find(type) != -1)
				return entry;
		}
		return NULL;
	}
	
	PlaceRestrictionEntry GetEntryByCountItem(string type) {
		foreach (PlaceRestrictionEntry entry : restrictions) {
			if (!entry)
				continue;
			if (entry.countItemNames.Find(type) != -1)
				return entry;
		}
		return NULL;
	}
	
	PlaceRestrictionEntry GetEntryByAny(string type) {
		PlaceRestrictionEntry entry = GetEntryByCountItem(type);
		if (!entry)
			entry = GetEntryByKitItem(type);
		return entry;
	}
	
	int GetRestrictedItemCount(string type) {
		PlaceRestrictionEntry entry = GetEntryByAny(type);
		if (!entry)
			return -1;
		return entry.max;
	}
	
	TStringArray GetKitItemNames(string itemtype) {
		TStringArray kits = new TStringArray();
		PlaceRestrictionEntry entry = GetEntryByCountItem(itemtype);
		if (!entry)
			entry = GetEntryByKitItem(itemtype);
		if (entry)
			kits.InsertAll(entry.kitItemNames);
		return kits;
	}
	
	int GetPlacedItemCount(string type, string steamid = "") {
		if (!receivedRestrictions)
			return -1;
		PlaceRestrictionEntry entry = GetEntryByCountItem(type);
		return GetPlacedItemCount(entry, steamid);
	}
	
	int GetPlacedItemCount(PlaceRestrictionEntry entry, string steamid = "") {
		if (!entry)
			return 0;
		if (GetGame().IsClient() && GetGame().IsMultiplayer()) {
			PlaceRestrictionEntryClient clientEntry;
			if (Class.CastTo(clientEntry, entry)) {
				return clientEntry.placed;
			}
			return -1;
		} else {
			if (steamid.Length() != ItemBase.BUILDER_ID_LENGTH)
				return 0;
			int count = 0;
			foreach (ItemBase item : ItemBase.itemList) {
				if (!item || item.m_builderID != steamid)
					continue;
				if (!item.cachedEntry) {
					item.cachedEntry = GetEntryByAny(item.GetType());
				}
				if (item.cachedEntry == entry)
					count++;
			}
			if (IsItemRestrictionDebugOutput())
				Print("Calculated Placed Count of " + steamid + " and Item " + entry.displayName + ": " + count);
			return count;
		}
		return -1;
	}
	
	bool CanPlaceItem(string type, string steamid = "") {
		if (ItemBase.IsPlacementAdmin(steamid))
			return true;
		PlaceRestrictionEntry entry = GetEntryByAny(type);
		if (!entry)
			return true;
		if (entry.max < 0)
			return true;
		int placed = GetPlacedItemCount(entry, steamid);
		if (placed == -1)
			return false;
		return entry.max > placed;
	}
	
	bool KitIsCounted(PlaceRestrictionEntry entry) {
		if (!entry)
			return false;
		return (entry.countItemNames.Count() == 1 && entry.kitItemNames.Count() == 1 && entry.countItemNames.Get(0) == entry.kitItemNames.Get(0));
	}
	
	bool KitIsCounted(string type) {
		PlaceRestrictionEntry entry = GetEntryByKitItem(type);
		return KitIsCounted(entry);
	}
}