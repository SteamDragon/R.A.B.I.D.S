#include <rabids.h>
#include <config.h>

// Initializing global boost::log logger
void init()
{
	static const std::string COMMON_FMT("R.A.B.I.D.S | [%TimeStamp%] | [%Severity%]:  %Message%");
	boost::log::add_console_log(
		std::cout,
		boost::log::keywords::format = COMMON_FMT,
		boost::log::keywords::auto_flush = true);

	boost::log::add_file_log(
		boost::log::keywords::file_name = "./data/rabids_%Y-%m-%d.%N.log",											  /*< file name pattern >*/
		boost::log::keywords::rotation_size = 10 * 1024 * 1024,												  /*< rotate files every 10 MiB... >*/
		boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0), /*< ...or at midnight >*/
		boost::log::keywords::format = COMMON_FMT,															  /*< log record format >*/
		boost::log::keywords::auto_flush = true);

	boost::log::add_common_attributes();
}

int main()
{
	init();
	config autoConfig("config.cfg");
	LOG(info) << "Roving Autonomous Bartmoss Interface Drone V2 Initialized";
	LOG(info) << "Version " << autoConfig.getVersion();
	RABIDS client(autoConfig.Token(), SleepyDiscord::USER_CONTROLED_THREADS);
	client.setIntents(SleepyDiscord::Intent::SERVER_MESSAGES, SleepyDiscord::Intent::DIRECT_MESSAGES, SleepyDiscord::Intent::SERVER_INTEGRATIONS, SleepyDiscord::Intent::SERVERS);
	client.startClient(autoConfig);
}