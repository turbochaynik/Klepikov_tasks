#ifndef BUS_H
#define BUS_H

#include "PassengerTransport.h"
#include <iostream>
#include <string>

class Bus : public PassengerTransport {
private:
    std::string routeNumber;
    unsigned passengerCapacity;
    unsigned currentPassengers;
    double ticketPrice;
    bool hasWifi;

public:
    Bus(const std::string& route = "Городской", unsigned capacity = 40, double price = 25.0, bool wifi = false);

    std::string getRouteNumber() const override;
    unsigned getPassengerCapacity() const override;
    unsigned getCurrentPassengers() const override;
    double getTicketPrice() const override;
    bool embarkPassengers(unsigned numPassengers) override;
    void disembarkPassengers(unsigned numPassengers) override;
    std::string getType() const override;
    void displayInfo() const override;
    double calculateRevenue() const override;
    double getMaxDistance() const override;
};

#endif 

