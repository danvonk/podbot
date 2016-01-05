#include "config.h"
#include <fstream>
#include <iostream>


namespace po = boost::program_options;

ConfigMgr::ConfigMgr()
	: desc_("Podbot usage")
{
	desc_.add_options()
		("help,h", "Display this help message")
		("version,v", "Display the version number")
		("db_user", po::value<std::string>()->default_value("root"), "Username for the database server")
		("db_pass", po::value<std::string>()->default_value(""), "Password for the database server")
		("db_name", po::value<std::string>()->default_value("dev"), "Name of the database")
		("db_port", po::value<int>()->default_value(3306), "Port number for the database server")
		("db_host", po::value<std::string>()->default_value("127.0.0.1"), "Hostname for the database server");
}


ConfigMgr::~ConfigMgr()
{
}

void ConfigMgr::ParseConfigFile(const std::string & path)
{
	std::ifstream ifs(path);
	if (ifs) {
		try {
			po::store(po::parse_config_file(ifs, desc_), vm_);
			po::notify(vm_);
		}
		catch (const std::exception&) {
			PBLOG_CRITICAL << "Could not parse config file. Check that it is in the correct location and that the syntax is valid.";
		}
	}
}

void ConfigMgr::ParseCommandLine(int argc, char * agrv[])
{
	try {
		po::store(po::parse_command_line(argc, agrv, desc_), vm_);
		po::notify(vm_);
	}
	catch (const std::exception&) {
		PBLOG_CRITICAL << "Could not parse command line arguments. Check that the argument(s) supplied are valid.";
	}
}

int ConfigMgr::GetCount(const std::string & key)
{
	return vm_.count(key);
}
