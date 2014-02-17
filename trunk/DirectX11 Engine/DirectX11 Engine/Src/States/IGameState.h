#ifndef _IGAMESTATE_H_
#define _IGAMESTATE_H_


class IGameState
{
private:


public:

	virtual  ~IGameState( void ) = 0 {  }
	virtual void EnterState( void ) = 0; 
	virtual void ExitState( void ) = 0;
	virtual bool Input( void ) = 0; 
	virtual void UpdateState( float _deltaTime) = 0;
	virtual void RenderState( void ) = 0;



};
#endif