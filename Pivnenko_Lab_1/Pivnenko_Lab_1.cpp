#include <iostream>
#include <string>
#include <limits>
#include <clocale>
#include <iomanip>
#include <fstream>
#include <stdexcept>

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

//Валидация =======================
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

bool getYesNoInput(const string& prompt) {
    string input;
    while (true) {
        cout << prompt << " (y/n): ";
        getline(cin, input);
        if (input == "y" || input == "Y") {
            return true;
        }
        else if (input == "n" || input == "N") {
            return false;
        }
        cout << "Ошибка! Введите 'y' или 'n': ";
    }
}
//====================================================

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
    if (pipe.name.empty()) {
        cout << "Ошибка! Труба не добавлена. Сначала добавьте трубу." << endl;
        return;
    }

    cout << "\n=== Редактирование трубы ===" << endl;
    cout << "Текущие данные:" << endl;
    cout << "Километровая отметка: " << pipe.name << endl;
    cout << "Длина: " << pipe.length_km << " км" << endl;
    cout << "Диаметр: " << pipe.diameter_mm << " мм" << endl;
    cout << "Состояние: " << (pipe.is_work ? "В работе" : "В ремонте") << endl;

    cout << "\nЧто вы хотите изменить?" << endl;
    cout << "1. Изменить статус ремонта" << endl;
    cout << "2. Изменить все параметры" << endl;
    cout << "0. Отмена" << endl;
    cout << "Выберите опцию: ";

    int choice = getValidInt("");
    switch (choice) {
    case 1:
        pipe.is_work = !pipe.is_work;
        cout << "Статус трубы изменен на: "
            << (pipe.is_work ? "В работе" : "В ремонте") << endl;
        break;
    case 2:
        cout << "Введите новую километровую отметку: ";
        getline(cin, pipe.name);
        pipe.length_km = getValidPositiveFloat("Введите новую длину трубы (км): ");
        pipe.diameter_mm = getValidPositiveInt("Введите новый диаметр трубы (мм): ");
        cout << "Данные трубы успешно обновлены!" << endl;
        break;
    case 0:
        cout << "Редактирование отменено." << endl;
        break;
    default:
        cout << "Неверный выбор. Редактирование отменено." << endl;
    }
}

void editCompressorStation(CompressorStation& cs) {
    if (cs.name.empty()) {
        cout << "Ошибка! КС не добавлена. Сначала добавьте КС." << endl;
        return;
    }

    cout << "\n=== Редактирование КС ===" << endl;
    cout << "Текущие данные:" << endl;
    cout << "Название: " << cs.name << endl;
    cout << "Всего цехов: " << cs.total << endl;
    cout << "Работающих цехов: " << cs.active_total << endl;
    cout << "Класс станции: " << cs.station_class << endl;

    cout << "\nЧто вы хотите сделать?" << endl;
    cout << "1. Запустить цех" << endl;
    cout << "2. Остановить цех" << endl;
    cout << "3. Изменить все параметры" << endl;
    cout << "0. Отмена" << endl;
    cout << "Выберите опцию: ";

    int choice = getValidInt("");
    switch (choice) {
    case 1:
        if (cs.active_total < cs.total) {
            cs.active_total++;
            cout << "Цех запущен. Теперь работает " << cs.active_total
                << " из " << cs.total << " цехов." << endl;
        }
        else {
            cout << "Ошибка! Все цехи уже работают." << endl;
        }
        break;
    case 2:
        if (cs.active_total > 0) {
            cs.active_total--;
            cout << "Цех остановлен. Теперь работает " << cs.active_total
                << " из " << cs.total << " цехов." << endl;
        }
        else {
            cout << "Ошибка! Нет работающих цехов для остановки." << endl;
        }
        break;
    case 3:
        cout << "Введите новое название КС: ";
        getline(cin, cs.name);
        cs.total = getValidPositiveInt("Введите новое общее количество цехов: ");

        while (true) {
            cs.active_total = getValidInt("Введите новое количество работающих цехов: ");
            if (cs.active_total >= 0 && cs.active_total <= cs.total) {
                break;
            }
            cout << "Ошибка! Количество работающих цехов не может превышать общее количество ("
                << cs.total << ") и не может быть отрицательным. Попробуйте снова." << endl;
        }

        cs.station_class = getValidPositiveInt("Введите новый класс станции: ");
        cout << "Данные КС успешно обновлены!" << endl;
        break;
    case 0:
        cout << "Редактирование отменено." << endl;
        break;
    default:
        cout << "Неверный выбор. Редактирование отменено." << endl;
    }
}

void saveToFile(const Pipe& pipe, const CompressorStation& cs) {
    ofstream outFile("data.txt");
    if (!outFile.is_open()) {
        cout << "Ошибка! Не удалось открыть файл для записи." << endl;
        return;
    }

    outFile << "PIPE" << endl;
    outFile << pipe.name << endl;
    outFile << pipe.length_km << endl;
    outFile << pipe.diameter_mm << endl;
    outFile << pipe.is_work << endl;

    outFile << "STATION" << endl;
    outFile << cs.name << endl;
    outFile << cs.total << endl;
    outFile << cs.active_total << endl;
    outFile << cs.station_class << endl;

    outFile.close();
    cout << "Данные успешно сохранены в файл 'data.txt'" << endl;
}

void loadFromFile(Pipe& pipe, CompressorStation& cs) {
    ifstream inFile("data.txt");
    if (!inFile.is_open()) {
        cout << "Ошибка! Файл 'data.txt' не найден." << endl;
        return;
    }

    string line;
    string section;

    try {
        getline(inFile, section);
        if (section != "PIPE") throw runtime_error("Неверный формат файла");

        getline(inFile, pipe.name);
        getline(inFile, line);
        pipe.length_km = stof(line);
        getline(inFile, line);
        pipe.diameter_mm = stoi(line);
        getline(inFile, line);
        pipe.is_work = (line == "1");

        getline(inFile, section);
        if (section != "STATION") throw runtime_error("Неверный формат файла");

        getline(inFile, cs.name);
        getline(inFile, line);
        cs.total = stoi(line);
        getline(inFile, line);
        cs.active_total = stoi(line);
        getline(inFile, line);
        cs.station_class = stoi(line);

        inFile.close();
        cout << "Данные успешно загружены из файла 'data.txt'" << endl;

    }
    catch (const exception& e) {
        cout << "Ошибка при загрузке файла: " << e.what() << endl;
        inFile.close();
        pipe = Pipe();
        cs = CompressorStation();
    }
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