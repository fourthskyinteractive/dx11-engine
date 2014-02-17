#include "MainMenuState.h"


MainMenuState* MainMenuState::GetInstance( void )
{

	// Lazy Instantiation
	static MainMenuState instance;

	return &instance;
}


MainMenuState::MainMenuState(void)
{

	m_obj = nullptr;
	m_input = nullptr;

	m_nBackgroundID = -1;

}
MainMenuState::~MainMenuState(void)
{


}



void MainMenuState::EnterState( void )
{
	m_input = DirectInput::GetInstance();

}
void MainMenuState::ExitState( void )
{
	m_input = nullptr;
}
bool MainMenuState::Input( void )
{


	return true;
}
void MainMenuState::UpdateState(float _deltaTime)
{



}
void MainMenuState::RenderState( void )
{


}