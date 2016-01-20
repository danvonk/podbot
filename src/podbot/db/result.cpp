#include "result.h"

Result::Result()
{
}

Result::Result(MYSQL_RES * result, MYSQL_FIELD * fields, u64 rowCount, u32 fieldCount)
	: row_count_(rowCount)
	, field_count_(fieldCount)
	, result_(result)
	, fields_(fields)
{
	//create an empty array of fields
	current_row_ = new Field[fieldCount];
}

Result::~Result()
{
	delete[] current_row_;
	mysql_free_result(result_);
}

void Result::NextRow()
{
	MYSQL_ROW row = mysql_fetch_row(result_);
	unsigned long* lengths = mysql_fetch_lengths(result_); //the array containing the lengths of each field

	for (u32 i = 0; i < field_count_; ++i) {
		current_row_[i].SetFieldValue(row[i], fields_[i].type, lengths[i]);
	}
}

u64 Result::get_row_count() const
{
	return row_count_;
}

u32 Result::get_field_count() const
{
	return field_count_;
}
