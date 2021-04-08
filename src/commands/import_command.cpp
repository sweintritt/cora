#include "commands/import_command.hpp"

#include <plog/Log.h>

#include "importer/radio_sure_importer.hpp"

#include <memory>

ImportCommand::ImportCommand() : Command("import", "Import radio stations from different sources"), m_importerByName() {
        auto radioSureImporter = std::unique_ptr<RadioSureImporter>(new RadioSureImporter());
        m_importerByName.insert(std::make_pair(radioSureImporter->getName(), std::move(radioSureImporter)));
        m_cli.addOption('i', "input", true, "Input to import stations from. Depends on the type an could be a file or URL.");
        m_cli.addOption('t', "type", true, "Type of imported data. Supported types are: radiosure (requires a file as input).");
}

ImportCommand::~ImportCommand() { }

void ImportCommand::execute(const std::vector<std::string>& args) {
    m_cli.parse(args);

    if (m_cli.hasOption('h')) {
        LOG(plog::info) << m_cli.usage();
        return;
    }

    if (!m_cli.hasValue('t') || !m_cli.hasValue('i')) {
        LOG(plog::warning) << "Missing arguments. Try " << m_name << " --help for more information.";
        return;
    }

    auto importer = m_importerByName.find(m_cli.getValue('t'));

    if (importer == m_importerByName.end()) {
        LOG(plog::error) << "Unknown importer type " << m_cli.getValue('t') << ". Try " << m_name << " --help for more information.";
    } else {
        auto stationsDao = createStationsDao();
        if (m_cli.hasValue('f')) {
            stationsDao->open(m_cli.getValue('f'));
        } else {
            stationsDao->open(getDefaultFile());
        }

        LOG(plog::info) << "importing from " << m_cli.getValue('i') << " using " << importer->second->getName() << " importer";
        importer->second->import(m_cli.getValue('i'), stationsDao);
    }
}