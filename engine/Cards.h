#ifndef H__CARDS
#define H__CARDS

#include "AUT.h"

// Constant: USE_36_DECK
// Causes the size of deck

//#define USE_52_DECK
#define USE_36_DECK

/*
	Enum: CARD_SUIT

	Enumerates card suits.

	CARD_SUIT_DIAMOND	- Causes the diamond suit.
	CARD_SUIT_HEART		- Causes the heart suit.
	CARD_SUIT_SPADE		- Causes the spade suit.
	CARD_SUIT_CLUB		- Causes the club suit.
	CARD_SUIT_COUNT		- Causes count of suits.
*/
enum CARD_SUIT
{
	CARD_SUIT_DIAMOND = 0,
	CARD_SUIT_HEART,
	CARD_SUIT_SPADE,	// крести
	CARD_SUIT_CLUB,		// пики
	CARD_SUIT_COUNT
};

/*
	Enum: CARD_RANK

	Enumerates card ranks.

	CARD_RANK_2			- Causes card with rank 2.
	CARD_RANK_3			- Causes card with rank 3.
	CARD_RANK_4			- Causes card with rank 4.
	CARD_RANK_5			- Causes card with rank 5.
	CARD_RANK_6			- Causes card with rank 6.
	CARD_RANK_7			- Causes card with rank 7.
	CARD_RANK_8			- Causes card with rank 8.
	CARD_RANK_9			- Causes card with rank 9.
	CARD_RANK_10		- Causes card with rank 10.
	CARD_RANK_JACK		- Causes card with rank jack.
	CARD_RANK_QUEEN		- Causes card with rank quenn.
	CARD_RANK_KING		- Causes card with rank king.
	CARD_RANK_ACE		- Causes card with rank ace.
	CARD_RANK_COUNT		- Causes number of ranks.
*/
enum CARD_RANK
{
#ifdef USE_52_DECK
	CARD_RANK_2 = 0,
	CARD_RANK_3,
	CARD_RANK_4,
	CARD_RANK_5,
	CARD_RANK_6,
#elif defined(USE_36_DECK)
	CARD_RANK_6 = 0,
#endif
	CARD_RANK_7,
	CARD_RANK_8,
	CARD_RANK_9,
	CARD_RANK_10,
	CARD_RANK_JACK,
	CARD_RANK_QUEEN,
	CARD_RANK_KING,
	CARD_RANK_ACE,
	CARD_RANK_COUNT
};

/*
	Enum: CARD_STATE

	Enumerates card states.

	CARD_STATE_ON_HANDS		- Causes card state on hands.
	CARD_STATE_IN_DECK		- Causes card state in deck.
	CARD_STATE_ON_FIELD		- Causes card state on field.
	CARD_STATE_OUT_OF_GAME	- Causes card state out of game.
*/
enum CARD_STATE
{
	CARD_STATE_ON_HANDS = 0,
	CARD_STATE_IN_DECK,
	CARD_STATE_ON_FIELD,
	CARD_STATE_OUT_OF_GAME
};

/*
	Class: CCard
	
	Describes a card in game.
*/

class CPlayer;

class CCard
{
public:
	/*
		Function: Clear

			Clears content of a card.

		Parameters:

		Returns:
	*/
	void Clear();

	/*
		Constructor: CCard
		Initializes a card and clears it.
	*/
	CCard() 
	{
		Clear();
	}

	/*
		Destructor: CCard
		Clears a card.
	*/
	~CCard() 
	{ 
		Clear();
	}

	/*
		Function: Init

		Initializes a card(loads texture from file).

		Parameters:
			_Suit - suit of card.
			_Rank - rank of card.

		Returns:
			Error code.
	*/
	HRESULT Init(CARD_SUIT const _Suit, CARD_RANK const _Rank);

	/*
		Function: Init

		Initializes card(loads texture from file).

		Parameters:
			sFile - name of card file.

		Returns:
			Error code.
	*/
	HRESULT Init(wchar_t sFile[128])
	{
		Clear();

		HRESULT hr;

		V_RETURN(D3DX11CreateShaderResourceViewFromFile(AUTGetD3DDevice(), sFile, NULL, NULL, &m_pSRV, &hr));

		return S_OK;
	};

	//void SetPlayer(CPlayer *const pPlayer) { m_pPlayer = pPlayer; };
	//CPlayer const *const GetPlayer() const { return m_pPlayer; };

	/*
		Function: SetPlayer

		Sets the player, which has this card.

		Parameters:
			pPlayer - player, which has this card.

		Returns:
	*/
	void SetPlayer(CPlayer *const pPlayer) { m_pPlayer = pPlayer; };
	/*
		Function: GetPlayer

		Gets the player, which has this card.

		Parameters:

		Returns:
			Player, which has this card.
	*/
	CPlayer *const GetPlayer() { return m_pPlayer; };

	/*
		Function: SetCardState

		Sets the state of card.

		Parameters:
			_bState - the state of card.

		Returns:
	*/
	void SetCardState(CARD_STATE const _bState) { m_State = _bState; };
	/*
		Function: GetCardState

		Gets the state of card.

		Parameters:

		Returns:
			The state of card.
	*/
	CARD_STATE GetCardState() const { return m_State; };

