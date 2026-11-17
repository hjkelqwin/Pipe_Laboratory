#pragma once
#include <iostream>
#include <string>

class CS {
    static int nextId;
    int id;
    std::string name;
    int totalShops;
    int workingShops;
    int stationClass;

public:
    CS();
    static void resetId() { nextId = 1; }
    CS(const std::string& name, int totalShops, int workingShops, int stationClass);

    int getId() const { return id; }
    const std::string& getName() const { return name; }
    double getIdlePercent() const;

    bool startShop();
    bool stopShop();

    void save(std::ostream& out) const;
    bool load(std::istream& in);

    friend std::ostream& operator<<(std::ostream& out, const CS& cs);
};