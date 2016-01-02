#pragma once

#include "common.h"
#include "field.h"
#include <mysql.h>

class Result {
public:
	Result();
	Result(MYSQL_RES* result, MYSQL_FIELD* fields, u64 rowCount, u32 fieldCount);

	void NextRow();

	u64 get_row_count() const;
	u32 get_field_count() const;

	//overload the [] to provide array style access to the fields.
	const Field& operator [] (u32 index) const {
		return current_row_[index];
	}

private:
	u64 row_count_;
	u32 field_count_;

	MYSQL_RES* result_;
	MYSQL_FIELD* fields_;

	Field* current_row_;
};