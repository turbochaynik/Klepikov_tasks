#ifndef TRAM_H
#define TRAM_H

#include "PassengerTransport.h"
#include <iostream>
#include <string>

class Tram : public PassengerTransport {
private:
    std::string routeNumber;
    unsigned passengerCapacity;
    unsigned currentPassengers;
    double ticketPrice;
    unsigned numberOfCars;

public:
    Tram(const std::string& route = "Трамвайная линия", unsigned capacity = 100, double price = 18.0, unsigned cars = 3);

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
