#include <iostream>
#include "Bus.h"
#include "Tram.h"
#include "WaterTransport.h"
#include "AirTransport.h"
#include "HybridTransport.h"
using namespace std;

void testTransport(PassengerTransport* transport) {
    transport->displayInfo();
    transport->embarkPassengers(30);
    std::cout << "После посадки:" << std::endl;
    transport->displayInfo();
    transport->disembarkPassengers(15);
    std::cout << "После высадки:" << std::endl;
    transport->displayInfo();
    std::cout << "-------------------------------------------------\n" << std::endl;
}

int main() {
    PassengerTransport* bus = new Bus("Маршрут 1", 40, 25.0, true);
    PassengerTransport* tram = new Tram("Маршрут 2", 100, 18.0, 5);
    PassengerTransport* waterTransport = new WaterTransport("Маршрут водного транспорта", 50, 35.0, 3);
    PassengerTransport* airTransport = new AirTransport("Маршрут воздушного транспорта", 150, 100.0, 2);
    PassengerTransport* hybridTransport = new HybridTransport("Гибридный маршрут", 60, 40.0, "Гибридный");

    testTransport(bus);
    testTransport(tram);
    testTransport(waterTransport);
    testTransport(airTransport);
    testTransport(hybridTransport);
    std::cout << "Воздушный транспорт может пройти максимум: " <<  airTransport->getMaxDistance() << " км" << std::endl;
    std::cout << "Водный транспорт может пройти максимум: " << waterTransport->getMaxDistance() << " км" << std::endl;
    std::cout << "Гибридный транспорт может пройти максимум: " << hybridTransport->getMaxDistance() << " км" << std::endl;

    delete bus;
    delete tram;
    delete waterTransport;
    delete airTransport;
    delete hybridTransport;

    return 0;
}

