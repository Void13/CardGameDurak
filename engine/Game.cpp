#include "Game.h"

/*
	Variable: g_Deck
	Singleton of deck.

	See also:
		<CDeckOfCards>
*/
CDeckOfCards				g_Deck;

/*
	Variable: g_Game
	Singleton of game.

	See also:
		<CGameField>
*/
CGameField					g_Game;

GAME_STATE GetGameState()
{
	return g_Game.GetGameState();
}

CPlayer const *const GetMyPlayer()
{
	return g_Game.GetMyPlayer();
}

CCard const *const GetBackCard()
{ 
	return g_Deck.GetBackCard();
};

bool CanAppShowNextMoveButton()
{
	CPlayer const *const pPlayer = g_Game.GetMyPlayer();

	return (pPlayer->GetPlayerState() == PLAYER_STATE_CHOOSING_CARD && g_Game.GetGameState() == GAME_STATE_ADDING_CARD && g_Game.GetLastCoveredCardID() != 0);
}

bool CanAppShowTakeButton()
{
	CPlayer const *const pPlayer = g_Game.GetMyPlayer();
	
	return (pPlayer->GetPlayerState() == PLAYER_STATE_COVERING_CARD && g_Game.GetGameState() == GAME_STATE_ADDED_CARD);
}

HRESULT InitDeck()
{
	HRESULT hr;
	V_RETURN(g_Deck.InitDeck());

	for (int i = 0; i < CARD_SUIT_COUNT; i++)
	{
		for (int j = 0; j < CARD_RANK_COUNT; j++)
		{
			g_Deck.GetCard((CARD_SUIT)i, (CARD_RANK)j)->SetCardEndPos(CARD_SIZE);
		}
	}

	return S_OK;
}

void InitField()
{
	g_Game.InitField();
}

void ProcessAndRender()
{
	g_Game.GameProcess();
	g_Game.Render();
}

CCard *const GetTrump()
{
	return g_Game.GetTrump();
}

int GetNumFreeCards()
{
	int dwNum = 0;
	for (int i = 0; i < CARD_SUIT_COUNT; i++)
	{
		for (int j = 0; j < CARD_RANK_COUNT; j++)
		{
			if (g_Deck.GetCard((CARD_SUIT)i, (CARD_RANK)j)->GetCardState() == CARD_STATE_IN_DECK)
				dwNum++;
		}
	}

	return dwNum;
}

void DestroyDeck()
{
	g_Deck.Clear();
}

void DestroyField()
{
	g_Game.Clear();
}

void CGameField::InitField()
{
	Clear();

	for (int i = 0; i < COUNT_PLAYERS - 1; i++)
	{
		CPlayer *pTmp = new CPlayer;
		m_ppPlayers.push_back(pTmp);

		pTmp->InitPlayer((PLAYER_PLACE)i);
	}

	CPlayer *pTmp = new CPlayer;
	pTmp->InitPlayer(PLAYER_PLACE_LEFT_BOTTOM_CORNER);
	m_ppPlayers.push_back(pTmp);

	if (!GetNumFreeCards())
	{
		//m_pTrump = m_ppPlayers[COUNT_PLAYERS - 1]->GetCards().at(6 - 1);

		m_pTrump = (*(*m_ppPlayers.rbegin())->GetCards().rbegin());
	} else {
		while (!m_pTrump)
		{
			CARD_SUIT Suit = (CARD_SUIT)(rand() % CARD_SUIT_COUNT);
			CARD_RANK Rank = (CARD_RANK)(rand() % CARD_RANK_COUNT);

			if (g_Deck.GetCard(Suit, Rank)->GetCardState() == CARD_STATE_IN_DECK)
			{
				m_pTrump = const_cast<CCard *>(g_Deck.GetCard(Suit, Rank));
			}
		}
	}

	CCard *pLowerTrump = nullptr;
	int nLayingPlayer = 0;

	for (int i = 0; i < COUNT_PLAYERS; i++)
	{
		m_ppPlayers[i]->SetPlayerState(PLAYER_STATE_CHOOSING_CARD);

		vector<CCard *> Cards = m_ppPlayers[i]->GetCards();

		for (int j = 0; j < m_ppPlayers[i]->GetNumCards(); j++)
		{
			if (Cards[j]->GetCardSuit() == m_pTrump->GetCardSuit() && (!pLowerTrump || pLowerTrump->GetCardRank() > Cards[j]->GetCardRank()))
			{
				pLowerTrump = Cards[j];

				nLayingPlayer = i;
			}
		}
	}

	m_nCoversPlayer = (nLayingPlayer + 1) % COUNT_PLAYERS;

	if (!pLowerTrump)
		m_nCoversPlayer = rand() % COUNT_PLAYERS;

	m_nPushingCardPlayer = m_nCoversPlayer - 1;
	if (m_nPushingCardPlayer < 0)
		m_nPushingCardPlayer = COUNT_PLAYERS - 1;

	//m_nCoversPlayer = 0;

	(*m_ppPlayers.rbegin())->SetIsBot(false);

	m_ppPlayers[m_nCoversPlayer]->SetPlayerState(PLAYER_STATE_WAITING_NEXT_MOVE);

	m_nLastCoveredCard = 0;

	m_GameState = GAME_STATE_ADDING_CARD;
};

