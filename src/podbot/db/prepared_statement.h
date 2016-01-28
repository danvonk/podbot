#pragma once

#include "common.h"
#include <mysql.h>

class DateTime;
namespace db {

	union PreparedStatementDataUnion {
		u8 _u8;
		u16 _u16;
		u32 _u32;
		u64 _u64;
		int8 _i8;
		int16 _i16;
		int32 _i32;
		int64 _i64;

		float _f;
		double _d;
	};

	enum class PreparedStatementType {
		Uint8,
		Uint16,
		Uint32,
		Uint64,
		Int8,
		Int16,
		Int32,
		Int64,
		Float,
		Double,
		Null,
		String,
		DateTime
	};

	struct TPreparedStatementData
	{
		PreparedStatementType type_name;
		PreparedStatementDataUnion data;
		std::vector<u8> string;
		Memory memory;
	};

	class PreparedStatement {
		friend class Connection;

	public:
		PreparedStatement(std::string&& query);
		~PreparedStatement();

		void set_bool(const u8 index, const bool val);
		void set_uint8(const u8 index, const u8 val);
		void set_uint16(const u8 index, const u16 val);
		void set_uint32(const u8 index, const u32 val);
		void set_uint64(const u8 index, const u64 val);

		void set_int8(const u8 index, const int8 val);
		void set_int16(const u8 index, const int16 val);
		void set_int32(const u8 index, const int32 val);
		void set_int64(const u8 index, const int64 val);

		void set_float(const u8 index, const float val);
		void set_double(const u8 index, const double val);
		
		void set_string(const u8 index, const std::string& val);
		void set_null(const u8 index);
		void set_blob(const u8 index, Memory* mem);
		void set_date_time(const u8 index, DateTime* dt);
		
		bool is_bound() {
		  return is_bound_;
		}


		//Delete the assignment operators
		PreparedStatement(PreparedStatement const& that) = delete;
		PreparedStatement& operator=(PreparedStatement const& that) = delete;
	private:
		void FillBuffer(TPreparedStatementData& el, MYSQL_BIND* param);
		void BindBuffers();

		MYSQL_TIME time_;
		MYSQL_STMT* statement_;
		MYSQL_BIND* bind_;
		std::string query_;
		u32 param_count_;
		int index_;
		bool is_bound_;

		std::vector<TPreparedStatementData> stmt_binds_;
	};
}