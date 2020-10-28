#include "main.h"

bool NVSEPlugin_Query(const NVSEInterface* nvse, PluginInfo* info)
{
	info->infoVersion = PluginInfo::kInfoVersion;
	info->name = "HeightRandomizer";
	info->version = 1;

	if (nvse->isEditor)
	{
		return false;
	}
	gLog.Open("HeightRandomizer.log");
	if (nvse->runtimeVersion != RUNTIME_VERSION_1_4_0_525)
	{
		_MESSAGE("ERROR: Unsupported runtime version (%08X).", nvse->runtimeVersion);
		return false;
	}
	_MESSAGE("successful load");
	return true;
}



float (*jeffthespreader)(unsigned int , unsigned int);
unsigned int (*NumberManipulatorFunction)(unsigned int);

float g_uSPECIALPercentage;
float g_uMaxDriftPercentage;
float g_uFemalePercentage;

float __fastcall HeightRandomizerHook(TESObjectREFR* form)
{
	float returnscale = form->scale;
	auto ActorBase = form->baseForm;
	if (ActorBase) {
		if (form->baseForm->typeID == kFormType_TESCreature)
		{

			returnscale *= ((TESCreature*)ActorBase)->baseScale;

		}
		else if (form->baseForm->typeID == kFormType_TESNPC)
		{
			TESNPC* baseNPC = ((TESNPC*)ActorBase);
			returnscale *= baseNPC->height;
			
			if (fabs(returnscale - 1) < 0.009 && form->refID != playerRefID && baseNPC->refID != playerID && jeffthespreader && NumberManipulatorFunction)
			{
				
				returnscale += (jeffthespreader(NumberManipulatorFunction(form->refID & 0xFFFFFF), 101) * (g_uMaxDriftPercentage * (1 - g_uSPECIALPercentage))) ;
				returnscale += (((baseNPC->attributes.attributes[TESAttributes::kStrength] - baseNPC->attributes.attributes[TESAttributes::kAgility]) / 2 + (baseNPC->attributes.attributes[TESAttributes::kEndurance] - 5.5) + (baseNPC->attributes.attributes[TESAttributes::kLuck] - 5.5)/4.5))/10 * (g_uMaxDriftPercentage * (g_uSPECIALPercentage));
				if (baseNPC->baseData.flags & TESActorBaseData::kFlags_Female) returnscale *= g_uFemalePercentage;

			}
		}
	
	}
	return returnscale;
}
bool NVSEPlugin_Load(const NVSEInterface* nvse)
{
	char iniDir[MAX_PATH];

	GetModuleFileNameA(GetModuleHandle(NULL), iniDir, MAX_PATH);
	if ((unsigned int(GetPrivateProfileInt("Main", "uHeightAlgorithm", 1, iniDir))) == 0) return true;
	strcpy((char*)(strrchr(iniDir, '\\') + 1), "Data\\NVSE\\Plugins\\HeightRandomizer.ini");
	_MESSAGE("%s", iniDir);
	unsigned int intArgtmp;
	intArgtmp = GetPrivateProfileInt("Main", "uSPECIALPercentage", 15, iniDir) ;
	if (intArgtmp >= 100) intArgtmp = 100;
	g_uSPECIALPercentage = float(intArgtmp) / 100;
	intArgtmp = GetPrivateProfileInt("Main", "uMaxDriftPercentage", 15, iniDir);
	if (intArgtmp >= 99) intArgtmp = 99;
	g_uMaxDriftPercentage = float(intArgtmp) / 50;
	g_uFemalePercentage = ((float) GetPrivateProfileInt("Main", "uFemalePercentage", 92, iniDir)) / 100;
	if (!GetPrivateProfileInt("Main", "bUseTrueRandom", 0, iniDir)) { NumberManipulatorFunction = FNV1aSpreadr; }
	else { NumberManipulatorFunction = RandomizerDefault ; }


	switch (unsigned int(GetPrivateProfileInt("Main", "uHeightAlgorithm", 1, iniDir)) % 3)
	{
	case 1:
		jeffthespreader = jeffthespreaderrealistic;
		break;
	case 2:
		jeffthespreader = jeffthespreadervariant;
		break;
	default:
		jeffthespreader = jeffthespreadersemilinear;

	}

	if (g_uFemalePercentage <= 0.1) g_uFemalePercentage = 0.1;
	WriteRelJump(0x0567400, (UInt32)HeightRandomizerHook);
	return true;
}