void CPlayer::DistributeCards()
{
	while (m_nNumCardOnHands < 6 && GetNumFreeCards() > 0)
	{
		CARD_SUIT Suit = (CARD_SUIT)(rand() % CARD_SUIT_COUNT);
		CARD_RANK Rank = (CARD_RANK)(rand() % CARD_RANK_COUNT);

		if (g_Deck.GetCard(Suit, Rank)->GetCardState() == CARD_STATE_IN_DECK)
		{
			PushCard(const_cast<CCard *>(g_Deck.GetCard(Suit, Rank)));
		}
	}
}

void CPlayer::InitPlayer(PLAYER_PLACE const _Place)
{
	Clear();

	m_Place = _Place;

	m_nNumCardOnHands = 0;

	m_PlayerState = PLAYER_STATE_INITIALIZED;

	DistributeCards();
};

/*
	Function: IsGreaterCard

	Find the greatest card.

	Parameters:
		pC1 - first card.
		pC2 - second card.

	Returns:
		Is C1 greater than C2?
*/
bool IsGreaterCard(CCard const *const pC1, CCard const *const pC2)
{
	if (pC1->GetCardSuit() == g_Game.GetTrump()->GetCardSuit())
	{
		if (pC2->GetCardSuit() == g_Game.GetTrump()->GetCardSuit())
			return pC1->GetCardRank() > pC2->GetCardRank();

		return true;
	}

	if (pC2->GetCardSuit() == g_Game.GetTrump()->GetCardSuit())
		return false;
		
	if (pC1->GetCardSuit() != pC2->GetCardSuit())
		return false;
		
	return pC1->GetCardRank() > pC2->GetCardRank();
}

void CPlayer::SortCards() const
{
	auto IsGreaterCardForSort = [](CCard const *const pC1, CCard const *const pC2) -> bool
	{
		if (pC1->GetCardSuit() == g_Game.GetTrump()->GetCardSuit())
		{
			if (pC2->GetCardSuit() == g_Game.GetTrump()->GetCardSuit())
				return pC1->GetCardRank() > pC2->GetCardRank();

			return true;
		}

		if (pC2->GetCardSuit() == g_Game.GetTrump()->GetCardSuit())
			return false;
		
		if (pC1->GetCardSuit() < pC2->GetCardSuit())
			return true;
		
		if (pC1->GetCardSuit() == pC2->GetCardSuit())
			return pC1->GetCardRank() > pC2->GetCardRank();

		return false;
	};

	for (int i = 0; i < (int)m_Cards.size(); i++)
	{
		CCard const *const *ppTrueCard = &m_Cards[i];

		for (int j = i + 1; j < (int)m_Cards.size(); j++)
		{
			if (m_Cards[j] != m_Cards[i] && IsGreaterCardForSort(m_Cards[j], *ppTrueCard))
				ppTrueCard = &m_Cards[j];
		}

		if (m_Cards[i] != *ppTrueCard)
		{
			CCard const *const pValue = m_Cards[i];
			memcpy((void *)(&m_Cards[i]), (void *)ppTrueCard, 4);
			memcpy((void *)ppTrueCard, (void *)(&pValue), 4);
		}
	}
};

