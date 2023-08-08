modded class MissionServer {
	
	void MissionServer() {
		GetPlaceRestrictionConfig();
		GetItemRestrictionsMainConfig();
		GetItemBaseRPCHandler();
		if (GetGame() && GetGame().IsServer() && GetItemRestrictionsMainConfig() && GetItemRestrictionsMainConfig().uninstallmode && GetItemRestrictionsMainConfig().installed) {
			string text = "Server was started in Uninstall Mode! This will delete all builders saved by the specified items ! After 5 min you are safe to shutdown the Server and Remove the Item Restrictions Mod";
			Print(text);
			PrintToRPT(text);
			DPrint(text);
			GetGame().AdminLog(text);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(AfterUninstallFinished, 300000, false);
		} else if (GetGame() && GetGame().IsServer() && GetItemRestrictionsMainConfig() && !GetItemRestrictionsMainConfig().uninstallmode && !GetItemRestrictionsMainConfig().installed) {
			text = "Server was started in Install Mode! This will alter all Items saved in the persistence files ! After 5 min you are safe to shutdown the Server and Keep the Item Restrictions Mod. Boot into uninstallmode when removing the mod !";
			Print(text);
			PrintToRPT(text);
			DPrint(text);
			GetGame().AdminLog(text);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(AfterInstallFinished, 300000, false);
		}
	}
	
	void AfterInstallFinished() {
		GetItemRestrictionsMainConfig().uninstallmode = false;
		GetItemRestrictionsMainConfig().installed = true;
		JsonFileLoader<ItemRestrictionsMainConfig>.JsonSaveFile( "$profile:LBmaster/mainconfig.json", GetItemRestrictionsMainConfig() );
			
		string text = "Server was started in Install Mode! after 5min the installmode was deactivated automaticaly";
		Print(text);
		PrintToRPT(text);
		DPrint(text);
		GetGame().AdminLog(text);
	}
	
	void AfterUninstallFinished() {
		GetItemRestrictionsMainConfig().uninstallmode = true;
		GetItemRestrictionsMainConfig().installed = false;
		JsonFileLoader<ItemRestrictionsMainConfig>.JsonSaveFile( "$profile:LBmaster/mainconfig.json", GetItemRestrictionsMainConfig() );
			
		string text = "Server was started in Uninstall Mode! after 5min the uninstallmode was deactivated automaticaly";
		Print(text);
		PrintToRPT(text);
		DPrint(text);
		GetGame().AdminLog(text);
	}
	
}