#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <unordered_map>
#include <string>
#include <vector>
#include <unistd.h>
#include "cs.h"
#include "pipe.h"
#include <set>
using namespace std;

string currentLog;
bool fromFile = !isatty(0);

void log(const string& message) {
    ofstream file(currentLog, ios::app);
    file << message << "\n";
}

void makeLogsFolder() {
    system("mkdir -p logs");
}

string makeLogName() {
    int number = 1;
    while (true) {
        string name = "logs/log_" + to_string(number) + ".txt";
        ifstream file(name);
        if (!file.is_open()) return name;
        number++;
    }
}

int readInt(string message) {
    int value;
    while (true) {
        cout << message;
        if (cin >> value && value >= 0) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            log(to_string(value));
            if (fromFile) cout << value << "\n";
            return value;
        }
        if (cin.eof()) {
            cout << "Ввод закончился\n";
            exit(0);
        }
        cout << " Ошибка ввода. Введите целое число (не меньше 0)\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

double readDouble(string message) {
    double value;
    while (true) {
        cout << message;
        if (cin >> value && value > 0) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            log(to_string(value));
            if (fromFile) cout << value << "\n";
            return value;
        }
        if (cin.eof()) {
            cout << "Ввод закончился\n";
            exit(0);
        }
        cout << " Ошибка ввода. Введите число больше 0\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

string readLine(string message) {
    string line;
    cout << message;
    getline(cin, line);
    log(line);
    if (fromFile) cout << line << "\n";
    return line;
}

void addPipe(unordered_map<int, Pipe>& pipes) {
    string name = readLine("Введите название трубы: ");
    double length = readDouble("Введите длину (км): ");
    double diameter = readDouble("Введите диаметр (мм): ");
    Pipe p(name, length, diameter);
    pipes[p.getId()] = p;
    cout << "Труба создана, ID = " << p.getId() << "\n";
}

void addCS(unordered_map<int, CS>& stations) {
    string name = readLine("Введите название КС: ");
    int total = readInt("Введите общее количество цехов: ");
    while (total == 0) total = readInt(" Цехов должно быть больше 0. Повторите ввод: ");
    int working = readInt("Введите количество цехов в работе: ");
    while (working > total) working = readInt(" Работающих цехов больше, чем всего. Повторите ввод: ");
    int stationClass = readInt("Введите класс станции: ");
    CS cs(name, total, working, stationClass);
    stations[cs.getId()] = cs;
    cout << "КС создана, ID = " << cs.getId() << "\n";
}

void showAll(const unordered_map<int, Pipe>& pipes, const unordered_map<int, CS>& stations) {
    cout << "\nТРУБЫ\n";
    for (const auto& item : pipes) cout << item.second << "\n";
    cout << "КОМПРЕССОРНЫЕ СТАНЦИИ\n";
    for (const auto& item : stations) cout << item.second << "\n";
}

void editPipe(unordered_map<int, Pipe>& pipes) {
    int id = readInt("Введите ID трубы: ");
    if (!pipes.count(id)) {
        cout << " Трубы с таким ID нет\n";
        return;
    }
    Pipe& p = pipes[id];
    p.setRepair(!p.isInRepair());
    cout << "Новый статус: " << (p.isInRepair() ? "В ремонте" : "Работает") << "\n";
}

void editCS(unordered_map<int, CS>& stations) {
    int id = readInt("Введите ID КС: ");
    if (!stations.count(id)) {
        cout << " КС с таким ID нет\n";
        return;
    }
    int choice = readInt("1 - запустить цех, 2 - остановить цех: ");
    bool ok = false;
    if (choice == 1) ok = stations[id].startShop();
    else if (choice == 2) ok = stations[id].stopShop();
    cout << (ok ? "Готово\n" : "Действие невозможно\n");
}

void deletePipe(unordered_map<int, Pipe>& pipes) {
    int id = readInt("Введите ID трубы: ");
    cout << (pipes.erase(id) ? "Труба удалена\n" : " Трубы с таким ID нет\n");
}

void deleteCS(unordered_map<int, CS>& stations) {
    int id = readInt("Введите ID КС: ");
    cout << (stations.erase(id) ? "КС удалена\n" : " КС с таким ID нет\n");
}

template<typename Item, typename T>
using Filter = bool(*)(const Item& item, T param);

bool CheckPipeByName(const Pipe& p, string param) {
    return p.getName().find(param) != string::npos;
}

bool CheckPipeByRepair(const Pipe& p, bool param) {
    return p.isInRepair() == param;
}

bool CheckCSByName(const CS& c, string param) {
    return c.getName().find(param) != string::npos;
}

bool CheckCSByIdle(const CS& c, pair<double, double> param) {
    return c.getIdlePercent() >= param.first && c.getIdlePercent() <= param.second;
}

template<typename Item, typename T>
set<int> FindByFilter(const unordered_map<int, Item>& items, Filter<Item, T> f, T param) {
    set<int> res;
    for (const auto& item : items) {
        if (f(item.second, param)) 
            res.push_back(item.first);
    }
    return res;
}

set<int> findPipes(const unordered_map<int, Pipe>& pipes) {
    int mode = readInt("Фильтр: 1 - по названию, 2 - по признаку \"в ремонте\": ");
    if (mode == 1) {
        string name = readLine("Введите название (или его часть): ");
        return FindByFilter(pipes, CheckPipeByName, name);
    }
    if (mode == 2) {
        bool repair = readInt("1 - в ремонте, 0 - работает: ") == 1;
        return FindByFilter(pipes, CheckPipeByRepair, repair);
    }
    cout << " Неверный фильтр\n";
    return set<int>();
}

set<int> findCS(const unordered_map<int, CS>& stations) {
    int mode = readInt("Фильтр: 1 - по названию, 2 - по проценту незадействованных цехов: ");
    if (mode == 1) {
        string name = readLine("Введите название (или его часть): ");
        return FindByFilter(stations, CheckCSByName, name);
    }
    if (mode == 2) {
        double from = readInt("Процент незадействованных цехов от: ");
        double to = readInt("Процент незадействованных цехов до: ");
        pair<double, double> range(from, to);
        return FindByFilter(stations, CheckCSByIdle, range);
    }
    cout << " Неверный фильтр\n";
    return set<int>();
}

void searchPipes(const unordered_map<int, Pipe>& pipes) {
    set<int> found = findPipes(pipes);
    if (found.empty()) 
        cout << "Ничего не найдено\n";
    for (int id : found) 
        cout << pipes.at(id) << "\n";
}

void searchCS(const unordered_map<int, CS>& stations) {
    set<int> found = findCS(stations);
    if (found.empty()) cout << "Ничего не найдено\n";
    for (int id : found) 
        cout << stations.at(id) << "\n";
}

set<int> selectPipes(const set<int>& found) {
    if (readInt("1 - все найденные, 2 - выбрать самостоятельно: ") == 1) 
        return found;
    set<int> selected;
    cout << "Вводите ID по одному, 0 - закончить\n";
    while (true) {
        int id = readInt("ID: ");
        if (id == 0) 
            break;
        for (int x : found) {
            if (x == id) {
                selected.emplace(id);
                break;
            }
            else cout << " Этого ID нет среди найденных\n";
        }
    }
    return selected;
}

void batchEditPipes(unordered_map<int, Pipe>& pipes) {
    set<int> found = findPipes(pipes);
    for (int id : found) cout << pipes.at(id) << "\n";
    if (found.empty()) {
        cout << "Ничего не найдено\n";
        return;
    }
    set<int> selected = selectPipes(found);
    int action = readInt("Действие: 1 - в ремонт, 2 - вернуть в работу, 3 - удалить: ");
    for (int id : selected) {
        if (action == 3) pipes.erase(id);
        else pipes[id].setRepair(action == 1);
    }
    cout << "Обработано труб: " << selected.size() << "\n";
}

void saveData(const unordered_map<int, Pipe>& pipes, const unordered_map<int, CS>& stations) {
    ofstream out(readLine("Введите имя файла для сохранения: "));
    if (!out.is_open()) {
        cout << " Ошибка открытия файла\n";
        return;
    }
    out << pipes.size() << "\n";
    for (const auto& item : pipes) item.second.save(out);
    out << stations.size() << "\n";
    for (const auto& item : stations) item.second.save(out);
    cout << "Данные сохранены\n";
}

void loadData(unordered_map<int, Pipe>& pipes, unordered_map<int, CS>& stations) {
    ifstream in(readLine("Введите имя файла для загрузки: "));
    if (!in.is_open()) {
        cout << " Файл не найден\n";
        return;
    }
    unordered_map<int, Pipe> newPipes;
    unordered_map<int, CS> newStations;
    int count;
    in >> count;
    in.ignore(numeric_limits<streamsize>::max(), '\n');
    for (int i = 0; i < count; i++) {
        Pipe p;
        if (!p.load(in)) { cout << " Файл повреждён\n"; return; }
        newPipes[p.getId()] = p;
    }
    in >> count;
    in.ignore(numeric_limits<streamsize>::max(), '\n');
    for (int i = 0; i < count; i++) {
        CS cs;
        if (!cs.load(in)) { cout << " Файл повреждён\n"; return; }
        newStations[cs.getId()] = cs;
    }
    pipes = newPipes;
    stations = newStations;
    cout << "Данные загружены\n";
}

bool processMenu(int choice, unordered_map<int, Pipe>& pipes, unordered_map<int, CS>& stations) {
    switch (choice) {
        case 1:  addPipe(pipes); break;
        case 2:  addCS(stations); break;
        case 3:  showAll(pipes, stations); break;
        case 4:  editPipe(pipes); break;
        case 5:  editCS(stations); break;
        case 6:  deletePipe(pipes); break;
        case 7:  deleteCS(stations); break;
        case 8:  searchPipes(pipes); break;
        case 9:  searchCS(stations); break;
        case 10: batchEditPipes(pipes); break;
        case 11: saveData(pipes, stations); break;
        case 12: loadData(pipes, stations); break;
        case 0:  return false;
        default: cout << " Неверный выбор. Введите число от 0 до 12\n";
    }
    return true;
}

int main() {
    makeLogsFolder();
    currentLog = makeLogName();
    unordered_map<int, Pipe> pipes;
    unordered_map<int, CS> stations;

    while (true) {
        cout << "\nМЕНЮ\n1. Добавить трубу\n2. Добавить КС\n3. Просмотр всех объектов\n"
             << "4. Редактировать трубу\n5. Редактировать КС\n6. Удалить трубу\n7. Удалить КС\n"
             << "8. Поиск труб\n9. Поиск КС\n10. Пакетное редактирование труб\n"
             << "11. Сохранить\n12. Загрузить\n0. Выход\n";
        if (!processMenu(readInt("Выберите действие: "), pipes, stations)) return 0;
    }
}