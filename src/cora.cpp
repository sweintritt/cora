#include <stdio.h>
#include <stdlib.h>

#include <chrono>
#include <thread>
#include <iostream>

#include <plog/Log.h>
#include <plog/Appenders/ConsoleAppender.h>

#include "cora.hpp"
#include "utils.hpp"
#include "version.hpp"
#include "commands/command_interpreter.hpp"
#include "commands/find_command.hpp"
#include "commands/help_command.hpp"
#include "commands/info_command.hpp"
#include "commands/import_command.hpp"
#include "commands/list_command.hpp"
#include "commands/play_command.hpp"
#include "commands/version_command.hpp"
#include "logging/message_only_formatter.hpp"

Cora::Cora() : m_commandInterpreter(std::make_shared<CommandInterpreter>()) {
    m_commandInterpreter->add(std::unique_ptr<Command>(new InfoCommand()));
    m_commandInterpreter->add(std::unique_ptr<Command>(new ImportCommand()));
    m_commandInterpreter->add(std::unique_ptr<Command>(new HelpCommand(m_commandInterpreter)));
    m_commandInterpreter->add(std::unique_ptr<Command>(new ListCommand()));
    m_commandInterpreter->add(std::unique_ptr<Command>(new PlayCommand()));
    m_commandInterpreter->add(std::unique_ptr<Command>(new FindCommand()));
    m_commandInterpreter->add(std::unique_ptr<Command>(new VersionCommand()));
}

Cora::~Cora() {}

void Cora::run(int argc, char* argv[]) {
    configureLogger();
    LOG(plog::debug) << "Starting cora v" << CORA_VERSION;

    if (argc < 2) {
        LOG(plog::debug) << "argc = " << argc;
        LOG(plog::warning) << "No command given. Try 'cora help' for more information.";
        return;
    }

    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i) {
        args.push_back(argv[i]);
    }

    runCommand(args);
}

void Cora::configureLogger() const {
    #ifndef NDEBUG
    static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init(plog::debug, &consoleAppender);
    LOG(plog::debug) << "Debug logging configured";
    #else
    static plog::ConsoleAppender<MessageOnlyFormatter> consoleAppender;
    plog::init(plog::info, &consoleAppender);
    #endif
}

void Cora::runCommand(const std::vector<std::string>& args) {
    const std::string cmd = args[0];

    try {
        m_commandInterpreter->execute(args);
    } catch (const std::exception& error) {
        LOG(plog::error) << error.what();
    } catch (const std::string& error) {
        LOG(plog::error) << error;
    } catch (const char* error) {
        LOG(plog::error) << error;
    } catch (...) {
        LOG(plog::error) << "error of unknown type";
    }
}