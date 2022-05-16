#pragma once

#include "stations_dao.hpp"

class RadioBrowserInfoStationsDao : public StationsDao {
    public:
        RadioBrowserInfoStationsDao();

        virtual ~RadioBrowserInfoStationsDao() override;

        virtual void open(const std::string& url) override;

        virtual void close() override;

        virtual void save(Station& station) override;

        virtual std::vector<long> find(const std::string& name, const std::string& genre, const std::string& country) override;

        virtual std::shared_ptr<Station> findById(const long id) override;

        virtual void deleteAllAddedBy(const std::string& addedBy) override;

        virtual std::shared_ptr<Station> getRandom() override;

        virtual int getVersion() override;

        virtual void create() override;

        virtual void upgrade(const int oldVersion, const int newVersion) override;

        virtual std::vector<long> getAllIds() override;

        virtual void beginTransaction() override;

        virtual void commit() override;

        virtual void rollback() override;

    private:
        int version;

        std::string file;
};
