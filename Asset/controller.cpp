
#include "main.h"
#include "controller.h"



// パッド変数
NpadJoyDualState g_OldPadState;
NpadJoyDualState g_PadState;


// 振動子変数
VibrationDeviceHandle g_VibrationDevice[2];
int g_VibrationLeftFrame;
int g_VibrationRightFrame;


// 6軸センサ変数
SixAxisSensorHandle g_SixAxisSensor[2];
SixAxisSensorState g_SixAxisSensorState[2] = {};

// タッチスクリーン変数
TouchScreenState<1> g_TouchScreenState;

// =========================================================
// Right stick settings
// =========================================================
// Smaller value means slower and smoother aiming.
// Valid range: 0.05f to 1.00f.
float g_RightStickSensitivity{ 0.3f };

// Raw right stick value.
Float2 g_RawRightStick{};

// Smoothed right stick value returned to the game.
Float2 g_FilteredRightStick{};

// Right stick dead zone.
constexpr float RIGHT_STICK_DEAD_ZONE = 0.12f;

// Value used when snapping a nearly-zero filtered value to zero.
constexpr float RIGHT_STICK_ZERO_THRESHOLD = 0.005f;

void InitController()
{
	// パッド初期化

	InitializeNpad();
	NpadIdType npadIds[] = { NpadId::No1,
							NpadId::Handheld };
	SetSupportedNpadStyleSet(NpadStyleJoyDual::Mask | NpadStyleHandheld::Mask);
	SetSupportedNpadIdType(npadIds, 2);



	// 振動子初期化
	if (GetNpadStyleSet(NpadId::No1).Test<nn::hid::NpadStyleJoyDual>())
		GetVibrationDeviceHandles(g_VibrationDevice, 2, NpadId::No1, NpadStyleJoyDual::Mask);
	else if (GetNpadStyleSet(NpadId::Handheld).Test<nn::hid::NpadStyleHandheld>())
		GetVibrationDeviceHandles(g_VibrationDevice, 2, NpadId::Handheld, NpadStyleHandheld::Mask);

	for (int i = 0; i < 2; i++)
	{
		InitializeVibrationDevice(g_VibrationDevice[i]);
	}

	g_VibrationLeftFrame = 0;
	g_VibrationRightFrame = 0;

	

	// 6軸センサ初期化
	if (GetNpadStyleSet(NpadId::No1).Test<nn::hid::NpadStyleJoyDual>())
		GetSixAxisSensorHandles(g_SixAxisSensor, 2, NpadId::No1, NpadStyleJoyDual::Mask);
	else if (GetNpadStyleSet(NpadId::Handheld).Test<nn::hid::NpadStyleHandheld>())
		GetSixAxisSensorHandles(g_SixAxisSensor, 2, NpadId::Handheld, NpadStyleHandheld::Mask);

	for (int i = 0; i < 2; i++)
	{
		StartSixAxisSensor(g_SixAxisSensor[i]);
	}



	// タッチスクリーン初期化
	InitializeTouchScreen();
	ResetRightStickInput();
}



void UninitController()
{

}


void UpdateController()
{

	g_OldPadState = g_PadState;


	// パッド状態取得
	if (GetNpadStyleSet(NpadId::No1).Test<nn::hid::NpadStyleJoyDual>())
		GetNpadState(&g_PadState, NpadId::No1);
	else if (GetNpadStyleSet(NpadId::Handheld).Test<nn::hid::NpadStyleHandheld>())
		GetNpadState((NpadHandheldState*)&g_PadState, NpadId::Handheld);


	// 6軸センサ状態取得
	GetSixAxisSensorState(&g_SixAxisSensorState[0], g_SixAxisSensor[0]);

	if (g_SixAxisSensor[1]._storage)
		GetSixAxisSensorState(&g_SixAxisSensorState[1], g_SixAxisSensor[1]);
	else
		g_SixAxisSensorState[1] = g_SixAxisSensorState[0];


	// タッチスクリーン状態取得
	GetTouchScreenStates(&g_TouchScreenState, 1);



	// 振動子停止
	if (g_VibrationLeftFrame > 0)
	{
		g_VibrationLeftFrame--;

		if (g_VibrationLeftFrame == 0)
		{
			VibrationValue vibration = VibrationValue::Make();

			for (int i = 0; i < 2; i++)
			{
				VibrationDeviceInfo info;
				GetVibrationDeviceInfo(&info, g_VibrationDevice[i]);
				if (info.position == VibrationDevicePosition_Left)
				{
					SendVibrationValue(g_VibrationDevice[i], vibration);
				}
			}
		}
	}

	if (g_VibrationRightFrame > 0)
	{
		g_VibrationRightFrame--;

		if (g_VibrationRightFrame == 0)
		{
			VibrationValue vibration = VibrationValue::Make();

			for (int i = 0; i < 2; i++)
			{
				VibrationDeviceInfo info;
				GetVibrationDeviceInfo(&info, g_VibrationDevice[i]);
				if (info.position == VibrationDevicePosition_Right)
				{
					SendVibrationValue(g_VibrationDevice[i], vibration);
				}
			}
		}
	}


}




