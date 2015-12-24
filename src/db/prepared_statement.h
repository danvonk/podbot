#pragma once

#include "common.h"
#include <mysql.h>

namespace db {
	struct Memory {
		void* data;
		u32 size;
	};

	union TPreparedStatementDataTypes {
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

	enum class TPreparedStatementDataTypeKey {
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
		String
	};

	struct TPreparedStatementData
	{
		TPreparedStatementDataTypeKey type_name;
		TPreparedStatementDataTypes data;
		std::vector<u8> string;
	};

	class PreparedStatement {
		friend class Connection;

	public:
		PreparedStatement(std::string&& query);
		~PreparedStatement();

		void BindBuffers(int paramCount);

		void set_bool(const u8 index, const bool val);
		void set_uint8(const u8 index, const u8 val);
		void set_uint16(const u8 index, const u16 val);
		void set_uint32(const u8 index, const u32 val);
		void set_uint64(const u8 index, const u64 val);

		void set_int8(const u8 index, const int32 val);
		void set_int16(const u8 index, const int16 val);
		void set_int32(const u8 index, const int32 val);
		void set_int64(const u8 index, const int64 val);

		void set_float(const u8 index, const float val);
		void set_double(const u8 index, const double val);
		
		void set_string(const u8 index, const std::string& val);
		void set_null(const u8 index);
		void set_blob(const u8 index, const std::vector<u8>& val);


		//Delete the assignment operators
		PreparedStatement(PreparedStatement const& that) = delete;
		PreparedStatement& operator=(PreparedStatement const& that) = delete;
	private:
		void FillBuffer(TPreparedStatementData& el, MYSQL_BIND* param);

		MYSQL_STMT* statement_;
		MYSQL_BIND* bind_;
		std::string query_;
		int index_;

		std::vector<TPreparedStatementData> stmt_binds_;
	};
}