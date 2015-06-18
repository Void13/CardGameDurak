#ifndef H__GAME
#define H__GAME

#include "AUT.h"
#include "Cards.h"
#include "AUTshaders.h"

// Constant: CARD_SCALE
// Causes the scale of card
#define CARD_SCALE 0.37f

// Constant: CARD_SIZE
// Causes the size of card
#define CARD_SIZE XMFLOAT2(194.0f * CARD_SCALE, 300.0f * CARD_SCALE)

// Constant: COUNT_PLAYERS
// Causes the number of players
#define COUNT_PLAYERS 4

/*
	Enum: PLAYER_PLACE

	Enumerates player places.

	PLAYER_PLACE_LEFT_UPPER_CORNER		- Causes the left upper corner place.
	PLAYER_PLACE_RIGHT_UPPER_CORNER		- Causes the right upper corner place.
	PLAYER_PLACE_RIGHT_BOTTOM_CORNER	- Causes the right bottom corner place.
	PLAYER_PLACE_LEFT_BOTTOM_CORNER		- Causes the left bottom corner place.
*/
enum PLAYER_PLACE
{
	PLAYER_PLACE_LEFT_UPPER_CORNER = 0,
	PLAYER_PLACE_RIGHT_UPPER_CORNER,
	PLAYER_PLACE_RIGHT_BOTTOM_CORNER,
	PLAYER_PLACE_LEFT_BOTTOM_CORNER
};

/*
	Enum: PLAYER_STATE

	Enumerates player states.

	PLAYER_STATE_NON_INITIALIZED			- Causes non initialized state
	PLAYER_STATE_INITIALIZED				- Causes initialized state
	PLAYER_STATE_CHOOSING_CARD				- Causes choosing card state
	PLAYER_STATE_CHOOSED_CARD				- Causes choosed card state
	PLAYER_STATE_COVERING_CARD				- Causes covering card state
	PLAYER_STATE_WAITING_NEXT_MOVE			- Causes waiting next move state
	PLAYER_STATE_TAKING_CARDS_FROM_FIELD	- Causes taking cards from field state
	PLAYER_STATE_GETTING_CARDS_FROM_DECK	- Causes getting cards from deck state
	PLAYER_STATE_LEFT						- Causes left state
*/
enum PLAYER_STATE
{
	PLAYER_STATE_NON_INITIALIZED = 0,
	PLAYER_STATE_INITIALIZED,
	PLAYER_STATE_CHOOSING_CARD,
	PLAYER_STATE_CHOOSED_CARD,
	PLAYER_STATE_COVERING_CARD,
	PLAYER_STATE_WAITING_NEXT_MOVE,
	PLAYER_STATE_TAKING_CARDS_FROM_FIELD,
	PLAYER_STATE_GETTING_CARDS_FROM_DECK,
	PLAYER_STATE_LEFT
};

/*
	Enum: GAME_STATE

	Enumerates game states.

	GAME_STATE_NON_LOADED				- Causes non loaded state.
	GAME_STATE_LOADED					- Causes loaded state.
	GAME_STATE_ADDING_CARD				- Causes adding card state.
	GAME_STATE_ADDED_CARD				- Causes added card state.
	GAME_STATE_COVERED_CARD				- Causes covered card state.
	GAME_STATE_TAKING_CARDS_FROM_FIELD	- Causes taking cards from field state.
	GAME_STATE_GETTING_CARDS_FROM_DECK	- Causes getting cards from deck state.
	GAME_STATE_ANIMATING				- Causes animating state.
	GAME_STATE_FINISHED					- Causes finished state.
*/
enum GAME_STATE
{
	GAME_STATE_NON_LOADED = 0,
	GAME_STATE_LOADED,
	GAME_STATE_ADDING_CARD,
	GAME_STATE_ADDED_CARD,
	GAME_STATE_COVERED_CARD,
	GAME_STATE_TAKING_CARDS_FROM_FIELD,
	GAME_STATE_GETTING_CARDS_FROM_DECK,
	GAME_STATE_ANIMATING,
	GAME_STATE_FINISHED
};

