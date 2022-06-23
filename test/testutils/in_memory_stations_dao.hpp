#pragma once

#include "db/stations_dao.hpp"
#include "db/station.hpp"

#include <memory>
#include <map>

class InMemoryStationsDao : public StationsDao {
    public:
        InMemoryStationsDao() = default;
        virtual ~InMemoryStationsDao() = default;
        void save(Station& station) override;
        std::vector<long> find(const std::string& name, const std::string& genre, const std::string& country) override;
        std::shared_ptr<Station> findById(const long id) override;
        void deleteAllAddedBy(const std::string& addedBy) override;
        std::shared_ptr<Station> getRandom() override;
        std::vector<long> getAllIds() override;
    
    protected:
        void onOpen() override;
        bool onClose() override;

    private:
        long m_id;
        std::map<long, std::shared_ptr<Station>> m_stations;
};
