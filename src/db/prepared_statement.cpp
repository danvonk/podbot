#include "prepared_statement.h"

using namespace db;
using pst = TPreparedStatementDataTypeKey;

PreparedStatement::PreparedStatement(std::string && query)
	: statement_(nullptr)
	, bind_(nullptr)
	, query_(std::move(query))
{
}

PreparedStatement::~PreparedStatement()
{
	delete[] bind_;
	delete statement_;
}

void PreparedStatement::FillBuffer(TPreparedStatementData& el, MYSQL_BIND* param)
{
	param->buffer_length = 0;
	param->is_null_value = 0;
	param->is_null = 0;
	param->length = nullptr;

	switch (el.type_name)
	{
	case pst::Uint8:
		param->buffer_type = MYSQL_TYPE_TINY;
		param->buffer = &(el.data._u8);
		param->is_unsigned = true;
		break;
	case pst::Int8:
		param->buffer_type = MYSQL_TYPE_TINY;
		param->buffer = &(el.data._i8);
		param->is_unsigned = false;
		break;
	case pst::Uint16:
		param->buffer_type = MYSQL_TYPE_SHORT;
		param->buffer = &(el.data._u16);
		param->is_unsigned = true;
		break;
	case pst::Int16:
		param->buffer_type = MYSQL_TYPE_SHORT;
		param->buffer = &(el.data._i16);
		param->is_unsigned = false;
		break;
	case pst::Uint32:
		param->buffer_type = MYSQL_TYPE_LONG;
		param->buffer = &(el.data._u32);
		param->is_unsigned = true;
		break;
	case pst::Int32:
		param->buffer_type = MYSQL_TYPE_LONG;
		param->buffer = &(el.data._i32);
		param->is_unsigned = false;
		break;
	case pst::Uint64:
		param->buffer_type = MYSQL_TYPE_LONGLONG;
		param->buffer = &(el.data._u64);
		param->is_unsigned = true;
		break;
	case pst::Int64:
		param->buffer_type = MYSQL_TYPE_LONGLONG;
		param->buffer = &(el.data._i64);
		param->is_unsigned = false;
		break;
	case pst::Float:
		param->buffer_type = MYSQL_TYPE_FLOAT;
		param->buffer = &(el.data._f);
		break;
	case pst::Double:
		param->buffer_type = MYSQL_TYPE_DOUBLE;
		param->buffer = &(el.data._d);
		break;
	case pst::String:
	{
		auto len = el.string.size();

		param->buffer_type = MYSQL_TYPE_VAR_STRING;
		//free(param->buffer); // or delete[]
		//delete[] static_cast<char*>(param->buffer);
		param->buffer = nullptr;
		param->buffer = new char[len];
		param->buffer_length = len;
		param->is_null_value = 0;
		param->length = new unsigned long(len - 1);

		memcpy(param->buffer, el.string.data(), len);
		break;
	}

	case pst::Null:
	default:
		param->buffer_type = MYSQL_TYPE_NULL;
		param->is_null_value = true;
		break;
	}
}

void PreparedStatement::BindBuffers(int paramCount)
{
	using pst = TPreparedStatementDataTypeKey;
	auto elements = stmt_binds_.size();

	//initialise a MYSQL_BIND structure of the correct size.
	if (elements != 1) {
		bind_ = new MYSQL_BIND[elements - 1];
	}
	else {
		bind_ = new MYSQL_BIND[1];
	}

	//zero the structure to prevent garbage values.
	memset(bind_, 0, sizeof(bind_) * paramCount);
	for (int i = 0; i < elements; ++i) {
		auto& el = stmt_binds_[i];
		FillBuffer(el, &bind_[i]);
	}

}

void PreparedStatement::set_bool(const u8 index, const bool val)
{
	//resize the vector if needed
	if (index >= stmt_binds_.size()) {
		stmt_binds_.resize(index + 1);
	}

	stmt_binds_[index].type_name = TPreparedStatementDataTypeKey::Uint8;
	if (val == true) {
		stmt_binds_[index].data._u8 = 1;
	}
	else {
		stmt_binds_[index].data._u8 = 0;
	}
}

