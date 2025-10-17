#ifndef AIRTRANSPORT_H
#define AIRTRANSPORT_H

#include "PassengerTransport.h"
#include <iostream>
#include <string>


class AirTransport : virtual public PassengerTransport {
protected:
        double maxDistanceAir;
private:
    std::string routeNumber;
    unsigned passengerCapacity;
    unsigned currentPassengers;
    double ticketPrice;
    unsigned numberOfEngines;

public:
    AirTransport(const std::string& route = "Воздушный маршрут", unsigned capacity = 200, double price = 150.0, unsigned engines = 4);

    std::string getRouteNumber() const override;
    unsigned getPassengerCapacity() const override;
    unsigned getCurrentPassengers() const override;
    double getTicketPrice() const override;

    bool embarkPassengers(unsigned numPassengers) override;
    void disembarkPassengers(unsigned numPassengers) override;
    double getMaxDistance() const override;
    std::string getType() const override;
    void displayInfo() const override;
    double calculateRevenue() const override;
};

#endif
