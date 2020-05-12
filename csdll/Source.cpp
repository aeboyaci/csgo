#include <Windows.h>


// Güncelleme gelince buradaki offset deðerlerini yeniden bul.
struct offsets
{
	uintptr_t dwlocalPlayer = 0xD2FB94;
	uintptr_t dwGlowObjectManager = 0x528B8A0;
	uintptr_t m_iGlowIndex = 0xA428;
	uintptr_t dwEntityList = 0x4D43AC4;
	uintptr_t m_bDormant = 0xED;
	uintptr_t m_iTeamNum = 0xF4;
	uintptr_t flashDuration = 0xA410;
	uintptr_t dwForceJump = 0x51ED760;
	uintptr_t m_fFlags = 0x104;
	uintptr_t velocity = 0x114;
}offests;

struct vector
{
	float x,y,z;
};

DWORD fMain(HMODULE hMod)
{
	uintptr_t client = (uintptr_t)(GetModuleHandle(TEXT("client_panorama.dll"))); // client_panorama.dll'in addressine pointer koyuyorum.

	// Pointerlar koyuyorum.
	uintptr_t pLocalPlayer = client + offests.dwlocalPlayer;
	uintptr_t pGlowObjectManager = client + offests.dwGlowObjectManager;
	uintptr_t pEntityList = client + offests.dwEntityList;
	int flashDur = 0;
	while (true)
	{
		if (GetAsyncKeyState(VK_END) & 1) break;
		
		// Point edilen deðerleri koyuyorum.
		uintptr_t LocalPlayer = *(uintptr_t*)pLocalPlayer;
		uintptr_t GlowObjectManager = *(uintptr_t*)pGlowObjectManager;
		uintptr_t EntityList = *(uintptr_t*)pEntityList;
		uintptr_t pFlashDuration = LocalPlayer + offests.flashDuration;
		uintptr_t pFlag = LocalPlayer + offests.m_fFlags;
		uintptr_t pForceJump = client + offests.dwForceJump;

		// Deðerler boþ deðilse iþlem gerçekleþtirmem lazým yoksa crash olur.(yani boþsa)
		if (LocalPlayer != NULL && GlowObjectManager != NULL && EntityList != NULL)
		{
			int myTeamNum = *(int*)(LocalPlayer + offests.m_iTeamNum);
			for (short i = 0; i < 64; i++) // i 64'ten büyükse player deðildir.
			{
				uintptr_t entity = *(uintptr_t*)(pEntityList + i * 0x10); // 0x10 Entity Size
				if (entity == NULL) continue;
				if (*(bool*)(entity + offests.m_bDormant)) continue;

				int glowIndex = *(int*)(entity + offests.m_iGlowIndex);
				int entityTeamNum = *(int*)(entity + offests.m_iTeamNum);

				if (entityTeamNum == myTeamNum)
				{
					// Takým arkadaþlarý
					*(float*)((GlowObjectManager)+((glowIndex * 0x38) + 0x4)) = 0.f;
					*(float*)((GlowObjectManager)+((glowIndex * 0x38) + 0x8)) = 0.f;
					*(float*)((GlowObjectManager)+((glowIndex * 0x38) + 0xC)) = 1.f;
					*(float*)((GlowObjectManager)+((glowIndex * 0x38) + 0x10)) = 1.7f;
				}
				else
				{
					// Rakip takým
					*(float*)((GlowObjectManager)+((glowIndex * 0x38) + 0x4)) = 1.f;
					*(float*)((GlowObjectManager)+((glowIndex * 0x38) + 0x8)) = 0.f;
					*(float*)((GlowObjectManager)+((glowIndex * 0x38) + 0xC)) = 0.f;
					*(float*)((GlowObjectManager)+((glowIndex * 0x38) + 0x10)) = 1.7f;
				}
				*(bool*)(GlowObjectManager + ((glowIndex * 0x38) + 0x24)) = true;
				*(bool*)(GlowObjectManager + ((glowIndex * 0x38) + 0x25)) = false;
			}

			// Anti-Flash
			flashDur = *(int*)(pFlashDuration);
			if (flashDur > 0)
				*(int*)(pFlashDuration) = 0;

					
			// BunnyHop
			uintptr_t pVector = (LocalPlayer + offests.velocity);
			vector playerVel = *(vector*)(pVector);
			int vel = playerVel.x + playerVel.y + playerVel.z;
			BYTE flag = *(BYTE*)(pFlag);
			if(vel != 0)
				if (GetAsyncKeyState(VK_SPACE) && flag && (1 << 0))
					*(int*)(pForceJump) = 6;

		}

		Sleep(10); // CPU'ya çok yüklenmemek için.
	}

	
	FreeLibraryAndExitThread(hMod,0);
	return 0;
}


BOOL APIENTRY DllMain(HMODULE hMod, DWORD fdwReason, LPVOID lpReserved)
{
	DisableThreadLibraryCalls(hMod);
	if (fdwReason == DLL_PROCESS_ATTACH)
		CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)fMain, hMod, 0, nullptr);
	return TRUE;
}
