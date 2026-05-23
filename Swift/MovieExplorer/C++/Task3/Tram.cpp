#include "Tram.h"

Tram::Tram(const std::string& route, unsigned capacity, double price, unsigned cars)
    : routeNumber(route), passengerCapacity(capacity), currentPassengers(0), ticketPrice(price), numberOfCars(cars) {}

std::string Tram::getRouteNumber() const { return routeNumber; }
unsigned Tram::getPassengerCapacity() const { return passengerCapacity; }
unsigned Tram::getCurrentPassengers() const { return currentPassengers; }
double Tram::getTicketPrice() const { return ticketPrice; }

bool Tram::embarkPassengers(unsigned numPassengers) {
    if (currentPassengers + numPassengers <= passengerCapacity) {
        currentPassengers += numPassengers;
        std::cout << "Трамвай: Посажено " << numPassengers << " пассажиров." << std::endl;
        return true;
    }
    std::cout << "Трамвай: Недостаточно места для " << numPassengers << " пассажиров." << std::endl;
    return false;
}

void Tram::disembarkPassengers(unsigned numPassengers) {
    if (numPassengers > currentPassengers) {
        std::cout << "Трамвай: Высажено " << currentPassengers << " пассажиров (все)." << std::endl;
        currentPassengers = 0;
    } else {
        currentPassengers -= numPassengers;
        std::cout << "Трамвай: Высажено " << numPassengers << " пассажиров." << std::endl;
    }
}

std::string Tram::getType() const { return "Трамвай"; }

void Tram::displayInfo() const {
    std::cout << "Трамвай - Маршрут: " << routeNumber
              << ", Вместимость: " << passengerCapacity
              << ", Цена: " << ticketPrice
              << ", Вагонов: " << numberOfCars << std::endl;
}

double Tram::calculateRevenue() const {
    return currentPassengers * ticketPrice;
}

double Tram::getMaxDistance() const {
    return 400.0;
}
