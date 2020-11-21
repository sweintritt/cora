#include <CppUTest/CommandLineTestRunner.h>
#include <plog/Log.h>
#include <plog/Appenders/ConsoleAppender.h>

IMPORT_TEST_GROUP(SqliteStationsDaoTest);

int main(int argc, char* argv[]) {
    try {
        static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
        plog::init(plog::debug, &consoleAppender);
        LOG(plog::info) << "Starting cora";
        return CommandLineTestRunner::RunAllTests(argc, argv);
    } catch (const std::exception& error) {
        LOG(plog::error) << error.what();
    } catch (const std::string& error) {
        LOG(plog::error) << error;
    } catch (const char* error) {
        LOG(plog::error) << error;
    } catch (...) {
        LOG(plog::error) << "error of unknown type";
    }

    return 1;
}