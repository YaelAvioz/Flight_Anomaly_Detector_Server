/*
 * CLI.cpp
 *
 * Author: Yael Avioz,207237421
 */

#include "CLI.h"
#include "commands.h"
#include <vector>

CLI::CLI(DefaultIO *dio)
{
    this->dio = dio;
    commands.push_back(new upload_command(dio, anomaly_report, anomaly_detector));
    commands.push_back(new settings_command(dio, anomaly_report, anomaly_detector));
    commands.push_back(new detect_command(dio, anomaly_report, anomaly_detector));
    commands.push_back(new display_command(dio, anomaly_report, anomaly_detector));
    commands.push_back(new upload_and_analyze_command(dio, anomaly_report, anomaly_detector));
}

void CLI::start()
{
    while (true)
    {
        // Print the menu
        this->dio->write("Welcome to the Anomaly Detection Server.\n");
        this->dio->write("Please choose an option:\n");
        for (int i = 0; i < 5; i++)
        {
            dio->write(to_string(i + 1) + "." + commands[i]->command_description() + "\n");
        }
        dio->write("6.exit\n");

        // Get the input
        string input_s = dio->read();
        int user_input = std::atoi(input_s.c_str());

        // In case the user choose exit stop the loop
        if (user_input == 6)
        {
            break;
        }
        
        else if ((user_input >= 1) && (user_input <= 5))
        {
            user_input -= 1;
            commands[user_input]->execute();
        }
    }
}

// Free the memory
CLI::~CLI()
{
    for (int i = 0; i < 5; i++)
    {
        delete commands[i];
    }
}