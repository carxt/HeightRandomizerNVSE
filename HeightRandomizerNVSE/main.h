#pragma once
#include <string>
#include "internal/utility.h"
#include "nvse/PluginAPI.h"
#include "nvse/GameObjects.h"
#include "nvse/SafeWrite.h"
#include <mutex>
#include <set>
float __fastcall HeightRandomizerHook(TESObjectREFR* form);
constexpr DWORD playerRefID = 0x14, playerBaseId = 0x7;

namespace HeightRandomizer
{
	bool bScalePlayerHead = false;
	bool IsInMainThread()
	{
		void* OSGlobalsPtr = *(void**)0x11DEA0C;
		if (OSGlobalsPtr)
		{
			return GetCurrentThreadId() == ThisStdCall(0x044EDB0, OSGlobalsPtr);
		}
		return true;
	}

	unsigned int FNV1aHasher(unsigned int key)
	{
		const unsigned int fnv_prime = 16777619;
		unsigned int       hash = 0x811c9dc5;
		BYTE* keyPTR = (BYTE*)&key;
		hash ^= (keyPTR[0]);
		hash *= fnv_prime;

		hash ^= (keyPTR[1]);
		hash *= fnv_prime;

		hash ^= (keyPTR[2]);
		hash *= fnv_prime;

		hash ^= (keyPTR[3]);
		hash *= fnv_prime;

		return hash;
	}

	unsigned int RandomizerDefault(unsigned int key)
	{
		srand(key);
		return rand();
	}

	float CurveNormalizerBase(unsigned int numberToSpread, unsigned int normalizer)
	{
		return (((float((numberToSpread % normalizer))) / (normalizer - 1 * (normalizer != 1))) - 0.5);
	}
	float CurveNormalizerRealistic(unsigned int numberToSpread, unsigned int normalizer)
	{
		float baseSpread = CurveNormalizerBase(numberToSpread, normalizer);
		float absFactor = fabs(baseSpread);
		return (0.115 * baseSpread) / (1 - (0.115) * (14.9) * absFactor);
	}
	float CurveNormalizerHighVariance(unsigned int numberToSpread, unsigned int normalizer)
	{
		float baseSpread = CurveNormalizerBase(numberToSpread, normalizer);
		float absFactor = abs(2 * baseSpread);
		absFactor *= absFactor * absFactor;
		float result = (((float(baseSpread > 0) * 2) - 1) * absFactor / 2);
		return result;
	}
	float CurveNormalizerSemilinear(unsigned int numberToSpread, unsigned int normalizer)
	{
		float baseSpread = CurveNormalizerBase(numberToSpread, normalizer);
		float absFactor = abs(1.25 * baseSpread);
		absFactor *= absFactor * absFactor;
		float result = (((float(baseSpread > 0) * 2) - 1) * sqrtf(absFactor));
		return result;
	}


	float GetScaledHeadSize(Actor* act)
	{
		float scaleIn = 1.0f;
		scaleIn = fmin(fmax(HeightRandomizerHook(act), 0.85), 1.15);
		//scaleIn = (sqrt(scaleIn) - 0.2125) / ((scaleIn) - 0.2125); //just go with sqrt
		if (scaleIn < 1)
		{
			scaleIn = sqrt(2.25 - (1.25 * scaleIn));
		}
		else 
		{
			scaleIn *= scaleIn;
			scaleIn *= scaleIn;
			scaleIn = (0.125 / scaleIn) + 0.875;
		}
		return scaleIn;
	}

	char NiHeadBlockNameScaler[MAX_PATH] = {};
	char NiHeadBlockNameNewId[MAX_PATH] = {};
	void* (__cdecl* NiAlloc)(unsigned int) = (decltype(NiAlloc))0xAA13E0;
	NiNode* (__thiscall* GetSubNiNode)(Actor*, NiNode*, char*) = (decltype(GetSubNiNode))0x0490310;
	NiNode* (__thiscall* NiSetLocalNodeScale) (NiNode*, float) = (decltype(NiSetLocalNodeScale))0x0440490;




	std::set<Actor*> v_currentPol;
	std::mutex cloneMut;
	class spinMutex
	{
		int spinCount;
		std::mutex& m_tx;
	public:
		spinMutex() = delete;
		spinMutex(std::mutex& mut, int spin) : m_tx(mut)
		{
			spinCount = 0;
			int spinAmountLow = spin / 5;
			while (spinCount < spin)
			{
				if (spinCount > spinAmountLow)
				{
					Sleep(0);
				}
				_mm_pause();
				if (m_tx.try_lock())
				{
					return;
				}
				spinCount++;
			}
			m_tx.lock();
		}
		virtual ~spinMutex()
		{
			m_tx.unlock();
		}
	};

