#include "WaterTransport.h"

WaterTransport::WaterTransport(const std::string& route, unsigned capacity, double price, unsigned engines)
    : routeNumber(route), passengerCapacity(capacity), currentPassengers(0), ticketPrice(price), numberOfEngines(engines) {
maxDistanceWater = 500.0;
}
std::string WaterTransport::getRouteNumber() const { return routeNumber; }
unsigned WaterTransport::getPassengerCapacity() const { return passengerCapacity; }
unsigned WaterTransport::getCurrentPassengers() const { return currentPassengers; }
double WaterTransport::getTicketPrice() const { return ticketPrice; }

bool WaterTransport::embarkPassengers(unsigned numPassengers) {
    if (currentPassengers + numPassengers <= passengerCapacity) {
        currentPassengers += numPassengers;
        std::cout << "Водный транспорт: Посажено " << numPassengers << " пассажиров." << std::endl;
        return true;
    }
    std::cout << "Водный транспорт: Недостаточно места для " << numPassengers << " пассажиров." << std::endl;
    return false;
}

void WaterTransport::disembarkPassengers(unsigned numPassengers) {
    if (numPassengers > currentPassengers) {
        std::cout << "Водный транспорт: Высажено " << currentPassengers << " пассажиров (все)." << std::endl;
        currentPassengers = 0;
    } else {
        currentPassengers -= numPassengers;
        std::cout << "Водный транспорт: Высажено " << numPassengers << " пассажиров." << std::endl;
    }
}

std::string WaterTransport::getType() const { return "Водный транспорт"; }

void WaterTransport::displayInfo() const {
    std::cout << "Водный транспорт - Маршрут: " << routeNumber
              << ", Вместимость: " << passengerCapacity
              << ", Цена: " << ticketPrice
              << ", Двигателей: " << numberOfEngines << std::endl;
}

double WaterTransport::calculateRevenue() const {
    return currentPassengers * ticketPrice;
}

double WaterTransport::getMaxDistance() const {
    return maxDistanceWater;
}

