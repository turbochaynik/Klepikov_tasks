#ifndef WATERTRANSPORT_H
#define WATERTRANSPORT_H

#include "PassengerTransport.h"
#include <iostream>
#include <string>

class WaterTransport : virtual public PassengerTransport {
protected:
    double maxDistanceWater;
private:
    std::string routeNumber;
    unsigned passengerCapacity;
    unsigned currentPassengers;
    double ticketPrice;
    unsigned numberOfEngines;

public:
    WaterTransport(const std::string& route = "Морской маршрут", unsigned capacity = 100, double price = 30.0, unsigned engines = 2);

    std::string getRouteNumber() const override;
    unsigned getPassengerCapacity() const override;
    unsigned getCurrentPassengers() const override;
    double getTicketPrice() const override;
    double getMaxDistance() const override;
    bool embarkPassengers(unsigned numPassengers) override;
    void disembarkPassengers(unsigned numPassengers) override;

    std::string getType() const override;
    void displayInfo() const override;
    double calculateRevenue() const override;
};

#endif