	/*
		Function: GetCardSuit

		Gets the suit of card.

		Parameters:

		Returns:
			The suit of card.
	*/
	CARD_SUIT GetCardSuit() const { return m_Suit; };
	/*
		Function: GetCardRank

		Gets the rank of card.

		Parameters:

		Returns:
			The rank of card.
	*/
	CARD_RANK GetCardRank() const { return m_Rank; };

	/*
		Function: SetCardStartPos

		Sets the pos of card.

		Parameters:
			_vPos - the pos of card.

		Returns:
	*/
	void SetCardStartPos(XMFLOAT2 const &_vPos) { m_vStart = _vPos; };
	/*
		Function: GetCardStartPos

		Gets the pos of card.

		Parameters:

		Returns:
			The pos of card.
	*/
	XMFLOAT2 const *const GetCardStartPos() const { return &m_vStart; };

	/*
		Function: SetCardOldStartPos

		Sets the old pos of card.

		Parameters:
			_vPos - the old pos of card.

		Returns:
	*/
	void SetCardOldStartPos(XMFLOAT2 const &_vPos) { m_vOldStart = _vPos; };
	/*
		Function: GetCardOldStartPos

		Gets the old pos of card.

		Parameters:

		Returns:
			The old pos of card.
	*/
	XMFLOAT2 const *const GetCardOldStartPos() const { return &m_vOldStart; };

	/*
		Function: SetCardEndPos

		Sets the size of card.

		Parameters:
			_vPos - the size of card.

		Returns:
	*/
	void SetCardEndPos(XMFLOAT2 const &_vPos) { m_vEnd = _vPos; };
	/*
		Function: GetCardEndPos

		Gets the size of card.

		Parameters:

		Returns:
			The size of card.
	*/
	XMFLOAT2 const *const GetCardEndPos() const { return &m_vEnd; };

	/*
		Function: GetSRV

		Gets the shader resource view.

		Parameters:

		Returns:
			The shader resource view.
	*/
	ID3D11ShaderResourceView *const GetSRV() const { return m_pSRV; }
private:
	/*
		Variable: m_Suit
		Suit of card.

		See also:
			<CARD_SUIT>
	*/
	CARD_SUIT	m_Suit;
	/*
		Variable: m_Rank
		Rank of card.

		See also:
			<CARD_RANK>
	*/
	CARD_RANK	m_Rank;

	/*
		Variable: m_pPlayer
		Player, which has this card.
	*/
	//void		*m_pPlayer;
	CPlayer		*m_pPlayer;

	/*
		Variable: m_vOldStart
		Old pos of card.
	*/
	XMFLOAT2	m_vOldStart;

	/*
		Variable: m_vStart
		Pos of card.
	*/
	XMFLOAT2	m_vStart;
	/*
		Variable: m_vEnd
		Size of card.
	*/
	XMFLOAT2	m_vEnd;

	/*
		Variable: m_State
		State of card.
	*/
	CARD_STATE	m_State;

	/*
		Variable: m_pSRV
		Shader resource view of card.
	*/
	ID3D11ShaderResourceView *m_pSRV;
};

/*
	Class: CDeckOfCards
	
	Describe a deck.
*/
class CDeckOfCards
{
public:
	/*
		Function: Clear

			Clears content of a deck.

		Parameters:

		Returns:
	*/
	void Clear()
	{
		for (int suit = 0; suit < CARD_SUIT_COUNT; suit++)
		{
			for (int rank = 0; rank < CARD_RANK_COUNT; rank++)
			{
				m_Cards[suit][rank].Clear();
			}
		}

		m_BackOfCard.Clear();
	};

	/*
		Constructor: CDeckOfCards
		Initializes a deck and clears it.
	*/
	CDeckOfCards() 
	{
		Clear();
	};

	/*
		Destructor: CDeckOfCards
		Clears a deck.
	*/
	~CDeckOfCards() 
	{
		Clear();
	};

	/*
		Function: InitDeck

		Initializes a deck(loads all cards and texture of back side of card).

		Parameters:

		Returns:
			Error code.
	*/
	HRESULT InitDeck()
	{
		Clear();

		HRESULT hr;

		for (int suit = 0; suit < CARD_SUIT_COUNT; suit++)
		{
			for (int rank = 0; rank < CARD_RANK_COUNT; rank++)
			{
				V_RETURN(m_Cards[suit][rank].Init((CARD_SUIT)suit, (CARD_RANK)rank));
			}
		}

		m_BackOfCard.Init(L"data\\models\\images\\DownCards.png");

		return S_OK;
	};

	/*
		Function: GetCard

		Gets card from deck.

		Parameters:
			_Suit - suit of needed card.
			_Rank - rank of needed card.

		Returns:
			Card from deck.
	*/
	CCard *const GetCard(CARD_SUIT const _Suit, CARD_RANK const _Rank) { return &m_Cards[_Suit][_Rank]; };

	/*
		Function: GetBackCard

		Gets back side of card.

		Parameters:

		Returns:
			Back side of card.
	*/
	CCard const *const GetBackCard() const { return &m_BackOfCard; };
private:
	/*
		Variable: m_Cards
		Array which contains all cards in game.

		See also:
			<CCard>
	*/
	CCard m_Cards[CARD_SUIT_COUNT][CARD_RANK_COUNT];

	/*
		Variable: m_BackOfCard
		Back side of card(needed texture).

		See also:
			<CCard>
	*/
	CCard m_BackOfCard;
};

#endif