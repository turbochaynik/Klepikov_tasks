#include "HybridTransport.h"

HybridTransport::HybridTransport(const std::string& route, unsigned capacity, double price, const std::string& energy)
    : routeNumber(route), passengerCapacity(capacity), currentPassengers(0), ticketPrice(price), energyType(energy) {
// maxdistance = AirTransport::getMaxDistance() + WaterTransport::getMaxDistance();
    maxDistanceHybrid = maxDistanceAir + maxDistanceWater;

}
std::string HybridTransport::getRouteNumber() const { return routeNumber; }
unsigned HybridTransport::getPassengerCapacity() const { return passengerCapacity; }
unsigned HybridTransport::getCurrentPassengers() const { return currentPassengers; }
double HybridTransport::getTicketPrice() const { return ticketPrice; }

bool HybridTransport::embarkPassengers(unsigned numPassengers) {
    if (currentPassengers + numPassengers <= passengerCapacity) {
        currentPassengers += numPassengers;
        std::cout << "Гибридный транспорт: Посажено " << numPassengers << " пассажиров." << std::endl;
        return true;
    }
    std::cout << "Гибридный транспорт: Недостаточно места для " << numPassengers << " пассажиров." << std::endl;
    return false;
}

void HybridTransport::disembarkPassengers(unsigned numPassengers) {
    if (numPassengers > currentPassengers) {
        std::cout << "Гибридный транспорт: Высажено " << currentPassengers << " пассажиров (все)." << std::endl;
        currentPassengers = 0;
    } else {
        currentPassengers -= numPassengers;
        std::cout << "Гибридный транспорт: Высажено " << numPassengers << " пассажиров." << std::endl;
    }
}

std::string HybridTransport::getType() const { return "Гибридный транспорт"; }

void HybridTransport::displayInfo() const {
    std::cout << "Гибридный транспорт - Маршрут: " << routeNumber
              << ", Вместимость: " << passengerCapacity
              << ", Цена: " << ticketPrice
              << ", Тип энергии: " << energyType << std::endl;
}

double HybridTransport::calculateRevenue() const {
    return currentPassengers * ticketPrice;
}

std::string HybridTransport::getEnergyType() const { return energyType; }
double HybridTransport::getMaxDistance() const {
    return maxDistanceHybrid;
}

