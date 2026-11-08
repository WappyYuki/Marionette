/******************************************************************************
 * ingameplay.cpp: インゲームの進行を管理する基底クラス */

#include "ingameplay.h"

bool InGamePlay::Start()
{
	if( state_ != State::NotStarted ) return state_ != State::Finished;

	state_ = State::Playing;
	frameCount_ = 0;
	if( !OnStart() ){
		Finish(); // 途中まで作った物を OnFinish で片付ける
		return false;
	}
	return true;
}

void InGamePlay::Update()
{
	if( state_ != State::Playing ) return;

	OnUpdate();
	++frameCount_;
}

void InGamePlay::Draw()
{
	if( state_ == State::NotStarted || state_ == State::Finished ) return;

	OnDraw();
}

void InGamePlay::Finish()
{
	if( state_ == State::NotStarted || state_ == State::Finished ) return;

	state_ = State::Finished;
	OnFinish();
}

void InGamePlay::Pause()
{
	if( state_ == State::Playing ) state_ = State::Paused;
}

void InGamePlay::Resume()
{
	if( state_ == State::Paused ) state_ = State::Playing;
}

void InGamePlay::GetViewTarget( float &x, float &y, float &z ) const
{
	x = y = z = 0.0f;
}