void CPlayer::DrawCards() const
{
	ID3D11DeviceContext *pImmediateContext = AUTGetImmediateContext();
	ID3D11RenderTargetView *pRenderTargetView = AUTGetRenderTargetView();

	SortCards();

	static ID3D11ShaderResourceView *pSRVNull[100] = { NULL };

	CCBPerBatch *cbPerBatch = AUTGetCBPerBatch();

	// 1024, 768
	if (m_Place == PLAYER_PLACE_LEFT_UPPER_CORNER)
		cbPerBatch->vStart = XMFLOAT2(50.0f, 20.0f);
	else if (m_Place == PLAYER_PLACE_RIGHT_UPPER_CORNER)
		cbPerBatch->vStart = XMFLOAT2(400.0f, 20.0f);
	else if (m_Place == PLAYER_PLACE_RIGHT_BOTTOM_CORNER)
		cbPerBatch->vStart = XMFLOAT2(400.0f, 340.0f);
	else
		cbPerBatch->vStart = XMFLOAT2(50.0f, 340.0f);

	CCard *pOldCard = nullptr;

	cbPerBatch->vStart.x -= 30.0f;

	for (CCard *pCard : m_Cards)
	{
		if (pOldCard && pOldCard->GetCardSuit() != pCard->GetCardSuit() && m_Place == PLAYER_PLACE_LEFT_BOTTOM_CORNER)
		{
			cbPerBatch->vStart.x += 120.0f / m_Cards.size(); // 15.0f;
			cbPerBatch->vStart.y += 5.0f;
		}

		cbPerBatch->vStart.x += 240.0f / m_Cards.size(); // 30.0f;
		cbPerBatch->vStart.y += 1.5f;

		cbPerBatch->vEnd = CARD_SIZE;

		pCard->SetCardStartPos(cbPerBatch->vStart);

		//AUTSetCBPerBatch(cbPerBatch);
		AUTUpdateD3DCBPerBatch(pImmediateContext);

		ID3D11ShaderResourceView *pSRV = (m_Place != PLAYER_PLACE_LEFT_BOTTOM_CORNER)?GetBackCard()->GetSRV():pCard->GetSRV();

		pImmediateContext->PSSetShaderResources(0, 1, &pSRV);
		pImmediateContext->Draw(1, 0);
		pImmediateContext->PSSetShaderResources(0, 1, pSRVNull);

		pOldCard = pCard;
	}
};

