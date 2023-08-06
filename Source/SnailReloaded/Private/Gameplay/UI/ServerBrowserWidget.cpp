// SnailReloaded - ThoughtOverflow 2023 - All rights reserved.


#include "Gameplay/UI/ServerBrowserWidget.h"

#include "Gameplay/UI/ServerEntryWidget.h"

FServerEntry::FServerEntry(): MaxPlayers(0), CurrentPlayers(0), bAuthenticatedSession(false), ServerPing(0)
{
}

void UServerBrowserWidget::RefreshBrowserWidget()
{
	ScrollBox->ClearChildren();
	for(auto& Server : Servers)
	{
		UServerEntryWidget* ServerEntryWidget = CreateWidget<UServerEntryWidget>(GetOwningPlayer(), ServerEntryClass);
		ServerEntryWidget->Auth = Server.bAuthenticatedSession;
		ServerEntryWidget->SessionName = FName(Server.ServerName);
		ServerEntryWidget->MaxPlayers = Server.MaxPlayers;
		ServerEntryWidget->CurrentPlayers = Server.CurrentPlayers;
		ServerEntryWidget->Ping = Server.ServerPing;
		ServerEntryWidget->SearchResult = Server.OnlineSessionSearchResult;
		ScrollBox->AddChild(ServerEntryWidget);
	}
}
