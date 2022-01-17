#include "AgcmFPSManager.h"
#include "AgcmAdmin.h"

//-----------------------------------------------------------------------
//

void AgcmFPSManager::AdjustFPS( bool foreground )
{
	AgcmFPSManager & inst = Inst();

	int cur_tickCount_ = GetTickCount();

	int frameTime = cur_tickCount_ - inst.prev_tickcount_;

	inst.prev_tickcount_ = cur_tickCount_;

	inst.CheckAvgFPS(frameTime);

	//if( !foreground )
	//{
	//	Sleep(15);
	//}
	//else
	{
		if( frameTime < 9 ) // 90프레임 이상나오면 쉰다.
			Sleep(9-frameTime);
	}
}

//-----------------------------------------------------------------------
//

void AgcmFPSManager::CheckAvgFPS(int frameTime)
{
	frame_count_ += 1.0f;

	static int frame_check_time_ = 0;
	frame_check_time_ += frameTime;

	if( frame_check_time_ > 10000 ) // 10초마다 평큔 프레임 갱신
	{
		avg_fps_ = frame_count_ / (frame_check_time_ * 0.001f);

		frame_check_time_ = 0;
		frame_count_ = 0;

		AdjustMultisampleLevel();
	}
}

//-----------------------------------------------------------------------
//

void AgcmFPSManager::AdjustMultisampleLevel()
{
	static int prev_multisample_level = 1;

	if( multisample_level_ > 1 )
	{
		if( avg_fps_ < 50 )
		{
			/*
			if( adj_multisample_level_ > 2 )
			{
				adj_multisample_level_ -= 4;

				if( adj_multisample_level_ < 2 )
					adj_multisample_level_ = 2;

				if( prev_multisample_level != adj_multisample_level_ )
				{
					RwD3D9ChangeMultiSamplingLevels( adj_multisample_level_ );

					prev_multisample_level = adj_multisample_level_;
				}
			}
			*/
			/*
			if( adj_multisample_level_ != 2 )
			{
				adj_multisample_level_ = 2;
				RwD3D9ChangeMultiSamplingLevels( 2 );
			}
			*/
		}
		/*
		else if( avg_fps_ > 70 )
		{
			if( adj_multisample_level_ < multisample_level_ )
			{
				adj_multisample_level_ += 2;

				if( multisample_level_ < adj_multisample_level_ )
					adj_multisample_level_ = multisample_level_;

				if( prev_multisample_level != adj_multisample_level_ )
				{
					RwD3D9ChangeMultiSamplingLevels( adj_multisample_level_ );
					prev_multisample_level = adj_multisample_level_;
				}
			}
		}
		*/
	}
}

//-----------------------------------------------------------------------