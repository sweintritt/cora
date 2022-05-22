#include "commands/import_command.hpp"

#include <plog/Log.h>

#include "importer/radio_sure_importer.hpp"
#include "importer/radio_browser_importer.hpp"

#include <memory>

ImportCommand::ImportCommand() : Command("import", "Import radio stations from different sources"), m_importerByName() {
        auto radioSureImporter = std::unique_ptr<RadioSureImporter>(new RadioSureImporter());
        m_importerByName.insert(std::make_pair(radioSureImporter->getName(), std::move(radioSureImporter)));
        auto radioBrowserImporter = std::unique_ptr<RadioBrowserImporter>(new RadioBrowserImporter());
        m_importerByName.insert(std::make_pair(radioBrowserImporter->getName(), std::move(radioBrowserImporter)));
        m_cli.addOption('i', "input", true, "Input to import stations from. Depends on the type an could be a file or URL.");
        m_cli.addOption('t', "type", true, "Type of imported data. Supported types are: 'radio-sure', which requires a file as input and 'radio-browser', which requires a URL. 'radio-browser' is the default and provides a default url.");
}

ImportCommand::~ImportCommand() { }

void ImportCommand::execute(const std::vector<std::string>& args) {
    m_cli.parse(args);

    if (m_cli.hasOption('h')) {
        LOG(plog::info) << m_cli.usage();
        return;
    }

    // TODO Name should be static, so it must not be hard coded here
    auto importer = m_importerByName.find(m_cli.getValue('t', "radio-browser"));

    if (importer == m_importerByName.end()) {
        LOG(plog::error) << "Unknown importer type " << m_cli.getValue('t') << ". Try " << m_name << " --help for more information.";
    } else {
        auto stationsDao = createStationsDao();
        stationsDao->open(m_cli.getValue('f', getDefaultFile()));
        const std::string input = m_cli.getValue('i', "");
        LOG(plog::info) << "importing from " << input << " using " << importer->second->getName() << " importer";
        importer->second->import(input, stationsDao);
    }
}