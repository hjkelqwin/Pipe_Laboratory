#include "cs.h"
#include <limits>
using namespace std;

int CS::nextId = 1;

CS::CS() : id(0), name(""), totalShops(1), workingShops(0), stationClass(1) {}

CS::CS(const string& name, int totalShops, int workingShops, int stationClass) : id(nextId++), name(name), totalShops(totalShops), workingShops(workingShops), stationClass(stationClass) {}

double CS::getIdlePercent() const {
    return (totalShops - workingShops) * 100.0 / totalShops;
}

bool CS::startShop() {
    if (workingShops >= totalShops) {
        return false;
    }
    ++workingShops;
    return true;
}

bool CS::stopShop() {
    if (workingShops <= 0) {
        return false;
    }
    --workingShops;
    return true;
}

void CS::save(ostream& out) const {
    out << id << '\n' << name << '\n'
        << totalShops << ' ' << workingShops << ' ' << stationClass << '\n';
}

bool CS::load(istream& in) {
    in >> id;
    in.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(in, name);
    in >> totalShops >> workingShops >> stationClass;
    in.ignore(numeric_limits<streamsize>::max(), '\n');
    if (id >= nextId) {
        nextId = id + 1;
    }
    return static_cast<bool>(in) && totalShops > 0;
}

ostream& operator<<(ostream& out, const CS& cs) {
    out << "КС ID " << cs.id << "\n"
        << "  Название: " << cs.name << "\n"
        << "  Всего цехов: " << cs.totalShops << "\n"
        << "  Цехов в работе: " << cs.workingShops << "\n"
        << "  Незадействовано: " << cs.getIdlePercent() << " %\n"
        << "  Класс станции: " << cs.stationClass << "\n";
    return out;
}