void CGameField::Render() const
{
	ID3D11DeviceContext *pImmediateContext = AUTGetImmediateContext();
	ID3D11RenderTargetView *pRenderTargetView = AUTGetRenderTargetView();

	static ID3D11ShaderResourceView *pSRVNull[100] = { NULL };
	static ID3D11RenderTargetView *pRTVNull[100] = { NULL };

	pImmediateContext->OMSetRenderTargets(1, &pRenderTargetView, NULL);

	AUTSetPShader(AUTMapIDsGet("PS_CardRenderer"), pImmediateContext);
	AUTSetGShader(AUTMapIDsGet("GS_CardRenderer"), pImmediateContext);
	AUTSetVShader(AUTMapIDsGet("VS_CardRenderer"), pImmediateContext);

	AUTSetPSSampler(AUTMapIDsGet("SamplerAnisotropic"), 0, pImmediateContext);

	pImmediateContext->IASetInputLayout(NULL);
	pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	CCBPerBatch *cbPerBatch = AUTGetCBPerBatch();

	for (CCard *pCard : m_CardsOnField)
	{
		cbPerBatch->vStart = *pCard->GetCardStartPos();
		cbPerBatch->vEnd = CARD_SIZE;
		AUTUpdateD3DCBPerBatch(pImmediateContext);

		ID3D11ShaderResourceView *pSRV = pCard->GetSRV();
		pImmediateContext->PSSetShaderResources(0, 1, &pSRV);
		pImmediateContext->Draw(1, 0);
		pImmediateContext->PSSetShaderResources(0, 1, pSRVNull);
	}

	for (CCard *pCard : m_CoverCardsOnField)
	{
		cbPerBatch->vStart = *pCard->GetCardStartPos();
		cbPerBatch->vEnd = CARD_SIZE;
		AUTUpdateD3DCBPerBatch(pImmediateContext);

		ID3D11ShaderResourceView *pSRV = pCard->GetSRV();
		pImmediateContext->PSSetShaderResources(0, 1, &pSRV);
		pImmediateContext->Draw(1, 0);
		pImmediateContext->PSSetShaderResources(0, 1, pSRVNull);
	}

	for (CPlayer *const pPlayer : m_ppPlayers)
	{
		pPlayer->DrawCards();
		pPlayer->CheckLife();
	}

	pImmediateContext->PSSetShaderResources(0, 1, pSRVNull);
	pImmediateContext->OMSetRenderTargets(1, pRTVNull, NULL);
	pImmediateContext->PSSetShader(NULL, NULL, 0);
	pImmediateContext->GSSetShader(NULL, NULL, 0);
	pImmediateContext->VSSetShader(NULL, NULL, 0);
};

/*
	
	Ѕаги:

	- «абирает карты и не даЄт ещЄ подкинуть(или фича?)

*/