bool GetControllerPress(int button)
{
	return g_PadState.buttons.Test(button);
}

bool GetControllerTrigger(int button)
{
	return (g_PadState.buttons ^ g_OldPadState.buttons & g_PadState.buttons).Test(button);

}


Float2 GetControllerLeftStick()
{
	Float2 stick;
	stick.x = (float)g_PadState.analogStickL.x / AnalogStickMax;
	stick.y = (float)g_PadState.analogStickL.y / AnalogStickMax;
	return stick;
}

Float2 GetControllerRightStick()
{
	g_RawRightStick.x = (float)g_PadState.analogStickR.x / AnalogStickMax;
	g_RawRightStick.y = (float)g_PadState.analogStickR.y / AnalogStickMax;

	float lengthSq =
		g_RawRightStick.x * g_RawRightStick.x +
		g_RawRightStick.y * g_RawRightStick.y;

	if (lengthSq < RIGHT_STICK_DEAD_ZONE * RIGHT_STICK_DEAD_ZONE)
	{
		g_RawRightStick = MakeFloat2(0.0f, 0.0f);
	}

	g_FilteredRightStick.x += (g_RawRightStick.x - g_FilteredRightStick.x) * g_RightStickSensitivity;
	g_FilteredRightStick.y += (g_RawRightStick.y - g_FilteredRightStick.y) * g_RightStickSensitivity;

	if (g_RawRightStick.x == 0.0f && fabsf(g_FilteredRightStick.x) < RIGHT_STICK_ZERO_THRESHOLD)
	{
		g_FilteredRightStick.x = 0.0f;
	}

	if (g_RawRightStick.y == 0.0f && fabsf(g_FilteredRightStick.y) < RIGHT_STICK_ZERO_THRESHOLD)
	{
		g_FilteredRightStick.y = 0.0f;
	}

	return g_FilteredRightStick;
}



Float3 GetControllerLeftAcceleration()
{
	return g_SixAxisSensorState[0].acceleration;
}

Float3 GetControllerRightAcceleration()
{
	return g_SixAxisSensorState[1].acceleration;
}

Float3 GetControllerLeftAngle()
{
	return g_SixAxisSensorState[0].angle;
}

Float3 GetControllerRightAngle()
{
	return g_SixAxisSensorState[1].angle;
}



void SetControllerLeftVibration(int frame)
{

	for (int i = 0; i < 2; i++)
	{
		VibrationDeviceInfo info;
		GetVibrationDeviceInfo(&info, g_VibrationDevice[i]);
		if (info.position == VibrationDevicePosition_Left)
		{
			VibrationValue vibration = VibrationValue::Make(0.0f, 10.0f, 0.5f, 320.0f);
			SendVibrationValue(g_VibrationDevice[i], vibration);
		}
	}

	g_VibrationLeftFrame = frame;

}


void SetControllerRightVibration(int frame)
{

	for (int i = 0; i < 2; i++)
	{
		VibrationDeviceInfo info;
		GetVibrationDeviceInfo(&info, g_VibrationDevice[i]);
		if (info.position == VibrationDevicePosition_Right)
		{
			VibrationValue vibration = VibrationValue::Make(0.0f, 10.0f, 0.5f, 320.0f);
			SendVibrationValue(g_VibrationDevice[i], vibration);
		}
	}

	g_VibrationRightFrame = frame;

}



bool GetControllerTouchScreen()
{
	if (g_TouchScreenState.count > 0)
		return true;
	else
		return false;
}

Float2 GetControllerTouchScreenPosition()
{
	Float2 position;
	position.x = g_TouchScreenState.touches[0].x * SCREEN_WIDTH / 1280.0f - SCREEN_WIDTH / 2.0f;
	position.y = g_TouchScreenState.touches[0].y * SCREEN_HEIGHT / 720.0f - SCREEN_HEIGHT / 2.0f;

	return position;
}

// =========================================================
// Set right stick sensitivity
// =========================================================
void SetRightStickSensitivity(float sensitivity)
{
	if (sensitivity < 0.05f)
	{
		sensitivity = 0.05f;
	}
	else if (sensitivity > 1.0f)
	{
		sensitivity = 1.0f;
	}

	g_RightStickSensitivity = sensitivity;
}

// =========================================================
// Get right stick sensitivity
// =========================================================
float GetRightStickSensitivity()
{
	return g_RightStickSensitivity;
}

// =========================================================
// Reset right stick filtered value
// =========================================================
void ResetRightStickInput()
{
	g_RawRightStick = MakeFloat2(0.0f, 0.0f);
	g_FilteredRightStick = MakeFloat2(0.0f, 0.0f);
}