/*
	Class: CPlayer

	Describe a player in game.
*/
class CPlayer
{
public:
	/*
		Function: Clear

			Clears content of a player.

		Parameters:

		Returns:
	*/
	void Clear()
	{
		if (m_pChoosedCard)
		{
			m_pChoosedCard->SetPlayer(nullptr);
			m_pChoosedCard->SetCardState(CARD_STATE_IN_DECK);
		}
		m_pChoosedCard = nullptr;

		m_nNumCardOnHands = 0;
		for (CCard *&ppCard : m_Cards)
		{
			if (ppCard)
			{
				ppCard->SetPlayer(nullptr);
				ppCard->SetCardState(CARD_STATE_IN_DECK);
			}
			ppCard = nullptr;
		}
		m_Cards.clear();

		m_PlayerState = PLAYER_STATE_NON_INITIALIZED;

		m_bIsBot = true;
	};

	/*
		Constructor: CPlayer
		Initializes a player and clears it.
	*/
	CPlayer() 
	{
		m_pChoosedCard = nullptr;

		Clear();
	};

	/*
		Destructor: CPlayer
		Clears a player.
	*/
	~CPlayer() 
	{
		Clear();
	};

	/*
		Function: InitPlayer

		Initializes a player(distributes cards and sets state).

		Parameters:
			_Place - player place on field.

		Returns:
	*/
	void InitPlayer(PLAYER_PLACE const _Place);

	/*
		Function: DrawCards

		Draws all cards which have player.

		Parameters:

		Returns:
	*/
	void DrawCards() const;

	/*
		Function: ProcessPlayerInput

		Processes player input.

		Parameters:
			_vPos - position of cursor.
			_bTaking - is enter pressed?
			_bNextMove - unused.

		Returns:
	*/
	void ProcessPlayerInput(XMFLOAT2 const &_vPos, bool const _bTaking, bool const _bNextMove);

	/*
		Function: GetNumCards

		Returns number of cards.

		Parameters:

		Returns:
	*/
	int GetNumCards() const { return m_nNumCardOnHands; };

	/*
		Function: GetCards

		Returns constant vector of player cards.

		Parameters:

		Returns:
			Constant vector of player cards.
	*/
	vector<CCard *> const GetCards() const { return m_Cards; };

	/*
		Function: GetChoosedCard

		Returns constant card which player choosed.

		Parameters:

		Returns:
			Constant card.
	*/
	CCard *const GetChoosedCard() const { return m_pChoosedCard; };

	/*
		Function: PushCard

		Pushes one card to player.

		Parameters:
			_pCard - Card to push.

		Returns:
	*/
	void PushCard(CCard *const _pCard) 
	{
		_pCard->SetPlayer(this);
		m_nNumCardOnHands++;
		_pCard->SetCardState(CARD_STATE_ON_HANDS);
		m_Cards.push_back(_pCard);
	};

	/*
		Function: RemoveCard

		Removes one card from player's card set.

		Parameters:
			_pCard - Card to remove.

		Returns:
	*/
	void RemoveCard(CCard *const _pCard)
	{
		for (int i = 0; i < (int)m_Cards.size(); i++)
		{
			if (m_Cards[i] == _pCard)
			{
				_pCard->SetPlayer(nullptr);
				_pCard->SetCardState(CARD_STATE_OUT_OF_GAME);
				m_nNumCardOnHands--;
				m_Cards.erase(m_Cards.begin() + i);
				return;
			}
		}
	};

	/*
		Function: CheckLife

		Checks player life.

		Parameters:

		Returns:
	*/
	void CheckLife()
	{
		if (m_PlayerState == PLAYER_STATE_CHOOSED_CARD)
			m_PlayerState = PLAYER_STATE_CHOOSING_CARD;

		m_pChoosedCard = nullptr;
	};

	/*
		Function: DistributeCards

		Distributes card to player.

		Parameters:

		Returns:
	*/
	void DistributeCards();

	/*
		Function: SetPlayerState

		Sets player state.

		Parameters:
			_bState - new player state.

		Returns:
	*/
	void SetPlayerState(PLAYER_STATE const _bState) { m_PlayerState = _bState; };

	/*
		Function: GetPlayerState

		Gets player state.

		Parameters:

		Returns:
			Current player state.
	*/
	PLAYER_STATE GetPlayerState() const { return m_PlayerState; };

