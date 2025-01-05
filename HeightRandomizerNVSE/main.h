#pragma once
#include <string>
#include <cmath>
#include "internal/utility.h"
#include "nvse/PluginAPI.h"
#include "nvse/GameObjects.h"
#include "nvse/SafeWrite.h"

float __fastcall HeightRandomizerHook(TESObjectREFR* form);
constexpr DWORD playerRefID = 0x14, playerBaseId = 0x7;

namespace HeightRandomizer
{
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
		if (act->refID != playerRefID && act->baseForm->refID != playerBaseId) //leave the player alone
		{
			scaleIn = fmin(fmax(HeightRandomizerHook(act), 0.75), 1.25);
		}
		scaleIn *= scaleIn;
		scaleIn = 1 / std::cbrt(scaleIn);
		return scaleIn;
	}

	char NiHeadBlockNameScaler[MAX_PATH] = {};
	char NiHeadBlockNameNewId[MAX_PATH] = {};

	template <uintptr_t a_vHook>
	class hk_ScaleInitHook
	{
	private:
		inline static uintptr_t a_addrOrig = a_vHook;
		static NiNode* __fastcall GenerateNode(Actor* act, void* edx, unsigned int bDoBackgroundClone)
		{
			void* (__cdecl * NiAlloc)(unsigned int) = (decltype(NiAlloc))0xAA13E0;
			NiNode* (__thiscall * GetSubNiNode)(Actor*, NiNode*, char*) = (decltype(GetSubNiNode)) 0x0490310;
			NiNode* (__thiscall * NiSetLocalNodeScale) (NiNode*, float) = (decltype(NiSetLocalNodeScale)) 0x0440490;


			NiNode* retNode = (NiNode*) ThisStdCall(a_addrOrig, act, bDoBackgroundClone);

			NiNode* targetNode = retNode;
			if (act->refID == playerRefID || act->baseForm->refID == playerBaseId)
			{
				targetNode = act->GetNiNode();
			}
			NiNode* headNode = GetSubNiNode(act, targetNode, NiHeadBlockNameScaler);
			unsigned int nodeChildren = ThisStdCall(0x043B480, targetNode);
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
					NiNode* headNodeChildN = (NiNode*) ThisStdCall(0x043B4A0, headNode, nodeC);
					ThisStdCall(0xA5ED10, proxyNode, headNodeChildN, 1);
				}
				ThisStdCall(0x572160, headNode);
				ThisStdCall(0xA5ED10, headNode, proxyNode, 1);
			}
			NiSetLocalNodeScale(proxyNode, GetScaledHeadSize(act));
			return retNode;
		}
	public:
		hk_ScaleInitHook()
		{
			uintptr_t hk_hookPoint = a_addrOrig;
			a_addrOrig = (*(uintptr_t*)a_addrOrig);
			SafeWrite32(hk_hookPoint, (uintptr_t)GenerateNode);
		}
	};

}
