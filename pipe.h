#pragma once
#include <iostream>
#include <string>

class Pipe {
    static int nextId;
    int id;
    std::string name;
    double length;
    double diameter;
    bool inRepair;

public:
    Pipe();
    static void resetId() { nextId = 1; }
    Pipe(const std::string& name, double length, double diameter);

    int getId() const { return id; }
    const std::string& getName() const { return name; }
    bool isInRepair() const { return inRepair; }
    void setRepair(bool value) { inRepair = value; }

    void save(std::ostream& out) const;
    bool load(std::istream& in);

    friend std::ostream& operator<<(std::ostream& out, const Pipe& p);
};