	struct NiAVUpdateInfo
	{
		float		timePassed;
		bool		updateController;
		bool		byte05;
		bool		byte06;
		bool		byte07;
		bool		byte08;
		UInt8		pad09[3];

		NiAVUpdateInfo() { ZeroMemory(this, sizeof(NiAVUpdateInfo)); }
	};
	static const NiAVUpdateInfo updateInfo;
	bool AppendNode(Actor* act, NiNode* origNode)
	{
		NiNode* targetNode = origNode;
		if (act->refID == playerRefID || (!targetNode))
		{
			targetNode = act->GetNiNode();
		} 
		if (!targetNode)
		{
			return false;
		}
		NiNode* headNode = GetSubNiNode(act, targetNode, NiHeadBlockNameScaler);
		if (!headNode)
		{
			return false;
		}
		unsigned int nodeChildren = ThisStdCall(0x43B480, headNode);
		NiNode* proxyNode = GetSubNiNode(act, targetNode, NiHeadBlockNameNewId);
		if (!proxyNode)
		{
			proxyNode = (NiNode*)NiAlloc(0xAC);
			proxyNode = (NiNode*)ThisStdCall(0xA5ECB0, proxyNode, 0);
			char* fixedStringBuf;
			char** fixedString = &fixedStringBuf;
			ThisStdCall(0x0438170, fixedString, NiHeadBlockNameNewId);
			ThisStdCall(0x0A5B950, proxyNode, fixedString);
			ThisStdCall(0x04381B0, (void*)fixedString);
			for (int nodeC = 0; nodeC < nodeChildren; nodeC++)
			{
				NiNode* headNodeChildN = (NiNode*)ThisStdCall(0x043B4A0, headNode, nodeC);
				ThisStdCall(0xA5ED10, proxyNode, headNodeChildN, 1);
			}
			ThisStdCall(0x572160, headNode);
			ThisStdCall(0xA5ED10, headNode, proxyNode, 1);
		}
		NiSetLocalNodeScale(proxyNode, GetScaledHeadSize(act));
		//ThisStdCall(0x0A5DD70, proxyNode, &updateInfo, 0); //dont use updatepass, instead use updatebound
		ThisStdCall(0x0A5E0B0, proxyNode, &updateInfo);

		return true;
	}

	inline uintptr_t GetRelJumpAddr(uintptr_t address)
	{
		return *(uintptr_t*)(address + 1) + address + 5;
	}

	void AddModelToQueue(Actor* act)
	{
		if (act->IsActor())
		{
			if ((act->refID != playerRefID && act->baseForm->refID != playerBaseId) || bScalePlayerHead) //leave the player alone unless enabled
			{
				if (act->baseForm->typeID == kFormType_NPC)
				{
					[](Actor* act)
					{
						spinMutex(cloneMut, 2500);
						v_currentPol.insert(act);
					}(act);
				}
			}

		}
	}

	template <uintptr_t a_vHook>
	class hk_ScaleInitHook
	{
	private:
		inline static uintptr_t a_addrOrig = a_vHook;
		static void* __fastcall UpdModelEpilog(Actor* act, void* edx, unsigned int bDoBackgroundClone)
		{
			void* retVal = (void*) ThisStdCall(a_addrOrig, act, bDoBackgroundClone);
			AddModelToQueue(act);
			return retVal;

		}
	public:
		hk_ScaleInitHook()
		{
			uintptr_t hk_hookPoint = a_addrOrig;
			a_addrOrig = GetRelJumpAddr(a_addrOrig);
			WriteRelCall(hk_hookPoint, (uintptr_t)UpdModelEpilog);
		}
	};


	template <uintptr_t a_vHook>
	class hk_GetScaleHook
	{
	private:
		inline static uintptr_t a_addrOrig = a_vHook;
		static float __fastcall GetActor3D(Actor* act)
		{
			float(__thiscall * hookCall)(Actor*) = (decltype(hookCall))a_addrOrig;
			float retVal = hookCall(act);
			AddModelToQueue(act);
			return retVal;

		}
	public:
		hk_GetScaleHook()
		{
			uintptr_t hk_hookPoint = a_addrOrig;
			a_addrOrig = GetRelJumpAddr(a_addrOrig);
			WriteRelCall(hk_hookPoint, (uintptr_t)GetActor3D);
		}
	};

}
