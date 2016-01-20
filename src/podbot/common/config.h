#pragma once
#include "common.h"
#include <boost/program_options.hpp>

//to be implemented
class ConfigMgr {
public:
	ConfigMgr();
	~ConfigMgr();
	
	void ParseConfigFile(const std::string& path);
	void ParseCommandLine(int argc, char* agrv[]);
	int GetCount(const std::string& key);

	//TODO: this should return the default value set in the description table, not a user-specified one.
	template <typename T>
	T GetValue(const std::string& key, T def) {
		try
		{
			T temp = vm_[key].as<T>();
			return temp;
		}
		catch (const std::exception&)
		{
			return def;
		}
	}

private:
	boost::program_options::variables_map vm_;
	boost::program_options::options_description desc_;
};