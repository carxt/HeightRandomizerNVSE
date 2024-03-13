// all credits to JazzisParis
#pragma once

#include <intrin.h>

const double
kDblZero = 0,
kDblPI = 3.141592653589793,
kDblPIx2 = 6.283185307179586,
kDblPIx3d2 = 4.71238898038469,
kDblPId2 = 1.5707963267948966,
kDblPId4 = 0.7853981633974483,
kDblPId6 = 0.5235987755982989,
kDblPId12 = 0.26179938779914946,
kDbl2dPI = 0.6366197723675814,
kDbl4dPI = 1.2732395447351628,
kDblTanPId6 = 0.5773502691896257,
kDblTanPId12 = 0.2679491924311227,
kDblPId180 = 0.017453292519943295;

const float
kFltZero = 0.0F,
kFltHalf = 0.5F,
kFltOne = 1.0F,
kFltTwo = 2.0F,
kFltFour = 4.0F,
kFltSix = 6.0F,
kFlt10 = 10.0F,
kFlt100 = 100.0F,
kFlt2048 = 2048.0F,
kFlt4096 = 4096.0F,
kFlt10000 = 10000.0F,
kFlt12288 = 12288.0F,
kFlt40000 = 40000.0F,
kFltMax = FLT_MAX;

#define GameHeapAlloc(size) ThisStdCall<void*>(0xAA3E40, (void*)0x11F6238, size)
#define GameHeapFree(ptr) ThisStdCall<void*>(0xAA4060, (void*)0x11F6238, ptr)
class CriticalSection {
	CRITICAL_SECTION	critSection;

public:
	CriticalSection() { InitializeCriticalSection(&critSection); }
	~CriticalSection() { DeleteCriticalSection(&critSection); }

	void Enter() { EnterCriticalSection(&critSection); }
	void Leave() { LeaveCriticalSection(&critSection); }
	bool TryEnter() { return TryEnterCriticalSection(&critSection) != 0; }
};

class LightCS {
	UInt32	owningThread;
	UInt32	enterCount;

public:
	LightCS() : owningThread(0), enterCount(0) {}

	void Enter();
	void EnterSleep();
	void Leave();
};
union Coordinate {
	UInt32		xy;
	struct {
		SInt16	y;
		SInt16	x;
	};

	Coordinate() {}
	Coordinate(SInt16 _x, SInt16 _y) : x(_x), y(_y) {}

	inline Coordinate& operator =(const Coordinate& rhs) {
		xy = rhs.xy;
		return *this;
	}
	inline Coordinate& operator =(const UInt32& rhs) {
		xy = rhs;
		return *this;
	}

	inline bool operator ==(const Coordinate& rhs) { return xy == rhs.xy; }
	inline bool operator !=(const Coordinate& rhs) { return xy != rhs.xy; }

	inline Coordinate operator +(const char* rhs) {
		return Coordinate(x + rhs[0], y + rhs[1]);
	}
};

template <typename T1, typename T2> inline T1 GetMin(T1 value1, T2 value2) {
	return (value1 < value2) ? value1 : value2;
}

template <typename T1, typename T2> inline T1 GetMax(T1 value1, T2 value2) {
	return (value1 > value2) ? value1 : value2;
}

template <typename T> inline T sqr(T value) {
	return value * value;
}

bool fCompare(float lval, float rval);

int __stdcall lfloor(float value);
int __stdcall lceil(float value);

float __stdcall fSqrt(float value);
double __stdcall dSqrt(double value);

double dCos(double angle);
double dSin(double angle);
double dTan(double angle);

double dAtan(double value);
double dAsin(double value);
double dAcos(double value);
double dAtan2(double y, double x);

UInt32 __fastcall GetNextPrime(UInt32 num);

UInt32 __fastcall RGBHexToDec(UInt32 rgb);

UInt32 __fastcall RGBDecToHex(UInt32 rgb);

UInt32 __fastcall StrLen(const char* str);

char* __fastcall StrEnd(const char* str);

bool __fastcall MemCmp(const void* ptr1, const void* ptr2, UInt32 bsize);

void __fastcall MemZero(void* dest, UInt32 bsize);

char* __fastcall StrCopy(char* dest, const char* src);

char* __fastcall StrNCopy(char* dest, const char* src, UInt32 length);

char* __fastcall StrCat(char* dest, const char* src);

UInt32 __fastcall StrHash(const char* inKey);

bool __fastcall CmprLetters(const char* lstr, const char* rstr);

bool __fastcall StrEqualCS(const char* lstr, const char* rstr);

bool __fastcall StrEqualCI(const char* lstr, const char* rstr);

char __fastcall StrCompare(const char* lstr, const char* rstr);

