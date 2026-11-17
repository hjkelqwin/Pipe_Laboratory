#include "pipe.h"
#include <limits>
using namespace std;

int Pipe::nextId = 1;

Pipe::Pipe() : id(0), name(""), length(1), diameter(1), inRepair(false) {}

Pipe::Pipe(const string& name, double length, double diameter)  : id(nextId++), name(name), length(length), diameter(diameter), inRepair(false) {}

void Pipe::save(ostream& out) const {
    out << id << '\n' << name << '\n' << length << ' ' << diameter << ' ' << inRepair << '\n';
}

bool Pipe::load(istream& in) {
    in >> id;
    in.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(in, name);
    in >> length >> diameter >> inRepair;
    in.ignore(numeric_limits<streamsize>::max(), '\n');
    if (id >= nextId) {
        nextId = id + 1;
    }
    return static_cast<bool>(in);
}

ostream& operator<<(ostream& out, const Pipe& p) {
    out << "Труба ID " << p.id << "\n"
        << "  Название: " << p.name << "\n"
        << "  Длина: " << p.length << " км\n"
        << "  Диаметр: " << p.diameter << " мм\n"
        << "  Статус: " << (p.inRepair ? "В ремонте" : "Работает") << "\n";
    return out;
}