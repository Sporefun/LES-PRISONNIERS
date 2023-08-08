static ref ItemRestrictionsMainConfig g_ItemRestrictionsMainConfig;

static ItemRestrictionsMainConfig GetItemRestrictionsMainConfig() {
	if (!g_ItemRestrictionsMainConfig) {
		g_ItemRestrictionsMainConfig = new ItemRestrictionsMainConfig();
		if (!FileExist("$profile:LBmaster")) {
			MakeDirectory("$profile:LBmaster");
			Print("Created Directory for Item Restrictions");
		} else {
			Print("Directory for Item Restrictions already exists");
		}
		if (!FileExist("$profile:LBmaster/mainconfig.json")) {
			JsonFileLoader<ItemRestrictionsMainConfig>.JsonSaveFile( "$profile:LBmaster/mainconfig.json", g_ItemRestrictionsMainConfig );
		} else {
			JsonFileLoader<ItemRestrictionsMainConfig>.JsonLoadFile( "$profile:LBmaster/mainconfig.json", g_ItemRestrictionsMainConfig );
			JsonFileLoader<ItemRestrictionsMainConfig>.JsonSaveFile( "$profile:LBmaster/mainconfig.json", g_ItemRestrictionsMainConfig );
		}
	}
	return g_ItemRestrictionsMainConfig;
}
class ItemRestrictionsMainConfig
{
	bool hasMapWidgetShown = true;
	bool canOpenIngamePanel = true;
	bool canSeePlayerMarkerOnPanelMap = true;
	bool canSeePlacedItemsMarkersOnPanelMap = true;
	bool canSeePlacedItemsMarkers3D = true;
	bool canSeeDistanceOf3DMarkers = true;
	float marker3DDistanceToDisplay = 100.0;
	bool disableCrateCraftingRecipe = false;
	bool uninstallmode = false;
	bool installed = false;
	bool debugOutput = false;
	bool debugOutputClient = false;
	
	ref array<string> ignoreRestrictionsSteamIds = new array<string>();
	
	ItemRestrictionsMainConfigClient GetClientConfigDefault() {
		ItemRestrictionsMainConfigClient conf = new ItemRestrictionsMainConfigClient();
		conf.canOpenIngamePanel = this.canOpenIngamePanel;
		conf.hasMapWidgetShown = this.hasMapWidgetShown;
		conf.canSeePlayerMarkerOnPanelMap = this.canSeePlayerMarkerOnPanelMap;
		conf.canSeePlacedItemsMarkersOnPanelMap = this.canSeePlacedItemsMarkersOnPanelMap;
		conf.canSeePlacedItemsMarkers3D = this.canSeePlacedItemsMarkers3D;
		conf.disableCrateCraftingRecipe = this.disableCrateCraftingRecipe;
		conf.ignoreRestrictions = false;
		conf.debugOutput = this.debugOutputClient;
		return conf;
	}
	
	ItemRestrictionsMainConfigClient GetClientConfig(string steamid) {
		ItemRestrictionsMainConfigClient conf = GetClientConfigDefault();
		conf.ignoreRestrictions = (ignoreRestrictionsSteamIds != NULL && ignoreRestrictionsSteamIds.Find(steamid) != -1)
		return conf;
	}
}
class ItemRestrictionsMainConfigClient
{
	bool hasMapWidgetShown = true;
	bool canOpenIngamePanel = true;
	bool canSeePlayerMarkerOnPanelMap = true;
	bool canSeePlacedItemsMarkersOnPanelMap = true;
	bool canSeePlacedItemsMarkers3D = true;
	bool disableCrateCraftingRecipe = false;
	bool ignoreRestrictions = false;
	bool debugOutput = false;
}