void PreparedStatement::set_uint8(const u8 index, const u8 val)
{
	//resize the vector if needed
	if (index >= stmt_binds_.size()) {
		stmt_binds_.resize(index + 1);
	}

	stmt_binds_[index].type_name = TPreparedStatementDataTypeKey::Uint8;
	stmt_binds_[index].data._u8 = val;
}

void PreparedStatement::set_uint16(const u8 index, const u16 val)
{
	//resize the vector if needed
	if (index >= stmt_binds_.size()) {
		stmt_binds_.resize(index + 1);
	}

	stmt_binds_[index].type_name = TPreparedStatementDataTypeKey::Uint16;
	stmt_binds_[index].data._u16 = val;
}

void PreparedStatement::set_uint32(const u8 index, const u32 val)
{
	//resize the vector if needed
	if (index >= stmt_binds_.size()) {
		stmt_binds_.resize(index + 1);
	}

	stmt_binds_[index].type_name = TPreparedStatementDataTypeKey::Uint32;
	stmt_binds_[index].data._u32 = val;
}

void PreparedStatement::set_uint64(const u8 index, const u64 val)
{
	//resize the vector if needed
	if (index >= stmt_binds_.size()) {
		stmt_binds_.resize(index + 1);
	}

	stmt_binds_[index].type_name = TPreparedStatementDataTypeKey::Uint64;
	stmt_binds_[index].data._u64 = val;
}

void PreparedStatement::set_int8(const u8 index, const int32 val)
{
	//resize the vector if needed
	if (index >= stmt_binds_.size()) {
		stmt_binds_.resize(index + 1);
	}

	stmt_binds_[index].type_name = TPreparedStatementDataTypeKey::Int8;
	stmt_binds_[index].data._i8 = val;
}

void PreparedStatement::set_int16(const u8 index, const int16 val)
{
	//resize the vector if needed
	if (index >= stmt_binds_.size()) {
		stmt_binds_.resize(index + 1);
	}

	stmt_binds_[index].type_name = TPreparedStatementDataTypeKey::Int16;
	stmt_binds_[index].data._i16 = val;
}

void PreparedStatement::set_int32(const u8 index, const int32 val)
{
	//resize the vector if needed
	if (index >= stmt_binds_.size()) {
		stmt_binds_.resize(index + 1);
	}

	stmt_binds_[index].type_name = TPreparedStatementDataTypeKey::Int32;
	stmt_binds_[index].data._i32 = val;
}

void PreparedStatement::set_int64(const u8 index, const int64 val)
{
	//resize the vector if needed
	if (index >= stmt_binds_.size()) {
		stmt_binds_.resize(index + 1);
	}

	stmt_binds_[index].type_name = TPreparedStatementDataTypeKey::Int64;
	stmt_binds_[index].data._i64 = val;
}

void PreparedStatement::set_float(const u8 index, const float val)
{
	//resize the vector if needed
	if (index >= stmt_binds_.size()) {
		stmt_binds_.resize(index + 1);
	}

	stmt_binds_[index].type_name = TPreparedStatementDataTypeKey::Float;
	stmt_binds_[index].data._f = val;
}

void PreparedStatement::set_double(const u8 index, const double val)
{
	//resize the vector if needed
	if (index >= stmt_binds_.size()) {
		stmt_binds_.resize(index + 1);
	}

	stmt_binds_[index].type_name = TPreparedStatementDataTypeKey::Double;
	stmt_binds_[index].data._d = val;
}

void PreparedStatement::set_string(const u8 index, const std::string & val)
{
	//resize the vector if needed
	if (index >= stmt_binds_.size()) {
		stmt_binds_.resize(index + 1);
	}

	//+1 for the null terminator
	stmt_binds_[index].string.resize(val.size() + 1);
	memcpy(stmt_binds_[index].string.data(), val.c_str(), val.size() + 1);
	stmt_binds_[index].type_name = TPreparedStatementDataTypeKey::String;
}