	/*
		Function: SortCards

		Sorts player's cards.

		Parameters:

		Returns:
	*/
	void SortCards() const;

	/*
		Function: SetIsBot

		Sets player type to bot or not bot(human maybe).

		Parameters:
			_bIsBot - bot or not bot.

		Returns:
	*/
	void SetIsBot(bool _bIsBot) { m_bIsBot = _bIsBot; };

	/*
		Function: IsBot

		Returns player bottively.

		Parameters:

		Returns:
			Player's bottively.
	*/
	bool IsBot() const { return m_bIsBot; };
private:
	/*
		Variable: m_Cards
		Player cards.

		See also:
			<CCard>
	*/
	vector<CCard *>	m_Cards;
	/*
		Variable: m_nNumCardOnHands
		Number of player cards.
	*/
	int				m_nNumCardOnHands;
	/*
		Variable: m_Place
		Player place on field.

		See also:
			<PLAYER_PLACE>
	*/
	PLAYER_PLACE	m_Place;
	/*
		Variable: m_pChoosedCard
		Player's choosed card.

		See also:
			<CCard>
	*/
	CCard			*m_pChoosedCard;
	/*
		Variable: m_PlayerState
		Player state.

		See also:
			<PLAYER_STATE>
	*/
	PLAYER_STATE	m_PlayerState;
	/*
		Variable: m_bIsBot
		Bottively.
	*/
	bool			m_bIsBot;
};

/*
	Class: CGameField
	
	Describes a gamefield.
*/
class CGameField
{
public:
	/*
		Function: Clear

			Clears content of a gamefield.

		Parameters:

		Returns:
	*/
	void Clear()
	{
		m_GameState = GAME_STATE_NON_LOADED;

		m_nCoversPlayer = 0;
		m_nPushingCardPlayer = 0;

		if (m_pTrump)
			m_pTrump->SetCardState(CARD_STATE_IN_DECK);
		m_pTrump = nullptr;
		
		for (CPlayer *pPlayer : m_ppPlayers)
		{
			if (pPlayer)
			{
				pPlayer->SetPlayerState(PLAYER_STATE_NON_INITIALIZED);
				delete pPlayer;
			}
			pPlayer = nullptr;
		}

		for (CCard *&ppCard : m_CardsOnField)
		{
			if (ppCard)
			{
				ppCard->SetPlayer(nullptr);
				ppCard->SetCardState(CARD_STATE_IN_DECK);
			}
			ppCard = NULL;
		}

		m_CardsOnField.clear();


		for (CCard *&ppCard : m_CoverCardsOnField)
		{
			if (ppCard)
			{
				ppCard->SetPlayer(nullptr);
				ppCard->SetCardState(CARD_STATE_IN_DECK);
			}
			ppCard = NULL;
		}

		m_CoverCardsOnField.clear();

		m_nStep = 0;

		while (!m_AnimQueue.empty())
			m_AnimQueue.pop();
	};

	/*
		Constructor: CGameField
		Initializes a gamefield and clears it.
	*/
	CGameField() 
	{
		m_pTrump = nullptr;
		Clear();
	};

	/*
		Destructor: CGameField
		Clears a gamefield.
	*/
	~CGameField() 
	{
		Clear();
	};

	/*
		Function: InitField

		Initializes field(inits all players, determines a trump, sets game state and determines pushing card player).

		Parameters:

		Returns:
	*/
	void InitField();

	/*
		Function: Render

		Renders field.

		Parameters:

		Returns:
	*/
	void Render() const;

	/*
		Function: GetGameState

		Gets game state.

		Parameters:

		Returns:
			Game state.
	*/
	GAME_STATE GetGameState() const { return m_GameState; };

	/*
		Function: GetTrump

		Gets trump.

		Parameters:

		Returns:
			Trump.
	*/
	CCard *const GetTrump() const { return m_pTrump; };

	/*
		Function: GameProcess

		Processes game.

		Parameters:

		Returns:
	*/
	void GameProcess();

	/*
		Function: GetMyPlayer

		Returns player-human-controller.

		Parameters:

		Returns:
			Const player.
	*/
	CPlayer const *const GetMyPlayer() const
	{
		auto it = m_ppPlayers.rbegin();
		return *it;

		//return m_ppPlayers[COUNT_PLAYERS - 1];
	}

