#include <logger/logger.hpp>
#include <iostream>

using std::cout;
using std::endl;

int main()
{
    cout << "--- Logger Test Start ---" << endl;

    ac::logger::get_instance().log(ac::log_level::info, "LOGGER_TEST", 1, "Logger test message 1.", __FILE__, __LINE__);
    ac::logger::get_instance().log(ac::log_level::warn, "LOGGER_TEST", 2, "Logger test message 2.", __FILE__, __LINE__);
    ac::logger::get_instance().log(ac::log_level::error, "LOGGER_TEST", 3, "Logger test message 3.", __FILE__, __LINE__);

    cout << "--- Logger Test End ---" << endl;
    cout << "Check console output and the contents of 'alacrity-chess/logs/alacrity-chess.log'" << endl;

    return 0;
}
