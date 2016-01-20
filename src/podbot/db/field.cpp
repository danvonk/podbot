#include "field.h"

Field::Field()
{
	data.value = nullptr;
	data.type = MYSQL_TYPE_NULL;
	data.raw = false;
	data.length = 0;
}

Field::~Field()
{
}

void Field::SetFieldValue(char * value, enum_field_types type, u32 length)
{
	data.value = new char[length + 1];
	memcpy(data.value, value, length);
	//add the null terminator
	*(reinterpret_cast<char*>(data.value) + length) = '\0';
	data.length = length;
}

bool Field::GetBool() const
{
	return false;
}

u8 Field::GetU8() const
{
	if (data.raw) {
		return *reinterpret_cast<u8*>(data.value);
	}
	return static_cast<u8>(strtoul((char*)data.value, nullptr, 10));
}

int8 Field::GetI8() const
{
	if (data.raw) {
		return *reinterpret_cast<int8*>(data.value);
	}
	return static_cast<int8>(strtoul((char*)data.value, nullptr, 10));
}

u16 Field::GetU16() const
{
	if (!data.value) {
		return 0;
	}

	if (data.raw) {
		return *reinterpret_cast<u16*>(data.value);
	}
	return static_cast<u16>(strtoul((char*)data.value, nullptr, 10));
}

int16 Field::GetI16() const
{
	if (!data.value) {
		return 0;
	}

	if (data.raw) {
		return *reinterpret_cast<int16*>(data.value);
	}
	return static_cast<int16>(strtoul((char*)data.value, nullptr, 10));
}

u32 Field::GetU32() const
{
	if (!data.value) {
		return 0;
	}

	if (data.raw) {
		return *reinterpret_cast<u32*>(data.value);
	}
	return static_cast<u32>(strtoul((char*)data.value, nullptr, 10));
}

int32 Field::GetI32() const
{
	if (!data.value) {
		return 0;
	}

	if (data.raw) {
		return *reinterpret_cast<int32*>(data.value);
	}
	return static_cast<int32>(strtoul((char*)data.value, nullptr, 10));
}

u64 Field::GetU64() const
{
	if (!data.value) {
		return 0;
	}

	if (data.raw) {
		return *reinterpret_cast<u64*>(data.value);
	}
	return static_cast<u64>(strtoul((char*)data.value, nullptr, 10));
}

int64 Field::GetI64() const
{
	if (!data.value) {
		return 0;
	}

	if (data.raw) {
		return *reinterpret_cast<int64*>(data.value);
	}
	return static_cast<int64>(strtoul((char*)data.value, nullptr, 10));
}

float Field::GetFloat() const
{
	if (!data.value) {
		return 0;
	}

	if (data.raw) {
		return *reinterpret_cast<float*>(data.value);
	}
	return static_cast<float>(strtoul((char*)data.value, nullptr, 10));
}

double Field::GetDouble() const
{
	if (!data.value) {
		return 0;
	}

	if (data.raw) {
		return *reinterpret_cast<double*>(data.value);
	}
	return static_cast<double>(strtoul((char*)data.value, nullptr, 10));
}

std::string Field::GetString() const
{
	return std::string(static_cast<const char*>(data.value), data.length);
}

bool Field::GetNull() const
{
	return data.value == nullptr;
}
