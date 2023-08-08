modded class MissionGameplay
{
	
	void MissionGameplay() {
		GetItemBaseRPCHandler();
	}
	
	ref LimitationsPanel m_LimitationsPanel;
	
	override void OnOpenLimitationsPanel() {
		if (!GetGame().GetUIManager().GetMenu()) {
			if (!m_LimitationsPanel)
				m_LimitationsPanel = new LimitationsPanel();
			GetGame().GetUIManager().ShowScriptedMenu(m_LimitationsPanel, NULL);
			GetGame().GetMission().PlayerControlDisable(INPUT_EXCLUDE_INVENTORY);
			GetGame().GetGame().GetUIManager().ShowUICursor(true);
			GetGame().GetMission().GetHud().Show(false);
		} else if (GetGame().GetUIManager().GetMenu() == m_LimitationsPanel)
			GetGame().GetUIManager().HideScriptedMenu(m_LimitationsPanel);
	}
}