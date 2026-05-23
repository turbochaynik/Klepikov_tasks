#ifndef PASSENGERTRANSPORT_H
#define PASSENGERTRANSPORT_H

#include <string>
#include <iostream>

class PassengerTransport {
public:
    virtual ~PassengerTransport() {}
    virtual std::string getRouteNumber() const = 0;
    virtual unsigned getPassengerCapacity() const = 0;
    virtual unsigned getCurrentPassengers() const = 0;
    virtual double getTicketPrice() const = 0;
    virtual bool embarkPassengers(unsigned numPassengers) = 0;
    virtual void disembarkPassengers(unsigned numPassengers) = 0;
    virtual std::string getType() const = 0;
    virtual void displayInfo() const = 0;
    virtual double calculateRevenue() const = 0;
    
    virtual double getMaxDistance() const = 0;
};

#endif