	/*
		Function: GetLastCoveredCardID

		Returns last covered card id.

		Parameters:

		Returns:
			Const ID.
	*/
	int const GetLastCoveredCardID() const { return m_nLastCoveredCard; };

	/*
		Function: ProcessPlayerInput

		Processes player input.

		Parameters:
			_vPos - position of cursor.
			_bTaking - is enter pressed?
			_bNextMove - unused.

		Returns:
	*/
	void ProcessPlayerInput(XMFLOAT2 const &_vPos, bool const _bTaking, bool const _bNextMove);
private:
	/*
		Variable: m_ppPlayers
		Array of players.

		See also:
			<CPlayer>
	*/
	vector<CPlayer *> m_ppPlayers;

	/*
		Variable: m_nCoversPlayer
		ID player which now covers.
	*/
	int				m_nCoversPlayer;
	/*
		Variable: m_nPushingCardPlayer
		ID player which now pushes.
	*/
	int				m_nPushingCardPlayer;

	/*
		Variable: m_CoverCardsOnField
		Array of covered cards on field.

		See also:
			<CCard>
	*/
	vector<CCard *>	m_CoverCardsOnField;
	/*
		Variable: m_CardsOnField
		Array of noncovered cards on field.

		See also:
			<CCard>
	*/
	vector<CCard *>	m_CardsOnField;

	/*
		Variable: m_nLastCoveredCard
		ID of last covered card.
	*/
	int				m_nLastCoveredCard;

	/*
		Variable: m_pTrump
		Trump.

		See also:
			<CCard>
	*/
	CCard			*m_pTrump;

	/*
		Variable: m_AnimQueue
		Animation queue.

		See also:
			<CCard>
	*/
	queue<CCard *>	m_AnimQueue;
	/*
		Variable: m_OldGameState
		Old game state.

		See also:
			<GAME_STATE>
	*/
	GAME_STATE		m_OldGameState;
	/*
		Variable: m_nStep
		Step of animation.
	*/
	int				m_nStep;

	/*
		Variable: m_GameState
		Game state.

		See also:
			<GAME_STATE>
	*/
	GAME_STATE		m_GameState;
};

/*
	Function: InitDeck

	Initializes a deck.

	Parameters:

	Returns:
		Error code.
*/
HRESULT InitDeck();

/*
	Function: InitField

	Initializes a field.

	Parameters:

	Returns:
*/
void InitField();

/*
	Function: DestroyDeck

	Destroys a deck.

	Parameters:

	Returns:
*/
void DestroyDeck();

/*
	Function: DestroyField

	Destroys a field.

	Parameters:

	Returns:
*/
void DestroyField();

/*
	Function: ProcessAndRender

	Processes game and renders.

	Parameters:

	Returns:
*/
void ProcessAndRender();

/*
	Function: GetTrump

	Gets trump.

	Parameters:

	Returns:
		Trump.
*/
CCard *const GetTrump();

/*
	Function: GetNumFreeCards

	Gets number of free cards.

	Parameters:

	Returns:
		Number of free cards.
*/
int GetNumFreeCards();

/*
	Function: CanAppShowNextMoveButton

	Permission to show NextMove button.

	Parameters:

	Returns:
		Permission to show NextMove button.
*/
bool CanAppShowNextMoveButton();

/*
	Function: CanAppShowTakeButton

	Permission to show Take button.

	Parameters:

	Returns:
		Permission to show Take button.
*/
bool CanAppShowTakeButton();

//GAME_STATE GetGameState();

//CPlayer const *const GetMyPlayer();

/*
	Function: GetBackCard

	Gets back side of card.

	Parameters:

	Returns:
		Back side of card.
*/
CCard const *const GetBackCard();

/*
	Function: ProcessPlayerInput

	Processing of player input.

	Parameters:
		_vPos - position of cursor.
		_bTaking - is enter pressed?
		_bNextMove - not used.

	Returns:
*/
void ProcessPlayerInput(XMFLOAT2 const &_vPos, bool _bTaking, bool _bNextMove);

#endif