#include "Cards.h"
#include "Game.h"

HRESULT CCard::Init(CARD_SUIT const _Suit, CARD_RANK const _Rank)
{
	Clear();

	m_Suit =_Suit;
	m_Rank = _Rank; 
	m_State = CARD_STATE_IN_DECK;

	HRESULT hr;
		
	wchar_t sFile[128];
	wsprintf(sFile, L"data\\models\\images\\1193080942_%d-%d.png", CARD_SUIT_COUNT - m_Suit, CARD_RANK_COUNT - m_Rank - 1);

	V_RETURN(D3DX11CreateShaderResourceViewFromFile(AUTGetD3DDevice(), sFile, NULL, NULL, &m_pSRV, &hr));

	return S_OK;
};

void CCard::Clear()
{
	m_pPlayer = nullptr;
	SAFE_RELEASE(m_pSRV);
}