#include <logger/logger.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <random>

using std::cout;
using std::endl;
using std::string;

string get_cmd_option(int argc, char* argv[], const string& option)
{
    for (int i = 1; i < argc; ++i)
    {
        string arg = argv[i];
        if (arg == option && i + 1 < argc)
        {
            return argv[i + 1];
        }
    }
    return "";
}

int main(int argc, char* argv[])
{
    string module_name = get_cmd_option(argc, argv, "--module");
    if (module_name.empty())
    {
        module_name = "DEFAULT_MODULE";
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(10, 100);

    long long counter = 0;
    cout << "Starting stress test for module: " << module_name << ". Press Ctrl+C to stop." << endl;

    while (true)
    {
        counter++;
        string message = "Stress test message #" + std::to_string(counter);

        if (counter % 100 == 0)
        {
            ac::logger::get_instance().log(ac::log_level::error, module_name.c_str(), 500, message, __FILE__, __LINE__);
        }
        else if (counter % 20 == 0)
        {
            ac::logger::get_instance().log(ac::log_level::warn, module_name.c_str(), 200, message, __FILE__, __LINE__);
        }
        else
        {
            ac::logger::get_instance().log(ac::log_level::info, module_name.c_str(), 1, message, __FILE__, __LINE__);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(distrib(gen)));
    }

    return 0;
}
