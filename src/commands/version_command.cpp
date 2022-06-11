#include "commands/version_command.hpp"

#include "version.hpp"

#include <plog/Log.h>

VersionCommand::VersionCommand(const std::shared_ptr<Settings> settings)
    : Command("version", "Show the version", settings) { }

void VersionCommand::execute(const std::vector<std::string>& args) {
    LOG(plog::info) << "Cora v" << CORA_VERSION;
}