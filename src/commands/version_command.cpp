#include "commands/version_command.hpp"

#include "version.hpp"

#include <plog/Log.h>

VersionCommand::VersionCommand() : Command("version", "Show the version") {
}

VersionCommand::~VersionCommand() { }

void VersionCommand::execute(const std::vector<std::string>& args) {
    LOG(plog::info) << "Cora v" << CORA_VERSION;
}