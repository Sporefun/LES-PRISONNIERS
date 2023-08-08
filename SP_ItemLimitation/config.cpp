////////////////////////////////////////////////////////////////////
//DeRap: config.bin
//Produced from mikero's Dos Tools Dll version 9.11
//https://mikero.bytex.digital/Downloads
//'now' is Tue Aug 08 14:40:00 2023 : 'file' last modified on Wed Nov 04 15:21:12 2020
////////////////////////////////////////////////////////////////////

#define _ARMA_

class CfgPatches
{
	class LBmaster_ItemLimitations
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data"};
	};
};
class CfgMods
{
	class LBmaster_ItemLimitations
	{
		dir = "LBmaster_ItemLimitations";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "LBmaster_ItemLimitations";
		credits = "LBmaster";
		author = "LBmaster";
		authorID = "0";
		version = "1.0";
		extra = 0;
		type = "mod";
		inputs = "LBmaster_ItemLimitations\inputsLBmasterItemLimits.xml";
		dependencies[] = {"Game","World","Mission"};
		class defs
		{
			class missionScriptModule
			{
				value = "";
				files[] = {"LBmaster_ItemLimitations/scripts/5_Mission"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"LBmaster_ItemLimitations/scripts/4_World"};
			};
			class gameScriptModule
			{
				value = "";
				files[] = {"LBmaster_ItemLimitations/scripts/3_Game"};
			};
		};
	};
};
