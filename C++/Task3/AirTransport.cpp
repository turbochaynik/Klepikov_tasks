#include "AirTransport.h"

AirTransport::AirTransport(const std::string& route, unsigned capacity, double price, unsigned engines)
    : routeNumber(route), passengerCapacity(capacity), currentPassengers(0), ticketPrice(price), numberOfEngines(engines) {
maxDistanceAir = 1200;
}
std::string AirTransport::getRouteNumber() const { return routeNumber; }
unsigned AirTransport::getPassengerCapacity() const { return passengerCapacity; }
unsigned AirTransport::getCurrentPassengers() const { return currentPassengers; }
double AirTransport::getTicketPrice() const { return ticketPrice; }

bool AirTransport::embarkPassengers(unsigned numPassengers) {
    if (currentPassengers + numPassengers <= passengerCapacity) {
        currentPassengers += numPassengers;
        std::cout << "Воздушный транспорт: Посажено " << numPassengers << " пассажиров." << std::endl;
        return true;
    }
    std::cout << "Воздушный транспорт: Недостаточно места для " << numPassengers << " пассажиров." << std::endl;
    return false;
}

void AirTransport::disembarkPassengers(unsigned numPassengers) {
    if (numPassengers > currentPassengers) {
        std::cout << "Воздушный транспорт: Высажено " << currentPassengers << " пассажиров (все)." << std::endl;
        currentPassengers = 0;
    } else {
        currentPassengers -= numPassengers;
        std::cout << "Воздушный транспорт: Высажено " << numPassengers << " пассажиров." << std::endl;
    }
}

std::string AirTransport::getType() const { return "Воздушный транспорт"; }

void AirTransport::displayInfo() const {
    std::cout << "Воздушный транспорт - Маршрут: " << routeNumber
              << ", Вместимость: " << passengerCapacity
              << ", Цена: " << ticketPrice
              << ", Двигателей: " << numberOfEngines << std::endl;
}

double AirTransport::calculateRevenue() const {
    return currentPassengers * ticketPrice;
}
double AirTransport::getMaxDistance() const {
    return maxDistanceAir;
}