void CGameField::GameProcess()
{
	vector<CPlayer *> &pPlayers = m_ppPlayers;

	auto _GetNextPlayerID = [&pPlayers](int _nPlayerID) -> int
	{
		int nResultID = (_nPlayerID + 1) % pPlayers.size();

		while (pPlayers[nResultID]->GetPlayerState() == PLAYER_STATE_LEFT)
		{
			nResultID = (nResultID + 1) % pPlayers.size();
		}

		return nResultID;
	};

	switch(m_GameState)
	{
	case GAME_STATE_ADDING_CARD:
		{
			if (m_ppPlayers[m_nPushingCardPlayer]->GetPlayerState() == PLAYER_STATE_WAITING_NEXT_MOVE)
			{
				m_GameState = GAME_STATE_COVERED_CARD;
				break;
			}

			bool bUseIsHere = !m_CardsOnField.empty() || !m_CoverCardsOnField.empty();

			CPlayer *const pPlayer = m_ppPlayers[m_nPushingCardPlayer];
			//CCard const *const pChoosedCard = pPlayer->GetChoosedCard();
			CCard *pChoosedCard = pPlayer->GetChoosedCard();

			vector<CCard *> &TCards1 = m_CardsOnField;
			vector<CCard *> &TCards2 = m_CoverCardsOnField;

			auto _IsCardOnField = [&TCards1, &TCards2](CCard const *const _pCard) -> bool
			{
				for (CCard *pCard : TCards1)
				{
					if (pCard->GetCardRank() == _pCard->GetCardRank())
					{
						return true;
					}
				}

				for (CCard *pCard : TCards2)
				{
					if (pCard->GetCardRank() == _pCard->GetCardRank())
					{
						return true;
					}
				}

				return false;
			};

			// карты нет на поле с такой мастью - выйти
			if (!m_ppPlayers[m_nPushingCardPlayer]->IsBot())
				if ((pPlayer->GetPlayerState() != PLAYER_STATE_CHOOSED_CARD) || (bUseIsHere && !_IsCardOnField(pChoosedCard)))
					break;

			if (m_ppPlayers[m_nPushingCardPlayer]->IsBot())
			{
				vector<CCard *> const Cards = pPlayer->GetCards();

				pChoosedCard = nullptr;

				for (CCard *pCard : Cards)
				{
					if (pCard->GetCardSuit() == m_pTrump->GetCardSuit())
					{
						pChoosedCard = pCard;
						break;
					}
				}
				
				if (!pChoosedCard)
				{
					for (CCard *pCard : Cards)
					{
						if ((!pChoosedCard || pCard->GetCardRank() > pChoosedCard->GetCardRank()) && (!bUseIsHere || _IsCardOnField(pCard)))
							pChoosedCard = pCard;
					}
				} else {
					pChoosedCard = nullptr;

					for (CCard *pCard : Cards)
					{
						if ((!pChoosedCard || pCard->GetCardRank() < pChoosedCard->GetCardRank()) && pCard->GetCardSuit() != m_pTrump->GetCardSuit() && (!bUseIsHere || _IsCardOnField(pCard)))
							pChoosedCard = pCard;
					}
				}
			} else {
				pChoosedCard = pPlayer->GetChoosedCard();
			}

			if (!pChoosedCard)
			{
				pPlayer->SetPlayerState(PLAYER_STATE_WAITING_NEXT_MOVE);

				m_GameState = GAME_STATE_COVERED_CARD;

				break;
			}

			pPlayer->RemoveCard(pChoosedCard);

			pChoosedCard->SetPlayer(nullptr);
			pChoosedCard->SetCardState(CARD_STATE_ON_FIELD);
			pChoosedCard->SetCardOldStartPos(*pChoosedCard->GetCardStartPos());
			pChoosedCard->SetCardStartPos(XMFLOAT2(200.0f + m_nLastCoveredCard * 100.0f, 180.0f + m_nLastCoveredCard * 0.0f));

			m_AnimQueue.push(pChoosedCard);

			m_CardsOnField.push_back(pChoosedCard);

			for (CPlayer *const pPlayer1 : m_ppPlayers)
			{
				pPlayer1->SetPlayerState(PLAYER_STATE_CHOOSING_CARD);
			}

			m_ppPlayers[m_nCoversPlayer]->SetPlayerState(PLAYER_STATE_COVERING_CARD);

			m_GameState = GAME_STATE_ANIMATING;

			m_OldGameState = GAME_STATE_ADDED_CARD;

			if (pPlayer->GetNumCards() <= 0 && GetNumFreeCards() == 0)
			{
				pPlayer->SetPlayerState(PLAYER_STATE_LEFT);

				m_ppPlayers[m_nPushingCardPlayer] = nullptr;
				m_ppPlayers.erase(m_ppPlayers.begin() + m_nPushingCardPlayer);

				if (m_ppPlayers.size() == 1)
				{
					// вдул. лох
					m_GameState = GAME_STATE_FINISHED;
				}
				else if (m_ppPlayers.size() == 0)
				{
					// вроде ничь€
					m_GameState = GAME_STATE_FINISHED;
				}
			}

			break;
		}
	case GAME_STATE_ADDED_CARD:
		{
			CPlayer *const pPlayer = m_ppPlayers[m_nCoversPlayer];
			CCard *pChoosedCard = pPlayer->GetChoosedCard();

			// выбираем карту, которой будем покрывать карту, лежащую на столе
			if (m_ppPlayers[m_nCoversPlayer]->IsBot() && pPlayer->GetPlayerState() != PLAYER_STATE_TAKING_CARDS_FROM_FIELD)
			{
				for (CCard *pCard : pPlayer->GetCards())
				{
					if (pCard->GetCardSuit() == m_pTrump->GetCardSuit() && 
						m_CardsOnField[m_nLastCoveredCard]->GetCardSuit() != m_pTrump->GetCardSuit() && 
						m_CardsOnField[m_nLastCoveredCard]->GetCardRank() == pCard->GetCardRank())
					{
						pChoosedCard = pCard;
						break;
					}

					if (IsGreaterCard(pCard, m_CardsOnField[m_nLastCoveredCard]) && (!pChoosedCard || IsGreaterCard(pChoosedCard, pCard)))
						pChoosedCard = pCard;
				}

				if (!pChoosedCard)
				{
					m_GameState = GAME_STATE_TAKING_CARDS_FROM_FIELD;

					pPlayer->SetPlayerState(PLAYER_STATE_TAKING_CARDS_FROM_FIELD);

					for (CPlayer *const pPlayer1 : m_ppPlayers)
					{
						pPlayer->SetPlayerState(PLAYER_STATE_WAITING_NEXT_MOVE);
					}

					break;
				}

				pChoosedCard->SetPlayer(nullptr);
			}

			// если это игрок и он выбрал карту
			// если это любой чувак и он покрывает

			if (!m_ppPlayers[m_nCoversPlayer]->IsBot() && pPlayer->GetPlayerState() == PLAYER_STATE_CHOOSED_CARD || 
				m_ppPlayers[m_nCoversPlayer]->IsBot() && pPlayer->GetPlayerState() == PLAYER_STATE_COVERING_CARD)
			{
				// ≈сли выбранна€ карта больше той, котора€ на поле
				if (IsGreaterCard(pChoosedCard, m_CardsOnField[m_nLastCoveredCard]))
				{
					pPlayer->RemoveCard(pChoosedCard);

					pChoosedCard->SetCardState(CARD_STATE_ON_FIELD);
					pChoosedCard->SetCardOldStartPos(*pChoosedCard->GetCardStartPos());
					pChoosedCard->SetCardStartPos(XMFLOAT2(215.0f + m_nLastCoveredCard * 100.0f, 200.0f + m_nLastCoveredCard * 0.0f));

					m_AnimQueue.push(pChoosedCard);

					m_nLastCoveredCard++;

					m_CoverCardsOnField.push_back(pChoosedCard);
	
					pPlayer->SetPlayerState(PLAYER_STATE_WAITING_NEXT_MOVE);

					m_GameState = GAME_STATE_ANIMATING;

					m_OldGameState = GAME_STATE_COVERED_CARD;

					if (pPlayer->GetNumCards() <= 0 && GetNumFreeCards() == 0)
					{
						pPlayer->SetPlayerState(PLAYER_STATE_LEFT);

						m_ppPlayers[m_nCoversPlayer] = nullptr;
						m_ppPlayers.erase(m_ppPlayers.begin() + m_nCoversPlayer);

						m_nCoversPlayer %= m_ppPlayers.size();

						if (m_ppPlayers.size() == 1)
						{
							// вдул. лох
							m_GameState = GAME_STATE_FINISHED;
						}
						else if (m_ppPlayers.size() == 0)
						{
							// вроде ничь€
							m_GameState = GAME_STATE_FINISHED;
						}

						m_OldGameState = GAME_STATE_GETTING_CARDS_FROM_DECK;
					}
				} else {
					pChoosedCard->GetPlayer()->SetPlayerState(PLAYER_STATE_COVERING_CARD);

					pChoosedCard = nullptr;
				}
			}
			else if (pPlayer->GetPlayerState() == PLAYER_STATE_TAKING_CARDS_FROM_FIELD)
			{
				CPlayer *const pPlayer = m_ppPlayers[m_nCoversPlayer];

				for (CCard *pCard : m_CardsOnField)
				{
					pPlayer->PushCard(pCard);

					pCard->SetCardOldStartPos(*pCard->GetCardStartPos());
					pCard->SetCardStartPos(*pPlayer->GetCards().at(pPlayer->GetNumCards() - 1)->GetCardStartPos());
					m_AnimQueue.push(pCard);

					pCard = nullptr;
				}

				m_CardsOnField.clear();

				for (CCard *pCard : m_CoverCardsOnField)
				{
					pPlayer->PushCard(pCard);

					pCard->SetCardOldStartPos(*pCard->GetCardStartPos());
					pCard->SetCardStartPos(*pPlayer->GetCards().at(pPlayer->GetNumCards() - 1)->GetCardStartPos());
					m_AnimQueue.push(pCard);

					pCard = nullptr;
				}

				m_CoverCardsOnField.clear();

				m_nLastCoveredCard = 0;

				m_nCoversPlayer = _GetNextPlayerID(m_nCoversPlayer);

				m_GameState = GAME_STATE_ANIMATING;

				m_OldGameState = GAME_STATE_GETTING_CARDS_FROM_DECK;
			}

			break;
		}
	case GAME_STATE_COVERED_CARD:
		{
			if (m_CardsOnField.size() < 6 && m_ppPlayers[m_nCoversPlayer]->GetNumCards() > 0)
			{
				int nPushingCardPlayer = 0;

				for (CPlayer *const pPlayer : m_ppPlayers)
				{
					if (pPlayer->GetPlayerState() != PLAYER_STATE_WAITING_NEXT_MOVE)
					{
						int nNextPlayer = _GetNextPlayerID(nPushingCardPlayer);
						int nPrevPlayer = _GetNextPlayerID(m_nCoversPlayer);

						if (nNextPlayer == m_nCoversPlayer || nPrevPlayer == nPushingCardPlayer)
						{
							m_nPushingCardPlayer = nPushingCardPlayer;

							m_GameState = GAME_STATE_ADDING_CARD;

							return;
						} else {
							pPlayer->SetPlayerState(PLAYER_STATE_WAITING_NEXT_MOVE);
						}
					}

					nPushingCardPlayer++;
				}
			}

			m_GameState = GAME_STATE_GETTING_CARDS_FROM_DECK;

			break;
		}
	case GAME_STATE_TAKING_CARDS_FROM_FIELD:
		{
			CPlayer *const pPlayer = m_ppPlayers[m_nCoversPlayer];

			for (CCard *pCard : m_CardsOnField)
			{
				pPlayer->PushCard(pCard);

				pCard->SetCardOldStartPos(*pCard->GetCardStartPos());
				pCard->SetCardStartPos(*pPlayer->GetCards().at(pPlayer->GetNumCards() - 1)->GetCardStartPos());
				m_AnimQueue.push(pCard);

				pCard = nullptr;
			}

			m_CardsOnField.clear();

			for (CCard *pCard : m_CoverCardsOnField)
			{
				pPlayer->PushCard(pCard);

				pCard->SetCardOldStartPos(*pCard->GetCardStartPos());
				pCard->SetCardStartPos(*pPlayer->GetCards().at(pPlayer->GetNumCards() - 1)->GetCardStartPos());
				m_AnimQueue.push(pCard);

				pCard = nullptr;
			}

			m_CoverCardsOnField.clear();

			m_nLastCoveredCard = 0;

			m_nCoversPlayer = _GetNextPlayerID(m_nCoversPlayer);

			m_GameState = GAME_STATE_ANIMATING;

			m_OldGameState = GAME_STATE_GETTING_CARDS_FROM_DECK;

			break;
		}
	case GAME_STATE_GETTING_CARDS_FROM_DECK:
		{
			for (CCard *pCard : m_CardsOnField)
			{
				pCard->SetCardOldStartPos(*pCard->GetCardStartPos());
				pCard->SetCardStartPos(XMFLOAT2(500.0f, 250.0f));
				m_AnimQueue.push(pCard);

				pCard->SetCardState(CARD_STATE_OUT_OF_GAME);
				pCard->SetPlayer(nullptr);
				pCard = nullptr;
			}

			m_CardsOnField.clear();

			for (CCard *pCard : m_CoverCardsOnField)
			{
				pCard->SetCardOldStartPos(*pCard->GetCardStartPos());
				pCard->SetCardStartPos(XMFLOAT2(500.0f, 250.0f));
				m_AnimQueue.push(pCard);

				pCard->SetCardState(CARD_STATE_OUT_OF_GAME);
				pCard->SetPlayer(nullptr);
				pCard = nullptr;
			}

			m_CoverCardsOnField.clear();

			m_nLastCoveredCard = 0;

			m_nPushingCardPlayer = m_nCoversPlayer;

			m_nCoversPlayer = _GetNextPlayerID(m_nCoversPlayer);

			for (CPlayer *const pPlayer : m_ppPlayers)
			{
				pPlayer->DistributeCards();
				pPlayer->SetPlayerState(PLAYER_STATE_CHOOSING_CARD);
			}

			m_ppPlayers[m_nCoversPlayer]->SetPlayerState(PLAYER_STATE_WAITING_NEXT_MOVE);

			m_GameState = GAME_STATE_ANIMATING;

			m_OldGameState = GAME_STATE_ADDING_CARD;

			break;
		}
	case GAME_STATE_ANIMATING:
		{
			if (m_AnimQueue.empty())
			{
				m_GameState = m_OldGameState;

				break;
			}

			CCard *const pCard = m_AnimQueue.front();
			static int nStepsCount = (int)(AUTGetFPS() * 0.5f);
			static XMFLOAT2 vNewPos = *pCard->GetCardStartPos();

			if (m_nStep == 0)
			{
				vNewPos = *pCard->GetCardStartPos();
				nStepsCount = (int)(AUTGetFPS() * 0.5f);
			}

			if (nStepsCount == 0)
				nStepsCount = 50;

			XMFLOAT2 vTempNewPos;
			XMStoreFloat2(&vTempNewPos, XMVectorAdd(XMLoadFloat2(pCard->GetCardOldStartPos()), XMVectorScale(XMVectorSubtract(XMLoadFloat2(&vNewPos), XMLoadFloat2(pCard->GetCardOldStartPos())), (float)m_nStep / nStepsCount)));

			pCard->SetCardStartPos(vTempNewPos);

			if (m_nStep == nStepsCount)
			{
				m_nStep = 0;

				m_AnimQueue.pop();

				if (m_AnimQueue.empty())
					m_GameState = m_OldGameState;
			} else
				m_nStep++;

			break;
		}
	default:
		break;
	}
}

