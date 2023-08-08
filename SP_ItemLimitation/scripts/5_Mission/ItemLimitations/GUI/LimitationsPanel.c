class LimitationsPanel: UIScriptedMenu {
	
	private MapWidget map_widget;
	WrapSpacerWidget contentWidget;
	
	bool initialized = false;
	int updateCounter = 0;
	
	override Widget Init() {
		if (!initialized) {
			Print("Limits Panel instance Initialized");
			if (GetItemRestrictionsMainConfigClient() && GetItemRestrictionsMainConfigClient().hasMapWidgetShown) {
				layoutRoot = GetGame().GetWorkspace().CreateWidgets("LBmaster_ItemLimitations\\gui\\layouts\\restrictionsmenu.layout");
			} else {
				layoutRoot = GetGame().GetWorkspace().CreateWidgets("LBmaster_ItemLimitations\\gui\\layouts\\restrictionsmenunomap.layout");
			}
			map_widget = MapWidget.Cast(layoutRoot.FindAnyWidget("map"));
			contentWidget = WrapSpacerWidget.Cast(layoutRoot.FindAnyWidget("leftpanel"));
			initialized = true;
		}
		CenterMapAtPlayerPos();
		InitWrapSpacer();
		UpdateMarker();
		return layoutRoot;
	}
	
	void CenterMapAtPlayerPos() {
		if (!GetItemRestrictionsMainConfigClient() || !GetItemRestrictionsMainConfigClient().canSeePlayerMarkerOnPanelMap)
			return;
		if (GetGame().GetPlayer() && GetGame().GetPlayer() && GetGame().GetPlayer().GetPosition() && map_widget) {
			float moveX = 1000;
			float moveZ = 650;
			
			vector playerPos = GetGame().GetPlayer().GetPosition();
			vector mapPos = Vector(playerPos[0] + moveX, 0, playerPos[2] - moveZ);
			
			map_widget.SetScale(0.2);
			map_widget.SetMapPos(mapPos);
		}
	}
	
	void InitWrapSpacer() {
		if (!contentWidget) {
			Print("Limits List not found !");
			return;
		}
		Widget child = contentWidget.GetChildren();
		while (child) {
			contentWidget.RemoveChild(child);
			child.Unlink();
			child = contentWidget.GetChildren();
		}
		array<ref PlaceRestrictionEntry> restrictions = GetPlaceRestrictionConfig().restrictions;
		if (!restrictions) {
			Print("Restrictions not found !");
			return;
		}
		if (IsItemRestrictionDebugOutput())
			Print("Restrictions has " + restrictions.Count() + " entries");
		if (ItemBase.IsPlacementAdmin("")) {
			TextWidget text = TextWidget.Cast(GetGame().GetWorkspace().CreateWidgets("LBmaster_ItemLimitations\\gui\\layouts\\itemrestrictionsentry.layout", contentWidget));
			text.SetText("Admin Mode");
			text.SetColor(ARGB(255,255,20,0));
		}
		foreach (PlaceRestrictionEntry entry : restrictions) {
			PlaceRestrictionEntryClient clientEntry;
			if (!Class.CastTo(clientEntry, entry))
				continue;
			text = TextWidget.Cast(GetGame().GetWorkspace().CreateWidgets("LBmaster_ItemLimitations\\gui\\layouts\\itemrestrictionsentry.layout", contentWidget));
			if (text) {
				string displayname = clientEntry.displayName;
				int placed = clientEntry.placed;
				int max = clientEntry.max;
				if (IsItemRestrictionDebugOutput())
					Print("Adding PlaceList: " + displayname + " " + placed + " " + max);
				string placeText = "(?/?)";
				if (placed >= 0 && max >= 0) {
					placeText = "(" + placed + "/" + max + ")";
					if (placed < max)
						text.SetColor(ARGB(255, 135, 255, 0));
					else if (placed == max)
						text.SetColor(ARGB(255, 255, 180, 0));
					else if (placed > max)
						text.SetColor(ARGB(255, 255, 20, 0));
				}
				text.SetText(displayname + " " + placeText);
			} else {
				Print("Unable to Create TextWidget");
			}
		}
		if (restrictions.Count() == 0) {
			text = TextWidget.Cast(GetGame().GetWorkspace().CreateWidgets("LBmaster_ItemLimitations\\gui\\layouts\\itemrestrictionsentry.layout", contentWidget));
			if (text) {
				text.SetText("No Limits yet");
			}
		}
	}
	
	string GetItemDisplayName(string itemname) {
		if (!itemname || itemname.Length() == 0)
			return "Unknown Item !";
		string path = "CfgVehicles " + itemname + " displayName";
		string text = "";
		if (GetGame() && GetGame().ConfigGetText(path, text)) {
			return text;
		}
		return "(Unknown) " + itemname;
	}
	
	void AddMarker(MapWidget map_widget) {
		if (!map_widget)
			return;
		if (!ItemBase.placedPositions)
			return;
		if (!GetItemRestrictionsMainConfigClient() || !GetItemRestrictionsMainConfigClient().canSeePlacedItemsMarkersOnPanelMap)
			return;
		for (int i = 0; i < ItemBase.placedPositions.Count(); i++) {
			Param2<string, ref vector> parm = ItemBase.placedPositions.Get(i);
			string displayname = GetItemDisplayName(parm.param1);
			if (!parm)
				continue;
			map_widget.AddUserMark(parm.param2, displayname, ARGB(255, 238, 77, 46), "DZ\\gear\\navigation\\data\\map_tree_ca.paa");
		}
	}
	
	override void Update( float timeslice ) {
		super.Update(timeslice);
		
		updateCounter++;
		updateCounter = updateCounter % 100;
		if (updateCounter == 0) {
			UpdateMarker();
		}
		
		if( GetGame() && GetGame().GetInput() && GetGame().GetInput().LocalPress("UAUIBack", false) ) {
			GetGame().GetUIManager().HideScriptedMenu(this);
		}
	}

    void UnlockControls() {
        GetGame().GetMission().PlayerControlEnable(false);
        //GetGame().GetInput().ResetGameFocus();
        GetGame().GetUIManager().ShowUICursor(false);
        GetGame().GetMission().GetHud().Show(true);
    }
	
	override void OnHide() {
		super.OnHide();
		UnlockControls();
	}
	
	override void OnShow() {
		super.OnShow();
		InitWrapSpacer();
	}
	
	void UpdateMarker() {
		if (!map_widget)
			return;
		ClearMarker();
		AddMarker(map_widget);
		AddPlayerMarker();
	}
	
	void ClearMarker() {
		if (map_widget) {
			map_widget.ClearUserMarks();
		}
	}
	
	void AddPlayerMarker() {
		if (!GetItemRestrictionsMainConfigClient() || !GetItemRestrictionsMainConfigClient().canSeePlayerMarkerOnPanelMap)
			return;
		Man player = GetGame().GetPlayer();
		if (map_widget && player) {
			map_widget.AddUserMark(player.GetPosition(), "Me", ARGB(255, 238, 77, 46), "DZ\\gear\\navigation\\data\\map_tree_ca.paa");
		}
	}
	
}