#pragma once

#include "nvse/PluginAPI.h"
#include "nvse/GameObjects.h"
#include "nvse/SafeWrite.h"


IDebugLog gLog;


unsigned int FNV1aSpreadr(unsigned int key)
{
	const unsigned int fnv_prime = 16777619;
	unsigned int hash = 0x811c9dc5;
	BYTE* keyPTR = (BYTE*)& key;
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



float jeffnothing(unsigned int numberToSpread, unsigned int normalizer)
{
	return  (((float((numberToSpread % normalizer))) / (normalizer - 1 * (normalizer != 1) )) - 0.5);

}
float jeffthespreaderrealistic(unsigned int numberToSpread, unsigned int normalizer)
{
	float spreaded = jeffnothing(numberToSpread, normalizer);
	float absFactor = fabs(spreaded);
	ASSERT_STR(absFactor < 0.55, "MATH FUNCTION FAILED");
	return (0.115 * spreaded) / (1 - (0.115) * (14.9) * absFactor);

}
float jeffthespreadervariant(unsigned int numberToSpread, unsigned int normalizer)
{
	float spreaded = jeffnothing(numberToSpread, normalizer);
	float absFactor = abs(2 * spreaded);
	absFactor *= absFactor * absFactor;
	float result = (((float(spreaded > 0) * 2) - 1) * absFactor / 2);
	return result;
}
float jeffthespreadersemilinear(unsigned int numberToSpread, unsigned int normalizer)
{
	float spreaded = jeffnothing(numberToSpread, normalizer);
	float absFactor = abs(1.25*spreaded);
	absFactor *= absFactor * absFactor;
	float result = (((float(spreaded > 0) * 2) - 1) * sqrtf(absFactor));
	return result;
}