void CGameField::ProcessPlayerInput(XMFLOAT2 const &_vPos, bool const _bTaking, bool const _bNextMove)
{
	(*m_ppPlayers.rbegin())->ProcessPlayerInput(_vPos, _bTaking, m_nLastCoveredCard == 0);
}

void ProcessPlayerInput(XMFLOAT2 const &_vPos, bool const _bTaking, bool const _bNextMove)
{
	return g_Game.ProcessPlayerInput(_vPos, _bTaking, _bNextMove);
}

void CPlayer::ProcessPlayerInput(XMFLOAT2 const &_vPos, bool const _bTaking, bool const _bNextMove)
{
	if (_bTaking)
	{
		if (m_PlayerState == PLAYER_STATE_CHOOSING_CARD && !_bNextMove)
			m_PlayerState = PLAYER_STATE_WAITING_NEXT_MOVE;
		else if (m_PlayerState == PLAYER_STATE_COVERING_CARD)
			m_PlayerState = PLAYER_STATE_TAKING_CARDS_FROM_FIELD;
	}

	//if (_bNextMove)
	//	m_PlayerState = PLAYER_STATE_WAITS_NEXT_MOVE;

	if (m_PlayerState != PLAYER_STATE_CHOOSING_CARD && m_PlayerState != PLAYER_STATE_COVERING_CARD)
		return;

	//SortCards();

	for (int i = (int)m_Cards.size() - 1; i >= 0; i--)
	{
		CCard *const pCard = m_Cards[i];
		XMFLOAT2 const *const vStartPos = pCard->GetCardStartPos();
		XMFLOAT2 vEndPos = *vStartPos;
		XMStoreFloat2(&vEndPos, XMVectorAdd(XMLoadFloat2(&vEndPos), XMLoadFloat2(pCard->GetCardEndPos())));

		if (vStartPos->x <= _vPos.x && vEndPos.x >= _vPos.x && vStartPos->y <= _vPos.y && vEndPos.y >= _vPos.y)
		{
			m_PlayerState = PLAYER_STATE_CHOOSED_CARD;
			m_pChoosedCard = pCard;

			return;
		}
	}
}