char __fastcall StrBeginsCS(const char* lstr, const char* rstr);

char __fastcall StrBeginsCI(const char* lstr, const char* rstr);

void __fastcall FixPath(char* str);

void __fastcall StrToLower(char* str);

void __fastcall ReplaceChr(char* str, char from, char to);

char* __fastcall FindChr(const char* str, char chr);

char* __fastcall FindChrR(const char* str, UInt32 length, char chr);

char* __fastcall SubStr(const char* srcStr, const char* subStr);

char* __fastcall SlashPos(const char* str);

char* __fastcall SlashPosR(const char* str);

char* __fastcall GetNextToken(char* str, char delim);

char* __fastcall GetNextToken(char* str, const char* delims);

char* __fastcall CopyString(const char* key);

char* __fastcall IntToStr(int num, char* str);

char* __fastcall FltToStr(float num, char* str);

int __fastcall StrToInt(const char* str);

double __fastcall StrToDbl(const char* str);

char* __fastcall UIntToHex(UInt32 num, char* str);

UInt32 __fastcall HexToUInt(const char* str);

bool __fastcall FileExists(const char* path);

class FileStream {
protected:
	HANDLE		theFile;
	UInt32		streamLength;
	UInt32		streamOffset;

public:
	FileStream() : theFile(INVALID_HANDLE_VALUE), streamLength(0), streamOffset(0) {}
	~FileStream() { if (theFile != INVALID_HANDLE_VALUE) Close(); }

	bool Good() const { return theFile != INVALID_HANDLE_VALUE; }
	HANDLE GetHandle() const { return theFile; }
	UInt32 GetLength() const { return streamLength; }
	UInt32 GetOffset() const { return streamOffset; }
	bool HitEOF() const { return streamOffset >= streamLength; }

	bool Open(const char* filePath);
	bool OpenAt(const char* filePath, UInt32 inOffset);
	bool OpenWrite(const char* filePath);
	bool Create(const char* filePath);
	bool OpenWriteEx(char* filePath, bool append);
	void SetOffset(UInt32 inOffset);

	void Close() {
		CloseHandle(theFile);
		theFile = INVALID_HANDLE_VALUE;
	}

	void ReadBuf(void* outData, UInt32 inLength);
	void WriteBuf(const void* inData, UInt32 inLength);

	static void MakeAllDirs(char* fullPath);
};

class DebugLog {
	FILE* theFile;
	UInt32			indent;

public:
	DebugLog() : theFile(NULL), indent(40) {}
	~DebugLog() { if (theFile) fclose(theFile); }

	bool Create(const char* filePath);
	void Message(const char* msgStr);
	void FmtMessage(const char* fmt, va_list args);
	void Indent() { if (indent) indent--; }
	void Outdent() { if (indent < 40) indent++; }
};

extern DebugLog gLog, s_debug;

void PrintLog(const char* fmt, ...);
void PrintDebug(const char* fmt, ...);

class LineIterator {
protected:
	char* dataPtr;

public:
	LineIterator(const char* filePath, char* buffer);

	bool End() const { return *dataPtr == 3; }
	void Next();
	char* Get() { return dataPtr; }
};

class DirectoryIterator {
	HANDLE				handle;
	WIN32_FIND_DATA		fndData;

public:
	DirectoryIterator(const char* path) : handle(FindFirstFile(path, &fndData)) {}
	~DirectoryIterator() { Close(); }

	bool End() const { return handle == INVALID_HANDLE_VALUE; }
	void Next() { if (!FindNextFile(handle, &fndData)) Close(); }
	bool IsFile() const { return !(fndData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY); }
	bool IsFolder() const {
		if (IsFile()) return false;
		if (fndData.cFileName[0] != '.') return true;
		if (fndData.cFileName[1] != '.') return fndData.cFileName[1] != 0;
		return fndData.cFileName[2] != 0;
	}
	const char* Get() const { return fndData.cFileName; }
	void Close() {
		if (handle != INVALID_HANDLE_VALUE) {
			FindClose(handle);
			handle = INVALID_HANDLE_VALUE;
		}
	}
};

bool FileToBuffer(const char* filePath, char* buffer);

void __fastcall GetTimeStamp(char* buffer);

UInt32 __fastcall ByteSwap(UInt32 dword);

void DumpMemImg(void* data, UInt32 size, UInt8 extra = 0);

void GetMD5File(const char* filePath, char* outHash);

void GetSHA1File(const char* filePath, char* outHash);

// Taken from xNVSE
// Pair this with _AddressOfReturnAddress()
UInt8* GetParentBasePtr(void* addressOfReturnAddress, bool lambda = false);