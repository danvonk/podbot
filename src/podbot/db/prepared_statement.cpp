#include "prepared_statement.h"
#include "common/date_time.h"

using namespace db;
using pst = PreparedStatementType;

PreparedStatement::PreparedStatement(std::string && query)
	: statement_(nullptr)
	, bind_(nullptr)
	, query_(std::move(query))
{
}

PreparedStatement::~PreparedStatement()
{
	//TODO: memory leak.
//	for (u32 i = 0; i < param_count_; ++i) {
////		delete bind_[i].length;
//		bind_[i].length = nullptr;
//		if (bind_[i].buffer != nullptr){
//			//memory leak
//			delete[] bind_[i].buffer;
//		}
//		bind_[i].buffer = nullptr;
//	}
//	delete[] statement_->bind->length;
//	delete[] statement_->bind->is_null;

//	mysql_stmt_close(statement_);
//	delete[] bind_;

	if (statement_) {
		mysql_stmt_close(statement_);
	}

	if (bind_) {
		for (u32 i = 0; i < param_count_; ++i) {
			if (bind_[i].buffer_type == MYSQL_TYPE_VAR_STRING) {
				delete [] (char*) bind_[i].buffer;
				delete bind_[i].length;
			}

			bind_[i].buffer = nullptr;
			bind_[i].length = nullptr;
		}
		delete[] bind_;
	}
}

void PreparedStatement::FillBuffer(TPreparedStatementData& el, MYSQL_BIND* param)
{
	param->buffer_length = 0;
	param->is_null_value = 0;
	param->is_null = 0;
	param->length = 0;

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
	case pst::DateTime:
	{
		DateTime* dt = static_cast<DateTime*>(el.memory.data);

		time_.day = dt->Day();
		time_.month = dt->Month();
		time_.year = dt->Year();

		time_.hour = dt->Hour();
		time_.minute = dt->Minute();
		time_.second = dt->Second();

		param->buffer_type = MYSQL_TYPE_DATETIME;
		param->buffer = &time_;
		break;
	}
	case pst::Null:
		param->buffer = nullptr;
		param->buffer_type = MYSQL_TYPE_NULL;
		param->is_null_value = true;
		break;
	}
}

//TODO: Memory Leak
void PreparedStatement::BindBuffers()
{
	param_count_ = mysql_stmt_param_count(statement_);
	//initialise a MYSQL_BIND structure of the correct size.
	bind_ = new MYSQL_BIND[param_count_];
	//zero the structure to prevent garbage values.
	memset(bind_, 0, sizeof(bind_) * param_count_);
	for (unsigned int i = 0; i < param_count_; ++i) {
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

	stmt_binds_[index].type_name = PreparedStatementType::Uint8;
	if (val != 0) {
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

	stmt_binds_[index].type_name = PreparedStatementType::Uint8;
	stmt_binds_[index].data._u8 = val;
}

void PreparedStatement::set_uint16(const u8 index, const u16 val)
{
	//resize the vector if needed
	if (index >= stmt_binds_.size()) {
		stmt_binds_.resize(index + 1);
	}

	stmt_binds_[index].type_name = PreparedStatementType::Uint16;
	stmt_binds_[index].data._u16 = val;
}

void PreparedStatement::set_uint32(const u8 index, const u32 val)
{
	//resize the vector if needed
	if (index >= stmt_binds_.size()) {
		stmt_binds_.resize(index + 1);
	}

	stmt_binds_[index].type_name = PreparedStatementType::Uint32;
	stmt_binds_[index].data._u32 = val;
}

void PreparedStatement::set_uint64(const u8 index, const u64 val)
{
	//resize the vector if needed
	if (index >= stmt_binds_.size()) {
		stmt_binds_.resize(index + 1);
	}

	stmt_binds_[index].type_name = PreparedStatementType::Uint64;
	stmt_binds_[index].data._u64 = val;
}

void PreparedStatement::set_int8(const u8 index, const int8 val)
{
	//resize the vector if needed
	if (index >= stmt_binds_.size()) {
		stmt_binds_.resize(index + 1);
	}

	stmt_binds_[index].type_name = PreparedStatementType::Int8;
	stmt_binds_[index].data._i8 = val;
}

void PreparedStatement::set_int16(const u8 index, const int16 val)
{
	//resize the vector if needed
	if (index >= stmt_binds_.size()) {
		stmt_binds_.resize(index + 1);
	}

	stmt_binds_[index].type_name = PreparedStatementType::Int16;
	stmt_binds_[index].data._i16 = val;
}

void PreparedStatement::set_int32(const u8 index, const int32 val)
{
	//resize the vector if needed
	if (index >= stmt_binds_.size()) {
		stmt_binds_.resize(index + 1);
	}

	stmt_binds_[index].type_name = PreparedStatementType::Int32;
	stmt_binds_[index].data._i32 = val;
}

void PreparedStatement::set_int64(const u8 index, const int64 val)
{
	//resize the vector if needed
	if (index >= stmt_binds_.size()) {
		stmt_binds_.resize(index + 1);
	}

	stmt_binds_[index].type_name = PreparedStatementType::Int64;
	stmt_binds_[index].data._i64 = val;
}

void PreparedStatement::set_float(const u8 index, const float val)
{
	//resize the vector if needed
	if (index >= stmt_binds_.size()) {
		stmt_binds_.resize(index + 1);
	}

	stmt_binds_[index].type_name = PreparedStatementType::Float;
	stmt_binds_[index].data._f = val;
}

void PreparedStatement::set_double(const u8 index, const double val)
{
	//resize the vector if needed
	if (index >= stmt_binds_.size()) {
		stmt_binds_.resize(index + 1);
	}

	stmt_binds_[index].type_name = PreparedStatementType::Double;
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
	stmt_binds_[index].type_name = PreparedStatementType::String;
}

void db::PreparedStatement::set_null(const u8 index)
{
}

void PreparedStatement::set_blob(const u8 index, Memory * mem)
{
}

void db::PreparedStatement::set_date_time(const u8 index, DateTime * dt)
{
	//resize the vector if needed
	if (index >= stmt_binds_.size()) {
		stmt_binds_.resize(index + 1);
	}

	stmt_binds_[index].type_name = PreparedStatementType::DateTime;
	stmt_binds_[index].memory.data = static_cast<void*>(dt);
	stmt_binds_[index].memory.size = sizeof(dt);

}
