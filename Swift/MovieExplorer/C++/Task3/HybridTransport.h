#ifndef HYBRIDTRANSPORT_H
#define HYBRIDTRANSPORT_H

#include "WaterTransport.h"
#include "AirTransport.h"
#include "PassengerTransport.h"
#include <iostream>
#include <string>

class HybridTransport : public WaterTransport, public AirTransport {
private:
    std::string routeNumber;
    unsigned passengerCapacity;
    unsigned currentPassengers;
    double ticketPrice;
    std::string energyType;  
    double maxdistance;
    double maxDistanceHybrid;
public:
//     maxdistance = AirTransport::getMaxDistance() + WaterTransport::getMaxDistance();
    HybridTransport(const std::string& route = "Гибридный маршрут", 
                      unsigned capacity = 60, 
                      double price = 40.0, 
                      const std::string& energy = "Гибридный");

    std::string getRouteNumber() const override;
    unsigned getPassengerCapacity() const override;
    unsigned getCurrentPassengers() const override;
    double getTicketPrice() const override;
    double getMaxDistance() const override;
    bool embarkPassengers(unsigned numPassengers) override;
    void disembarkPassengers(unsigned numPassengers) override;
//     maxdistance = AirTransport::getMaxDistance() + WaterTransport::getMaxDistance();
    std::string getType() const override;
    void displayInfo() const override;
    double calculateRevenue() const override;

    std::string getEnergyType() const;
};

#endif
