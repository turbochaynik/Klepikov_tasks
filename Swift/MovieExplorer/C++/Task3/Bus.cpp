#include "Bus.h"

Bus::Bus(const std::string& route, unsigned capacity, double price, bool wifi)
    : routeNumber(route), passengerCapacity(capacity), currentPassengers(0), ticketPrice(price), hasWifi(wifi) {}

std::string Bus::getRouteNumber() const { return routeNumber; }
unsigned Bus::getPassengerCapacity() const { return passengerCapacity; }
unsigned Bus::getCurrentPassengers() const { return currentPassengers; }
double Bus::getTicketPrice() const { return ticketPrice; }

bool Bus::embarkPassengers(unsigned numPassengers) {
    if (currentPassengers + numPassengers <= passengerCapacity) {
        currentPassengers += numPassengers;
        std::cout << "Автобус: Посажено " << numPassengers << " пассажиров." << std::endl;
        return true;
    }
    std::cout << "Автобус: Недостаточно места для " << numPassengers << " пассажиров." << std::endl;
    return false;
}

void Bus::disembarkPassengers(unsigned numPassengers) {
    if (numPassengers > currentPassengers) {
        std::cout << "Автобус: Высажено " << currentPassengers << " пассажиров (все)." << std::endl;
        currentPassengers = 0;
    } else {
        currentPassengers -= numPassengers;
        std::cout << "Автобус: Высажено " << numPassengers << " пассажиров." << std::endl;
    }
}

std::string Bus::getType() const { return "Автобус"; }

void Bus::displayInfo() const {
    std::cout << "Автобус - Маршрут: " << routeNumber
              << ", Вместимость: " << passengerCapacity
              << ", Цена: " << ticketPrice
              << ", Wi-Fi: " << (hasWifi ? "Да" : "Нет") << std::endl;
}

double Bus::calculateRevenue() const {
    return currentPassengers * ticketPrice;
}

double Bus::getMaxDistance() const {
    return 100.0;
}
