#include <iostream>
#include <string>
#include <limits>
#include <clocale>
#include <iomanip>

using namespace std;

struct Pipe {
    string name;
    float length_km;
    int diameter_mm;
    bool is_work;
};

struct CompressorStation {
    string name;
    int total;
    int active_total;
    int station_class;
};

int getValidInt(const string& prompt) {
    int value;
    cout << prompt;
    while (!(cin >> value)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Ошибка ввода. Введите целое число: ";
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return value;
}

float getValidFloat(const string& prompt) {
    float value;
    cout << prompt;
    while (!(cin >> value)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Ошибка ввода. Введите число: ";
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return value;
}

// Вынесены из getValidFloat!
float getValidPositiveFloat(const string& prompt) {
    float value;
    while (true) {
        value = getValidFloat(prompt);
        if (value > 0) {
            return value;
        }
        cout << "Ошибка! Значение должно быть положительным. Попробуйте снова: ";
    }
}

int getValidPositiveInt(const string& prompt) {
    int value;
    while (true) {
        value = getValidInt(prompt);
        if (value > 0) {
            return value;
        }
        cout << "Ошибка! Значение должно быть положительным. Попробуйте снова: ";
    }
}

void displayMenu() {
    cout << "\n=== Система управления трубопроводом ===" << endl;
    cout << "1. Добавить трубу" << endl;
    cout << "2. Добавить КС" << endl;
    cout << "3. Просмотр всех объектов" << endl;
    cout << "4. Редактировать трубу" << endl;
    cout << "5. Редактировать КС" << endl;
    cout << "6. Сохранить в файл" << endl;
    cout << "7. Загрузить из файла" << endl;
    cout << "0. Выход" << endl;
    cout << "Выберите опцию: ";
}

void addPipe(Pipe& pipe) {
    cout << "\n=== Добавление новой трубы ===" << endl;
    cout << "Введите километровую отметку: ";
    getline(cin, pipe.name);
    pipe.length_km = getValidPositiveFloat("Введите длину трубы (км): ");
    pipe.diameter_mm = getValidPositiveInt("Введите диаметр трубы (мм): ");
    pipe.is_work = true;
    cout << "Труба успешно добавлена!" << endl;
}


void addCompressorStation(CompressorStation& cs) {
    cout << "\n=== Добавление новой КС ===" << endl;
    cout << "Введите название КС: ";
    getline(cin, cs.name);
    cs.total = getValidPositiveInt("Введите общее количество цехов: ");

    while (true) {
        cs.active_total = getValidInt("Введите количество работающих цехов: ");
        if (cs.active_total >= 0 && cs.active_total <= cs.total) {
            break;
        }
        cout << "Ошибка! Количество работающих цехов не может превышать общее количество ("
            << cs.total << ") и не может быть отрицательным. Попробуйте снова." << endl;
    }

    cs.station_class = getValidPositiveInt("Введите класс станции: ");
    cout << "Компрессорная станция успешно добавлена!" << endl;
}


void displayAll(const Pipe& pipe, const CompressorStation& cs) {
    cout << "\n=== Просмотр всех объектов ===" << endl;

    // Вывод информации о трубе
    cout << "--- Труба ---" << endl;
    if (pipe.name.empty()) {
        cout << "Труба не добавлена" << endl;
    }
    else {
        cout << "Километровая отметка: " << pipe.name << endl;
        cout << "Длина: " << pipe.length_km << " км" << endl;
        cout << "Диаметр: " << pipe.diameter_mm << " мм" << endl;
        cout << "Состояние: " << (pipe.is_work ? "В работе" : "В ремонте") << endl;
    }

    // Вывод информации о компрессорной станции
    cout << "\n--- Компрессорная станция ---" << endl;
    if (cs.name.empty()) {
        cout << "КС не добавлена" << endl;
    }
    else {
        cout << "Название: " << cs.name << endl;
        cout << "Общее количество цехов: " << cs.total << endl;
        cout << "Работающих цехов: " << cs.active_total << endl;
        cout << "Класс станции: " << cs.station_class << endl;
        cout << "Процент работающих цехов: "
            << fixed << setprecision(1)
            << (cs.total > 0 ? (100.0 * cs.active_total / cs.total) : 0)
            << "%" << endl;
    }
}

void editPipe(Pipe& pipe) {
    cout << "Ждем следующего коммита" << endl;
}

void editCompressorStation(CompressorStation& cs) {
    cout << "Ждем следующего коммита" << endl;
}

void saveToFile(const Pipe& pipe, const CompressorStation& cs) {
    cout << "Ждем следующего коммита" << endl;
}

void loadFromFile(Pipe& pipe, CompressorStation& cs) {
    cout << "Ждем следующего коммита" << endl;
}

int main() {
    setlocale(LC_ALL, "Russian");

    Pipe pipe;
    CompressorStation cs;
    int choice;

    while (true) {
        displayMenu();
        choice = getValidInt("");

        switch (choice) {
        case 1:
            addPipe(pipe);
            break;
        case 2:
            addCompressorStation(cs);
            break;
        case 3:
            displayAll(pipe, cs);
            break;
        case 4:
            editPipe(pipe);
            break;
        case 5:
            editCompressorStation(cs);
            break;
        case 6:
            saveToFile(pipe, cs);
            break;
        case 7:
            loadFromFile(pipe, cs);
            break;
        case 0:
            cout << "Выход из программы..." << endl;
            return 0;
        default:
            cout << "Неверный выбор. Пожалуйста, выберите от 0 до 7." << endl;
        }
    }

    return 0;
}