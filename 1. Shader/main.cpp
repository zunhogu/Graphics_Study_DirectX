#include "systemclass.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) {
	SystemClass* System;
	bool result;

	// 시스템 객체를 생성한다.
	System = new SystemClass;
	if (!System) {
		return 0;
	}

	// 생성한 시스템 객체를 초기화한다.
	result = System->Initialize();
	if (result) {
		System->Run();
	}

	// 시스템 객체를 메모리에서 정리한다.
	System->Shutdown();
	delete System;
	System = 0;

	return 0;
}