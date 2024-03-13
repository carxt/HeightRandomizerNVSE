#pragma once
#include <string>
#include "internal/utility.h"
#include "nvse/PluginAPI.h"
#include "nvse/GameObjects.h"
#include "nvse/SafeWrite.h"


#pragma once
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
}
