#pragma once

#include <mysql.h>
#include "common.h"

class Field {
	friend class Result;

public:
	Field();
	~Field();

	void SetFieldValue(char* value, enum_field_types type, u32 length);

	bool GetBool() const;

	u8 GetU8() const;
	int8 GetI8() const;

	u16 GetU16() const;
	int16 GetI16() const;

	u32 GetU32() const;
	int32 GetI32() const;

	u64 GetU64() const;
	int64 GetI64() const;

	float GetFloat() const;
	double GetDouble() const;

	//getBinary needed here too
	std::string GetString() const;
	bool GetNull() const;

private:
#pragma pack(push, 1)
	struct {
		u32 length;
		void* value;
		enum_field_types type;
		bool raw;
	} data;
#pragma pack(pop)
};