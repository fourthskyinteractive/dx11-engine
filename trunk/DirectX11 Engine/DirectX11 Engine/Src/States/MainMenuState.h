#ifndef _MAINMENUSTATE_H_
#define _MAINMENUSTATE_H_


#include "IGameState.h"

#include "../Game Objects\BaseObject.h"
#include "../Game Objects\ScreenSpaceObject.h"
#include "../Input\DirectInput.h"


class MainMenuState : public IGameState
{
private:

	BaseObject* m_obj;

	DirectInput * m_input;

	int m_nBackgroundID;



	MainMenuState(void);
	virtual ~MainMenuState(void);
	MainMenuState( const MainMenuState& );
	MainMenuState& operator = (const MainMenuState& );




public:

	static MainMenuState* GetInstance( void );

	void EnterState( void ); 
	void ExitState( void );
	bool Input( void ); 
	void UpdateState(float _deltaTime);
	void RenderState( void );


};

#endif